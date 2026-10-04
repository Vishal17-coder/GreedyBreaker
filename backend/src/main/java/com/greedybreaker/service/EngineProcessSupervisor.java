package com.greedybreaker.service;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.greedybreaker.config.EngineProperties;
import com.greedybreaker.dto.engine.EngineOutputDTO;
import com.greedybreaker.dto.engine.EngineSearchConfigDTO;
import com.greedybreaker.exception.EngineExecutionException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.TimeUnit;

@Service
public class EngineProcessSupervisor {

    private static final Logger log = LoggerFactory.getLogger(EngineProcessSupervisor.class);

    private final EngineProperties engineProperties;
    private final ObjectMapper objectMapper;

    public EngineProcessSupervisor(EngineProperties engineProperties, ObjectMapper objectMapper) {
        this.engineProperties = engineProperties;
        this.objectMapper = objectMapper;
    }

    public static class ProcessExecutionResult {
        private final boolean timedOut;
        private final int exitCode;
        private final EngineOutputDTO output;
        private final String stdout;
        private final String stderr;
        private final double elapsedMs;

        public ProcessExecutionResult(boolean timedOut, int exitCode, EngineOutputDTO output,
                                      String stdout, String stderr, double elapsedMs) {
            this.timedOut = timedOut;
            this.exitCode = exitCode;
            this.output = output;
            this.stdout = stdout;
            this.stderr = stderr;
            this.elapsedMs = elapsedMs;
        }

        public boolean isTimedOut() { return timedOut; }
        public int getExitCode() { return exitCode; }
        public EngineOutputDTO getOutput() { return output; }
        public String getStdout() { return stdout; }
        public String getStderr() { return stderr; }
        public double getElapsedMs() { return elapsedMs; }
    }

    public ProcessExecutionResult executeEngine(EngineSearchConfigDTO config, long timeoutMs) {
        String binaryPath = engineProperties.resolveBinaryPath();
        File binaryFile = new File(binaryPath);
        if (!binaryFile.exists() || !binaryFile.isFile()) {
            throw new EngineExecutionException("GreedyBreaker engine binary not found at: " + binaryPath, -1, "");
        }

        long actualTimeout = (timeoutMs > 0) ? timeoutMs : engineProperties.getDefaultTimeoutMs();
        long startTime = System.nanoTime();

        ProcessBuilder pb = new ProcessBuilder(binaryPath);
        pb.redirectErrorStream(false);
        if (binaryFile.getParentFile() != null) {
            pb.directory(binaryFile.getParentFile());
        }

        Process process = null;
        try {
            process = pb.start();

            // 1. Drain stdout and stderr asynchronously to prevent pipe deadlocks
            CompletableFuture<String> stdoutFuture = drainStreamAsync(process.getInputStream());
            CompletableFuture<String> stderrFuture = drainStreamAsync(process.getErrorStream());

            // 2. Write configuration JSON to process stdin
            try (OutputStream os = process.getOutputStream()) {
                objectMapper.writeValue(os, config);
                os.flush();
            }

            // 3. Await completion or timeout
            boolean finished = process.waitFor(actualTimeout, TimeUnit.MILLISECONDS);
            double elapsedMs = (System.nanoTime() - startTime) / 1_000_000.0;

            if (!finished) {
                log.warn("C++ Engine exceeded timeout of {} ms; terminating process forcefully.", actualTimeout);
                process.destroyForcibly();
                String stderr = stderrFuture.join();
                return new ProcessExecutionResult(true, -1, null, "", stderr, elapsedMs);
            }

            int exitCode = process.exitValue();
            String stdout = stdoutFuture.join();
            String stderr = stderrFuture.join();

            if (exitCode != 0) {
                log.error("C++ Engine exited with non-zero code {}. Stderr: {}", exitCode, stderr);
                return new ProcessExecutionResult(false, exitCode, null, stdout, stderr, elapsedMs);
            }

            // 4. Parse structured JSON from stdout
            EngineOutputDTO outputDTO = objectMapper.readValue(stdout, EngineOutputDTO.class);
            return new ProcessExecutionResult(false, 0, outputDTO, stdout, stderr, elapsedMs);

        } catch (IOException e) {
            log.error("I/O exception while communicating with C++ engine: {}", e.getMessage(), e);
            throw new EngineExecutionException("Failed to invoke C++ engine subprocess: " + e.getMessage(), e);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            if (process != null) {
                process.destroyForcibly();
            }
            throw new EngineExecutionException("Engine execution was interrupted.", e);
        } finally {
            if (process != null && process.isAlive()) {
                process.destroyForcibly();
            }
        }
    }

    private CompletableFuture<String> drainStreamAsync(InputStream is) {
        return CompletableFuture.supplyAsync(() -> {
            try (BufferedReader reader = new BufferedReader(new InputStreamReader(is, StandardCharsets.UTF_8))) {
                StringBuilder sb = new StringBuilder();
                String line;
                while ((line = reader.readLine()) != null) {
                    sb.append(line).append("\n");
                }
                return sb.toString();
            } catch (IOException e) {
                return "";
            }
        });
    }
}
