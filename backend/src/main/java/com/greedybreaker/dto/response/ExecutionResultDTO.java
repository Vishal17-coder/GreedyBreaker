package com.greedybreaker.dto.response;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.JsonNode;
import com.greedybreaker.dto.engine.EngineOutputDTO;
import com.greedybreaker.model.enums.ExperimentStatus;

public class ExecutionResultDTO {
    @JsonProperty("experiment_id")
    private String experimentId;
    private ExperimentStatus status;
    @JsonProperty("search_result")
    private EngineOutputDTO.EngineSearchResult searchResult;
    @JsonProperty("reproducibility_token")
    private JsonNode reproducibilityToken;

    public ExecutionResultDTO() {}

    public String getExperimentId() { return experimentId; }
    public void setExperimentId(String experimentId) { this.experimentId = experimentId; }
    public ExperimentStatus getStatus() { return status; }
    public void setStatus(ExperimentStatus status) { this.status = status; }
    public EngineOutputDTO.EngineSearchResult getSearchResult() { return searchResult; }
    public void setSearchResult(EngineOutputDTO.EngineSearchResult searchResult) { this.searchResult = searchResult; }
    public JsonNode getReproducibilityToken() { return reproducibilityToken; }
    public void setReproducibilityToken(JsonNode reproducibilityToken) { this.reproducibilityToken = reproducibilityToken; }
}
