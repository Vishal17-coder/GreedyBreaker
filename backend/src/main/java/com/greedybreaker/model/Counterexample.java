package com.greedybreaker.model;

import jakarta.persistence.*;

@Entity
@Table(name = "counterexamples")
public class Counterexample {

    @Id
    @Column(name = "id", length = 64, nullable = false)
    private String id;

    @OneToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "execution_id", nullable = false)
    private ExperimentExecution execution;

    @Column(name = "original_instance_json", columnDefinition = "TEXT", nullable = false)
    private String originalInstanceJson;

    @Column(name = "minimal_instance_json", columnDefinition = "TEXT", nullable = false)
    private String minimalInstanceJson;

    @Column(name = "greedy_value", nullable = false)
    private double greedyValue;

    @Column(name = "optimal_value", nullable = false)
    private double optimalValue;

    @Column(name = "absolute_gap", nullable = false)
    private double absoluteGap;

    @Column(name = "relative_gap", nullable = false)
    private double relativeGap;

    @Column(name = "failure_percent", nullable = false)
    private double failurePercent;

    @Column(name = "reduction_steps", nullable = false)
    private int reductionSteps = 0;

    @Column(name = "reduction_percent", nullable = false)
    private double reductionPercent = 0.0;

    @Column(name = "minimality_status", nullable = false)
    private String minimalityStatus;

    @Column(name = "ordering_criteria", columnDefinition = "TEXT", nullable = false)
    private String orderingCriteria;

    @Column(name = "verification_status", length = 64)
    private String verificationStatus;

    public Counterexample() {}

    // Getters and Setters
    public String getId() { return id; }
    public void setId(String id) { this.id = id; }

    public ExperimentExecution getExecution() { return execution; }
    public void setExecution(ExperimentExecution execution) { this.execution = execution; }

    public String getOriginalInstanceJson() { return originalInstanceJson; }
    public void setOriginalInstanceJson(String originalInstanceJson) { this.originalInstanceJson = originalInstanceJson; }

    public String getMinimalInstanceJson() { return minimalInstanceJson; }
    public void setMinimalInstanceJson(String minimalInstanceJson) { this.minimalInstanceJson = minimalInstanceJson; }

    public double getGreedyValue() { return greedyValue; }
    public void setGreedyValue(double greedyValue) { this.greedyValue = greedyValue; }

    public double getOptimalValue() { return optimalValue; }
    public void setOptimalValue(double optimalValue) { this.optimalValue = optimalValue; }

    public double getAbsoluteGap() { return absoluteGap; }
    public void setAbsoluteGap(double absoluteGap) { this.absoluteGap = absoluteGap; }

    public double getRelativeGap() { return relativeGap; }
    public void setRelativeGap(double relativeGap) { this.relativeGap = relativeGap; }

    public double getFailurePercent() { return failurePercent; }
    public void setFailurePercent(double failurePercent) { this.failurePercent = failurePercent; }

    public int getReductionSteps() { return reductionSteps; }
    public void setReductionSteps(int reductionSteps) { this.reductionSteps = reductionSteps; }

    public double getReductionPercent() { return reductionPercent; }
    public void setReductionPercent(double reductionPercent) { this.reductionPercent = reductionPercent; }

    public String getMinimalityStatus() { return minimalityStatus; }
    public void setMinimalityStatus(String minimalityStatus) { this.minimalityStatus = minimalityStatus; }

    public String getOrderingCriteria() { return orderingCriteria; }
    public void setOrderingCriteria(String orderingCriteria) { this.orderingCriteria = orderingCriteria; }

    public String getVerificationStatus() { return verificationStatus; }
    public void setVerificationStatus(String verificationStatus) { this.verificationStatus = verificationStatus; }
}
