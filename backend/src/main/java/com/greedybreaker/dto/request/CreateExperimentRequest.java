package com.greedybreaker.dto.request;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.JsonNode;
import com.greedybreaker.model.enums.ProblemType;
import com.greedybreaker.model.enums.SearchMode;
import com.greedybreaker.model.enums.SearchStrategy;
import jakarta.validation.constraints.Max;
import jakarta.validation.constraints.Min;
import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.NotNull;

public class CreateExperimentRequest {

    @NotBlank(message = "Experiment name must not be blank")
    private String name;

    private String description;

    @NotNull(message = "problem_type is required")
    @JsonProperty("problem_type")
    private ProblemType problemType;

    @JsonProperty("strategy_id")
    private String strategyId = "default";

    @JsonProperty("oracle_id")
    private String oracleId = "default";

    private SearchMode mode = SearchMode.ANY;

    private SearchStrategy strategy = SearchStrategy.EXHAUSTIVE;

    @JsonProperty("random_seed")
    private Long randomSeed = 42L;

    @Min(value = 1, message = "max_candidates must be at least 1")
    @Max(value = 100000, message = "max_candidates cannot exceed 100,000")
    @JsonProperty("max_candidates")
    private Long maxCandidates = 1000L;

    @Min(value = 100, message = "max_time_ms must be at least 100 ms")
    @Max(value = 30000, message = "max_time_ms cannot exceed 30,000 ms (30 seconds)")
    @JsonProperty("max_time_ms")
    private Long maxTimeMs = 5000L;

    @NotNull(message = "search_space is required")
    @JsonProperty("search_space")
    private JsonNode searchSpace;

    public CreateExperimentRequest() {}

    // Getters and Setters
    public String getName() { return name; }
    public void setName(String name) { this.name = name; }

    public String getDescription() { return description; }
    public void setDescription(String description) { this.description = description; }

    public ProblemType getProblemType() { return problemType; }
    public void setProblemType(ProblemType problemType) { this.problemType = problemType; }

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
