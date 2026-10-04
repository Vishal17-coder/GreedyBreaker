package com.greedybreaker.dto.engine;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.JsonNode;

@JsonIgnoreProperties(ignoreUnknown = true)
public class EngineSearchConfigDTO {
    @JsonProperty("experiment_id")
    private String experimentId;

    @JsonProperty("problem_type")
    private String problemType;

    @JsonProperty("strategy_id")
    private String strategyId;

    @JsonProperty("oracle_id")
    private String oracleId;

    @JsonProperty("mode")
    private String mode;

    @JsonProperty("strategy")
    private String strategy;

    @JsonProperty("random_seed")
    private Long randomSeed;

    @JsonProperty("max_candidates")
    private Long maxCandidates;

    @JsonProperty("max_time_ms")
    private Long maxTimeMs;

    @JsonProperty("search_space")
    private JsonNode searchSpace;

    public EngineSearchConfigDTO() {}

    // Getters and Setters
    public String getExperimentId() { return experimentId; }
    public void setExperimentId(String experimentId) { this.experimentId = experimentId; }

    public String getProblemType() { return problemType; }
    public void setProblemType(String problemType) { this.problemType = problemType; }

    public String getStrategyId() { return strategyId; }
    public void setStrategyId(String strategyId) { this.strategyId = strategyId; }

    public String getOracleId() { return oracleId; }
    public void setOracleId(String oracleId) { this.oracleId = oracleId; }

    public String getMode() { return mode; }
    public void setMode(String mode) { this.mode = mode; }

    public String getStrategy() { return strategy; }
    public void setStrategy(String strategy) { this.strategy = strategy; }

    public Long getRandomSeed() { return randomSeed; }
    public void setRandomSeed(Long randomSeed) { this.randomSeed = randomSeed; }

    public Long getMaxCandidates() { return maxCandidates; }
    public void setMaxCandidates(Long maxCandidates) { this.maxCandidates = maxCandidates; }

    public Long getMaxTimeMs() { return maxTimeMs; }
    public void setMaxTimeMs(Long maxTimeMs) { this.maxTimeMs = maxTimeMs; }

    public JsonNode getSearchSpace() { return searchSpace; }
    public void setSearchSpace(JsonNode searchSpace) { this.searchSpace = searchSpace; }
}
