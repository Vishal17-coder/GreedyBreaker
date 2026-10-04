package com.greedybreaker.dto.response;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.greedybreaker.model.enums.ExperimentStatus;

public class ExecutionStatusDTO {
    @JsonProperty("experiment_id")
    private String experimentId;
    private ExperimentStatus status;
    @JsonProperty("instances_evaluated")
    private long instancesEvaluated;
    @JsonProperty("counterexamples_found")
    private long counterexamplesFound;
    @JsonProperty("elapsed_time_ms")
    private double elapsedTimeMs;
    private String message;

    public ExecutionStatusDTO() {}

    public String getExperimentId() { return experimentId; }
    public void setExperimentId(String experimentId) { this.experimentId = experimentId; }
    public ExperimentStatus getStatus() { return status; }
    public void setStatus(ExperimentStatus status) { this.status = status; }
    public long getInstancesEvaluated() { return instancesEvaluated; }
    public void setInstancesEvaluated(long instancesEvaluated) { this.instancesEvaluated = instancesEvaluated; }
    public long getCounterexamplesFound() { return counterexamplesFound; }
    public void setCounterexamplesFound(long counterexamplesFound) { this.counterexamplesFound = counterexamplesFound; }
    public double getElapsedTimeMs() { return elapsedTimeMs; }
    public void setElapsedTimeMs(double elapsedTimeMs) { this.elapsedTimeMs = elapsedTimeMs; }
    public String getMessage() { return message; }
    public void setMessage(String message) { this.message = message; }
}
