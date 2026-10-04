package com.greedybreaker.config;

import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.context.annotation.Configuration;

import java.io.File;

@Configuration
@ConfigurationProperties(prefix = "greedybreaker.engine")
public class EngineProperties {

    /**
     * Path to the C++ greedybreaker_cli binary.
     */
    private String binaryPath = "../engine/build/bin/greedybreaker_cli.exe";

    /**
     * Default execution timeout in milliseconds.
     */
    private long defaultTimeoutMs = 5000L;

    /**
     * Hard ceiling on candidates evaluated per experiment.
     */
    private long maxCandidatesLimit = 100000L;

    /**
     * Hard ceiling on execution duration per experiment in ms.
     */
    private long maxDurationLimitMs = 30000L;

    public EngineProperties() {}

    public String getBinaryPath() { return binaryPath; }
    public void setBinaryPath(String binaryPath) { this.binaryPath = binaryPath; }

    public long getDefaultTimeoutMs() { return defaultTimeoutMs; }
    public void setDefaultTimeoutMs(long defaultTimeoutMs) { this.defaultTimeoutMs = defaultTimeoutMs; }

    public long getMaxCandidatesLimit() { return maxCandidatesLimit; }
    public void setMaxCandidatesLimit(long maxCandidatesLimit) { this.maxCandidatesLimit = maxCandidatesLimit; }

    public long getMaxDurationLimitMs() { return maxDurationLimitMs; }
    public void setMaxDurationLimitMs(long maxDurationLimitMs) { this.maxDurationLimitMs = maxDurationLimitMs; }

    /**
     * Resolves the executable path, checking both relative paths and fallback locations.
     */
    public String resolveBinaryPath() {
        File file = new File(binaryPath);
        if (file.exists() && file.isFile()) {
            return file.getAbsolutePath();
        }
        // Fallback 1: check relative from workspace root
        File rootFile = new File("engine/build/bin/greedybreaker_cli.exe");
        if (rootFile.exists() && rootFile.isFile()) {
            return rootFile.getAbsolutePath();
        }
        // Fallback 2: check relative from backend dir
        File backendRel = new File("../engine/build/bin/greedybreaker_cli.exe");
        if (backendRel.exists() && backendRel.isFile()) {
            return backendRel.getAbsolutePath();
        }
        return binaryPath;
    }
}
