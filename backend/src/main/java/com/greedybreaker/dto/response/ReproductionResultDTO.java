package com.greedybreaker.dto.response;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.greedybreaker.model.enums.ExperimentStatus;
import java.time.Instant;

public class ReproductionResultDTO {
    @JsonProperty("reproduction_experiment_id")
    private String reproductionExperimentId;
    @JsonProperty("original_experiment_id")
    private String originalExperimentId;
    private ExperimentStatus status;
    @JsonProperty("is_deterministic_match")
    private boolean isDeterministicMatch;
    @JsonProperty("match_summary")
    private String matchSummary;
    @JsonProperty("created_at")
    private Instant createdAt;

    public ReproductionResultDTO() {}

    public String getReproductionExperimentId() { return reproductionExperimentId; }
    public void setReproductionExperimentId(String reproductionExperimentId) { this.reproductionExperimentId = reproductionExperimentId; }
    public String getOriginalExperimentId() { return originalExperimentId; }
    public void setOriginalExperimentId(String originalExperimentId) { this.originalExperimentId = originalExperimentId; }
    public ExperimentStatus getStatus() { return status; }
    public void setStatus(ExperimentStatus status) { this.status = status; }
    public boolean isDeterministicMatch() { return isDeterministicMatch; }
    public void setDeterministicMatch(boolean deterministicMatch) { isDeterministicMatch = deterministicMatch; }
    public String getMatchSummary() { return matchSummary; }
    public void setMatchSummary(String matchSummary) { this.matchSummary = matchSummary; }
    public Instant getCreatedAt() { return createdAt; }
    public void setCreatedAt(Instant createdAt) { this.createdAt = createdAt; }
}
