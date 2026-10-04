package com.greedybreaker.dto.response;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.greedybreaker.model.enums.ExperimentStatus;
import com.greedybreaker.model.enums.ProblemType;
import java.time.Instant;

public class ExperimentSummaryDTO {
    private String id;
    private String name;
    @JsonProperty("problem_type")
    private ProblemType problemType;
    private ExperimentStatus status;
    @JsonProperty("has_counterexample")
    private boolean hasCounterexample;
    @JsonProperty("instances_evaluated")
    private long instancesEvaluated;
    @JsonProperty("created_at")
    private Instant createdAt;

    public ExperimentSummaryDTO() {}

    public String getId() { return id; }
    public void setId(String id) { this.id = id; }
    public String getName() { return name; }
    public void setName(String name) { this.name = name; }
    public ProblemType getProblemType() { return problemType; }
    public void setProblemType(ProblemType problemType) { this.problemType = problemType; }
    public ExperimentStatus getStatus() { return status; }
    public void setStatus(ExperimentStatus status) { this.status = status; }
    public boolean isHasCounterexample() { return hasCounterexample; }
    public void setHasCounterexample(boolean hasCounterexample) { this.hasCounterexample = hasCounterexample; }
    public long getInstancesEvaluated() { return instancesEvaluated; }
    public void setInstancesEvaluated(long instancesEvaluated) { this.instancesEvaluated = instancesEvaluated; }
    public Instant getCreatedAt() { return createdAt; }
    public void setCreatedAt(Instant createdAt) { this.createdAt = createdAt; }
}
