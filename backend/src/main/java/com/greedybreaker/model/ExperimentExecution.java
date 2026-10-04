package com.greedybreaker.model;

import com.greedybreaker.model.enums.ExperimentStatus;
import com.greedybreaker.model.enums.SearchCoverage;
import jakarta.persistence.*;
import java.time.Instant;

@Entity
@Table(name = "experiment_executions")
public class ExperimentExecution {

    @Id
    @Column(name = "id", length = 64, nullable = false)
    private String id;

    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "experiment_id", nullable = false)
    private Experiment experiment;

    @Enumerated(EnumType.STRING)
    @Column(name = "status", length = 32, nullable = false)
    private ExperimentStatus status;

    @Column(name = "has_counterexample", nullable = false)
    private boolean hasCounterexample = false;

    @Column(name = "instances_evaluated", nullable = false)
    private long instancesEvaluated = 0;

    @Column(name = "counterexamples_found", nullable = false)
    private long counterexamplesFound = 0;

    @Column(name = "failure_rate", nullable = false)
    private double failureRate = 0.0;

    @Column(name = "worst_gap", nullable = false)
    private double worstGap = 0.0;

    @Column(name = "average_gap", nullable = false)
    private double averageGap = 0.0;

    @Column(name = "elapsed_time_ms", nullable = false)
    private double elapsedTimeMs = 0.0;

    @Enumerated(EnumType.STRING)
    @Column(name = "coverage", length = 32, nullable = false)
    private SearchCoverage coverage = SearchCoverage.PARTIAL;

    @Column(name = "coverage_ratio", nullable = false)
    private double coverageRatio = 0.0;

    @Column(name = "states_evaluated", nullable = false)
    private long statesEvaluated = 0;

    @Column(name = "execution_time_ns", nullable = false)
    private long executionTimeNs = 0;

    @Column(name = "status_code", nullable = false)
    private int statusCode = 0;

    @Column(name = "status_message", columnDefinition = "TEXT")
    private String statusMessage;

    @Column(name = "started_at", nullable = false)
    private Instant startedAt;

    @Column(name = "completed_at", nullable = false)
    private Instant completedAt;

    @OneToOne(mappedBy = "execution", cascade = CascadeType.ALL, fetch = FetchType.EAGER)
    private Counterexample counterexample;

    @OneToOne(mappedBy = "execution", cascade = CascadeType.ALL, fetch = FetchType.EAGER)
    private ForensicReportEntity forensicReport;

    @OneToOne(mappedBy = "execution", cascade = CascadeType.ALL, fetch = FetchType.EAGER)
    private ReproducibilityTokenEntity reproducibilityToken;

    public ExperimentExecution() {
        this.startedAt = Instant.now();
        this.completedAt = Instant.now();
    }

    public ExperimentExecution(String id, Experiment experiment) {
        this();
        this.id = id;
        this.experiment = experiment;
    }

    // Getters and Setters
    public String getId() { return id; }
    public void setId(String id) { this.id = id; }

    public Experiment getExperiment() { return experiment; }
    public void setExperiment(Experiment experiment) { this.experiment = experiment; }

    public ExperimentStatus getStatus() { return status; }
    public void setStatus(ExperimentStatus status) { this.status = status; }

    public boolean isHasCounterexample() { return hasCounterexample; }
    public void setHasCounterexample(boolean hasCounterexample) { this.hasCounterexample = hasCounterexample; }

    public long getInstancesEvaluated() { return instancesEvaluated; }
    public void setInstancesEvaluated(long instancesEvaluated) { this.instancesEvaluated = instancesEvaluated; }

    public long getCounterexamplesFound() { return counterexamplesFound; }
    public void setCounterexamplesFound(long counterexamplesFound) { this.counterexamplesFound = counterexamplesFound; }

    public double getFailureRate() { return failureRate; }
    public void setFailureRate(double failureRate) { this.failureRate = failureRate; }

    public double getWorstGap() { return worstGap; }
    public void setWorstGap(double worstGap) { this.worstGap = worstGap; }

    public double getAverageGap() { return averageGap; }
    public void setAverageGap(double averageGap) { this.averageGap = averageGap; }

    public double getElapsedTimeMs() { return elapsedTimeMs; }
    public void setElapsedTimeMs(double elapsedTimeMs) { this.elapsedTimeMs = elapsedTimeMs; }

    public SearchCoverage getCoverage() { return coverage; }
    public void setCoverage(SearchCoverage coverage) { this.coverage = coverage; }

    public double getCoverageRatio() { return coverageRatio; }
    public void setCoverageRatio(double coverageRatio) { this.coverageRatio = coverageRatio; }

    public long getStatesEvaluated() { return statesEvaluated; }
    public void setStatesEvaluated(long statesEvaluated) { this.statesEvaluated = statesEvaluated; }

    public long getExecutionTimeNs() { return executionTimeNs; }
    public void setExecutionTimeNs(long executionTimeNs) { this.executionTimeNs = executionTimeNs; }

    public int getStatusCode() { return statusCode; }
    public void setStatusCode(int statusCode) { this.statusCode = statusCode; }

    public String getStatusMessage() { return statusMessage; }
    public void setStatusMessage(String statusMessage) { this.statusMessage = statusMessage; }

    public Instant getStartedAt() { return startedAt; }
    public void setStartedAt(Instant startedAt) { this.startedAt = startedAt; }

    public Instant getCompletedAt() { return completedAt; }
    public void setCompletedAt(Instant completedAt) { this.completedAt = completedAt; }

    public Counterexample getCounterexample() { return counterexample; }
    public void setCounterexample(Counterexample counterexample) {
        this.counterexample = counterexample;
        if (counterexample != null) {
            counterexample.setExecution(this);
        }
    }

    public ForensicReportEntity getForensicReport() { return forensicReport; }
    public void setForensicReport(ForensicReportEntity forensicReport) {
        this.forensicReport = forensicReport;
        if (forensicReport != null) {
            forensicReport.setExecution(this);
        }
    }

    public ReproducibilityTokenEntity getReproducibilityToken() { return reproducibilityToken; }
    public void setReproducibilityToken(ReproducibilityTokenEntity reproducibilityToken) {
        this.reproducibilityToken = reproducibilityToken;
        if (reproducibilityToken != null) {
            reproducibilityToken.setExecution(this);
        }
    }
}
