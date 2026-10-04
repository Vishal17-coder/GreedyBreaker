package com.greedybreaker.dto.response;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.JsonNode;
import com.greedybreaker.model.enums.ExperimentStatus;
import com.greedybreaker.model.enums.ProblemType;
import com.greedybreaker.model.enums.SearchMode;
import com.greedybreaker.model.enums.SearchStrategy;
import java.time.Instant;

public class ExperimentDetailDTO {
    private String id;
    private String name;
    private String description;
    @JsonProperty("problem_type")
    private ProblemType problemType;
    private ExperimentStatus status;
    @JsonProperty("parent_experiment_id")
    private String parentExperimentId;
    private ConfigurationDTO configuration;
    private SummaryDTO summary;
    @JsonProperty("created_at")
    private Instant createdAt;
    @JsonProperty("updated_at")
    private Instant updatedAt;

    public ExperimentDetailDTO() {}

    public static class ConfigurationDTO {
        @JsonProperty("strategy_id")
        private String strategyId;
        @JsonProperty("oracle_id")
        private String oracleId;
        private SearchMode mode;
        private SearchStrategy strategy;
        @JsonProperty("random_seed")
        private Long randomSeed;
        @JsonProperty("max_candidates")
        private Long maxCandidates;
        @JsonProperty("max_time_ms")
        private Long maxTimeMs;
        @JsonProperty("search_space")
        private JsonNode searchSpace;

        public String getStrategyId() { return strategyId; }
        public void setStrategyId(String strategyId) { this.strategyId = strategyId; }
        public String getOracleId() { return oracleId; }
        public void setOracleId(String oracleId) { this.oracleId = oracleId; }
        public SearchMode getMode() { return mode; }
        public void setMode(SearchMode mode) { this.mode = mode; }
        public SearchStrategy getStrategy() { return strategy; }
        public void setStrategy(SearchStrategy strategy) { this.strategy = strategy; }
        public Long getRandomSeed() { return randomSeed; }
        public void setRandomSeed(Long randomSeed) { this.randomSeed = randomSeed; }
        public Long getMaxCandidates() { return maxCandidates; }
        public void setMaxCandidates(Long maxCandidates) { this.maxCandidates = maxCandidates; }
        public Long getMaxTimeMs() { return maxTimeMs; }
        public void setMaxTimeMs(Long maxTimeMs) { this.maxTimeMs = maxTimeMs; }
        public JsonNode getSearchSpace() { return searchSpace; }
        public void setSearchSpace(JsonNode searchSpace) { this.searchSpace = searchSpace; }
    }

    public static class SummaryDTO {
        @JsonProperty("has_counterexample")
        private boolean hasCounterexample;
        @JsonProperty("counterexamples_count")
        private long counterexamplesCount;
        @JsonProperty("instances_evaluated")
        private long instancesEvaluated;
        @JsonProperty("worst_gap")
        private double worstGap;

        public boolean isHasCounterexample() { return hasCounterexample; }
        public void setHasCounterexample(boolean hasCounterexample) { this.hasCounterexample = hasCounterexample; }
        public long getCounterexamplesCount() { return counterexamplesCount; }
        public void setCounterexamplesCount(long counterexamplesCount) { this.counterexamplesCount = counterexamplesCount; }
        public long getInstancesEvaluated() { return instancesEvaluated; }
        public void setInstancesEvaluated(long instancesEvaluated) { this.instancesEvaluated = instancesEvaluated; }
        public double getWorstGap() { return worstGap; }
        public void setWorstGap(double worstGap) { this.worstGap = worstGap; }
    }

    public String getId() { return id; }
    public void setId(String id) { this.id = id; }
    public String getName() { return name; }
    public void setName(String name) { this.name = name; }
    public String getDescription() { return description; }
    public void setDescription(String description) { this.description = description; }
    public ProblemType getProblemType() { return problemType; }
    public void setProblemType(ProblemType problemType) { this.problemType = problemType; }
    public ExperimentStatus getStatus() { return status; }
    public void setStatus(ExperimentStatus status) { this.status = status; }
    public String getParentExperimentId() { return parentExperimentId; }
    public void setParentExperimentId(String parentExperimentId) { this.parentExperimentId = parentExperimentId; }
    public ConfigurationDTO getConfiguration() { return configuration; }
    public void setConfiguration(ConfigurationDTO configuration) { this.configuration = configuration; }
    public SummaryDTO getSummary() { return summary; }
    public void setSummary(SummaryDTO summary) { this.summary = summary; }
    public Instant getCreatedAt() { return createdAt; }
    public void setCreatedAt(Instant createdAt) { this.createdAt = createdAt; }
    public Instant getUpdatedAt() { return updatedAt; }
    public void setUpdatedAt(Instant updatedAt) { this.updatedAt = updatedAt; }
}
