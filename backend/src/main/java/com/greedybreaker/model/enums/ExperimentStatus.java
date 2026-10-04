package com.greedybreaker.model.enums;

public enum ExperimentStatus {
    CREATED,
    VALIDATING,
    VALID,
    INVALID,
    QUEUED,
    RUNNING,
    COMPLETED,
    NO_COUNTEREXAMPLE,
    SEARCH_INCOMPLETE,
    TIMED_OUT,
    FAILED,
    CANCELLED;

    public boolean isTerminal() {
        return this == INVALID || this == COMPLETED || this == NO_COUNTEREXAMPLE
                || this == SEARCH_INCOMPLETE || this == TIMED_OUT || this == FAILED || this == CANCELLED;
    }
}
