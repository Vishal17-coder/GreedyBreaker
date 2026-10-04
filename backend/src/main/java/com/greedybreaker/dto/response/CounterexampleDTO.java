package com.greedybreaker.dto.response;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.JsonNode;
import com.greedybreaker.dto.engine.EngineOutputDTO;

public class CounterexampleDTO {
    @JsonProperty("experiment_id")
    private String experimentId;
    @JsonProperty("has_counterexample")
    private boolean hasCounterexample;
    @JsonProperty("original_instance")
    private JsonNode originalInstance;
    @JsonProperty("minimal_instance")
    private JsonNode minimalInstance;
    @JsonProperty("reduction_steps")
    private int reductionSteps;
    @JsonProperty("reduction_percent")
    private double reductionPercent;
    @JsonProperty("minimality_status")
    private String minimalityStatus;
    @JsonProperty("ordering_criteria")
    private String orderingCriteria;
    @JsonProperty("final_verification")
    private EngineOutputDTO.EngineVerificationResult finalVerification;

    public CounterexampleDTO() {}

    public String getExperimentId() { return experimentId; }
    public void setExperimentId(String experimentId) { this.experimentId = experimentId; }
    public boolean isHasCounterexample() { return hasCounterexample; }
    public void setHasCounterexample(boolean hasCounterexample) { this.hasCounterexample = hasCounterexample; }
    public JsonNode getOriginalInstance() { return originalInstance; }
    public void setOriginalInstance(JsonNode originalInstance) { this.originalInstance = originalInstance; }
    public JsonNode getMinimalInstance() { return minimalInstance; }
    public void setMinimalInstance(JsonNode minimalInstance) { this.minimalInstance = minimalInstance; }
    public int getReductionSteps() { return reductionSteps; }
    public void setReductionSteps(int reductionSteps) { this.reductionSteps = reductionSteps; }
    public double getReductionPercent() { return reductionPercent; }
    public void setReductionPercent(double reductionPercent) { this.reductionPercent = reductionPercent; }
    public String getMinimalityStatus() { return minimalityStatus; }
    public void setMinimalityStatus(String minimalityStatus) { this.minimalityStatus = minimalityStatus; }
    public String getOrderingCriteria() { return orderingCriteria; }
    public void setOrderingCriteria(String orderingCriteria) { this.orderingCriteria = orderingCriteria; }
    public EngineOutputDTO.EngineVerificationResult getFinalVerification() { return finalVerification; }
    public void setFinalVerification(EngineOutputDTO.EngineVerificationResult finalVerification) { this.finalVerification = finalVerification; }
}
