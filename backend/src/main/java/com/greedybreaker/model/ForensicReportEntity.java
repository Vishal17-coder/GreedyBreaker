package com.greedybreaker.model;

import jakarta.persistence.*;

@Entity
@Table(name = "forensic_reports")
public class ForensicReportEntity {

    @Id
    @Column(name = "id", length = 64, nullable = false)
    private String id;

    @OneToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "execution_id", nullable = false)
    private ExperimentExecution execution;

    @Column(name = "divergence_step_index", nullable = false)
    private long divergenceStepIndex = -1;

    @Column(name = "critical_greedy_action", columnDefinition = "TEXT", nullable = false)
    private String criticalGreedyAction;

    @Column(name = "optimal_alternative_action", columnDefinition = "TEXT", nullable = false)
    private String optimalAlternativeAction;

    @Column(name = "causal_consequence", columnDefinition = "TEXT", nullable = false)
    private String causalConsequence;

    @Column(name = "evidence_confidence", length = 64, nullable = false)
    private String evidenceConfidence;

    @Column(name = "trace_steps_json", columnDefinition = "TEXT", nullable = false)
    private String traceStepsJson;

    public ForensicReportEntity() {}

    // Getters and Setters
    public String getId() { return id; }
    public void setId(String id) { this.id = id; }

    public ExperimentExecution getExecution() { return execution; }
    public void setExecution(ExperimentExecution execution) { this.execution = execution; }

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

    public String getTraceStepsJson() { return traceStepsJson; }
    public void setTraceStepsJson(String traceStepsJson) { this.traceStepsJson = traceStepsJson; }
}
