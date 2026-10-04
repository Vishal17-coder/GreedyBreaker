package com.greedybreaker.exception;

public class EngineExecutionException extends RuntimeException {
    private final int exitCode;
    private final String stderr;

    public EngineExecutionException(String message, int exitCode, String stderr) {
        super(message);
        this.exitCode = exitCode;
        this.stderr = stderr;
    }

    public EngineExecutionException(String message, Throwable cause) {
        super(message, cause);
        this.exitCode = -1;
        this.stderr = "";
    }

    public int getExitCode() { return exitCode; }
    public String getStderr() { return stderr; }
}
