package com.greedybreaker.dto.response;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.greedybreaker.dto.engine.EngineOutputDTO;

public class ForensicReportDTO {
    @JsonProperty("experiment_id")
    private String experimentId;
    @JsonProperty("problem_type")
    private String problemType;
    @JsonProperty("divergence_step_index")
    private long divergenceStepIndex;
    @JsonProperty("critical_greedy_action")
    private String criticalGreedyAction;
    @JsonProperty("optimal_alternative_action")
    private String optimalAlternativeAction;
    @JsonProperty("causal_consequence")
    private String causalConsequence;
    @JsonProperty("evidence_confidence")
    private String evidenceConfidence;
    @JsonProperty("minimality_status")
    private String minimalityStatus;
    @JsonProperty("greedy_trace")
    private EngineOutputDTO.EngineTrace greedyTrace;

    public ForensicReportDTO() {}

    public String getExperimentId() { return experimentId; }
    public void setExperimentId(String experimentId) { this.experimentId = experimentId; }
    public String getProblemType() { return problemType; }
    public void setProblemType(String problemType) { this.problemType = problemType; }
    public long getDivergenceStepIndex() { return divergenceStepIndex; }
    public void setDivergenceStepIndex(long divergenceStepIndex) { this.divergenceStepIndex = divergenceStepIndex; }
    public String getCriticalGreedyAction() { return criticalGreedyAction; }
    public void setCriticalGreedyAction(String criticalGreedyAction) { this.criticalGreedyAction = criticalGreedyAction; }
    public String getOptimalAlternativeAction() { return optimalAlternativeAction; }
    public void setOptimalAlternativeAction(String optimalAlternativeAction) { this.optimalAlternativeAction = optimalAlternativeAction; }
    public String getCausalConsequence() { return causalConsequence; }
    public void setCausalConsequence(String causalConsequence) { this.causalConsequence = causalConsequence; }
    public String getEvidenceConfidence() { return evidenceConfidence; }
    public void setEvidenceConfidence(String evidenceConfidence) { this.evidenceConfidence = evidenceConfidence; }
    public String getMinimalityStatus() { return minimalityStatus; }
    public void setMinimalityStatus(String minimalityStatus) { this.minimalityStatus = minimalityStatus; }
    public EngineOutputDTO.EngineTrace getGreedyTrace() { return greedyTrace; }
    public void setGreedyTrace(EngineOutputDTO.EngineTrace greedyTrace) { this.greedyTrace = greedyTrace; }
}
