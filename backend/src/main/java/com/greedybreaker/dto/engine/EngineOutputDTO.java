package com.greedybreaker.dto.engine;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.JsonNode;
import java.util.List;

@JsonIgnoreProperties(ignoreUnknown = true)
public class EngineOutputDTO {
    @JsonProperty("search_result")
    private EngineSearchResult searchResult;

    @JsonProperty("minimization_result")
    private EngineMinimizationResult minimizationResult;

    @JsonProperty("forensic_report")
    private EngineForensicReport forensicReport;

    public EngineSearchResult getSearchResult() { return searchResult; }
    public void setSearchResult(EngineSearchResult searchResult) { this.searchResult = searchResult; }

    public EngineMinimizationResult getMinimizationResult() { return minimizationResult; }
    public void setMinimizationResult(EngineMinimizationResult minimizationResult) { this.minimizationResult = minimizationResult; }

    public EngineForensicReport getForensicReport() { return forensicReport; }
    public void setForensicReport(EngineForensicReport forensicReport) { this.forensicReport = forensicReport; }

    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class EngineSearchResult {
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
        @JsonProperty("has_counterexample")
        private boolean hasCounterexample;
        @JsonProperty("instances_evaluated")
        private long instancesEvaluated;
        @JsonProperty("counterexamples_found")
        private long counterexamplesFound;
        @JsonProperty("failure_rate")
        private double failureRate;
        @JsonProperty("worst_gap")
        private double worstGap;
        @JsonProperty("average_gap")
        private double averageGap;
        @JsonProperty("coverage")
        private String coverage;
        @JsonProperty("coverage_ratio")
        private double coverageRatio;
        @JsonProperty("elapsed_time_ms")
        private double elapsedTimeMs;
        @JsonProperty("estimated_space_size")
        private long estimatedSpaceSize;
        @JsonProperty("status_code")
        private int statusCode;
        @JsonProperty("status_message")
        private String statusMessage;
        @JsonProperty("best_instance")
        private JsonNode bestInstance;
        @JsonProperty("best_greedy")
        private EngineGreedyResult bestGreedy;
        @JsonProperty("best_oracle")
        private EngineOracleResult bestOracle;
        @JsonProperty("best_verification")
        private EngineVerificationResult bestVerification;
        @JsonProperty("reproducibility_token")
        private JsonNode reproducibilityToken;

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
        public String getCoverage() { return coverage; }
        public void setCoverage(String coverage) { this.coverage = coverage; }
        public double getCoverageRatio() { return coverageRatio; }
        public void setCoverageRatio(double coverageRatio) { this.coverageRatio = coverageRatio; }
        public double getElapsedTimeMs() { return elapsedTimeMs; }
        public void setElapsedTimeMs(double elapsedTimeMs) { this.elapsedTimeMs = elapsedTimeMs; }
        public long getEstimatedSpaceSize() { return estimatedSpaceSize; }
        public void setEstimatedSpaceSize(long estimatedSpaceSize) { this.estimatedSpaceSize = estimatedSpaceSize; }
        public int getStatusCode() { return statusCode; }
        public void setStatusCode(int statusCode) { this.statusCode = statusCode; }
        public String getStatusMessage() { return statusMessage; }
        public void setStatusMessage(String statusMessage) { this.statusMessage = statusMessage; }
        public JsonNode getBestInstance() { return bestInstance; }
        public void setBestInstance(JsonNode bestInstance) { this.bestInstance = bestInstance; }
        public EngineGreedyResult getBestGreedy() { return bestGreedy; }
        public void setBestGreedy(EngineGreedyResult bestGreedy) { this.bestGreedy = bestGreedy; }
        public EngineOracleResult getBestOracle() { return bestOracle; }
        public void setBestOracle(EngineOracleResult bestOracle) { this.bestOracle = bestOracle; }
        public EngineVerificationResult getBestVerification() { return bestVerification; }
        public void setBestVerification(EngineVerificationResult bestVerification) { this.bestVerification = bestVerification; }
        public JsonNode getReproducibilityToken() { return reproducibilityToken; }
        public void setReproducibilityToken(JsonNode reproducibilityToken) { this.reproducibilityToken = reproducibilityToken; }
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class EngineGreedyResult {
        @JsonProperty("feasible")
        private boolean feasible;
        @JsonProperty("objective_value")
        private double objectiveValue;
        @JsonProperty("solution_description")
        private String solutionDescription;
        @JsonProperty("items_selected")
        private List<Object> itemsSelected;
        @JsonProperty("trace")
        private EngineTrace trace;
        @JsonProperty("metrics")
        private EngineMetrics metrics;

        public boolean isFeasible() { return feasible; }
        public void setFeasible(boolean feasible) { this.feasible = feasible; }
        public double getObjectiveValue() { return objectiveValue; }
        public void setObjectiveValue(double objectiveValue) { this.objectiveValue = objectiveValue; }
        public String getSolutionDescription() { return solutionDescription; }
        public void setSolutionDescription(String solutionDescription) { this.solutionDescription = solutionDescription; }
        public List<Object> getItemsSelected() { return itemsSelected; }
        public void setItemsSelected(List<Object> itemsSelected) { this.itemsSelected = itemsSelected; }
        public EngineTrace getTrace() { return trace; }
        public void setTrace(EngineTrace trace) { this.trace = trace; }
        public EngineMetrics getMetrics() { return metrics; }
        public void setMetrics(EngineMetrics metrics) { this.metrics = metrics; }
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class EngineOracleResult {
        @JsonProperty("confidence")
        private String confidence;
        @JsonProperty("optimal_value")
        private double optimalValue;
        @JsonProperty("oracle_method")
        private String oracleMethod;
        @JsonProperty("solution_description")
        private String solutionDescription;
        @JsonProperty("items_selected")
        private List<Object> itemsSelected;
        @JsonProperty("metrics")
        private EngineMetrics metrics;

        public String getConfidence() { return confidence; }
        public void setConfidence(String confidence) { this.confidence = confidence; }
        public double getOptimalValue() { return optimalValue; }
        public void setOptimalValue(double optimalValue) { this.optimalValue = optimalValue; }
        public String getOracleMethod() { return oracleMethod; }
        public void setOracleMethod(String oracleMethod) { this.oracleMethod = oracleMethod; }
        public String getSolutionDescription() { return solutionDescription; }
        public void setSolutionDescription(String solutionDescription) { this.solutionDescription = solutionDescription; }
        public List<Object> getItemsSelected() { return itemsSelected; }
        public void setItemsSelected(List<Object> itemsSelected) { this.itemsSelected = itemsSelected; }
        public EngineMetrics getMetrics() { return metrics; }
        public void setMetrics(EngineMetrics metrics) { this.metrics = metrics; }
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class EngineVerificationResult {
        @JsonProperty("status")
        private String status;
        @JsonProperty("is_counterexample")
        private boolean isCounterexample;
        @JsonProperty("greedy_value")
        private double greedyValue;
        @JsonProperty("optimal_value")
        private double optimalValue;
        @JsonProperty("absolute_gap")
        private double absoluteGap;
        @JsonProperty("relative_gap")
        private double relativeGap;
        @JsonProperty("failure_percent")
        private double failurePercent;
        @JsonProperty("summary")
        private String summary;

        public String getStatus() { return status; }
        public void setStatus(String status) { this.status = status; }
        public boolean isCounterexample() { return isCounterexample; }
        public void setCounterexample(boolean counterexample) { isCounterexample = counterexample; }
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
        public String getSummary() { return summary; }
        public void setSummary(String summary) { this.summary = summary; }
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class EngineMetrics {
        @JsonProperty("execution_time_ns")
        private long executionTimeNs;
        @JsonProperty("execution_time_ms")
        private double executionTimeMs;
        @JsonProperty("states_evaluated")
        private long statesEvaluated;

        public long getExecutionTimeNs() { return executionTimeNs; }
        public void setExecutionTimeNs(long executionTimeNs) { this.executionTimeNs = executionTimeNs; }
        public double getExecutionTimeMs() { return executionTimeMs; }
        public void setExecutionTimeMs(double executionTimeMs) { this.executionTimeMs = executionTimeMs; }
        public long getStatesEvaluated() { return statesEvaluated; }
        public void setStatesEvaluated(long statesEvaluated) { this.statesEvaluated = statesEvaluated; }
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class EngineMinimizationResult {
        @JsonProperty("original_instance")
        private JsonNode originalInstance;
        @JsonProperty("minimal_instance")
        private JsonNode minimalInstance;
        @JsonProperty("reduction_steps")
        private int reductionSteps;
        @JsonProperty("original_size_metric")
        private double originalSizeMetric;
        @JsonProperty("minimal_size_metric")
        private double minimalSizeMetric;
        @JsonProperty("reduction_percent")
        private double reductionPercent;
        @JsonProperty("ordering_criteria")
        private String orderingCriteria;
        @JsonProperty("minimality_status")
        private String minimalityStatus;
        @JsonProperty("final_verification")
        private EngineVerificationResult finalVerification;

        public JsonNode getOriginalInstance() { return originalInstance; }
        public void setOriginalInstance(JsonNode originalInstance) { this.originalInstance = originalInstance; }
        public JsonNode getMinimalInstance() { return minimalInstance; }
        public void setMinimalInstance(JsonNode minimalInstance) { this.minimalInstance = minimalInstance; }
        public int getReductionSteps() { return reductionSteps; }
        public void setReductionSteps(int reductionSteps) { this.reductionSteps = reductionSteps; }
        public double getOriginalSizeMetric() { return originalSizeMetric; }
        public void setOriginalSizeMetric(double originalSizeMetric) { this.originalSizeMetric = originalSizeMetric; }
        public double getMinimalSizeMetric() { return minimalSizeMetric; }
        public void setMinimalSizeMetric(double minimalSizeMetric) { this.minimalSizeMetric = minimalSizeMetric; }
        public double getReductionPercent() { return reductionPercent; }
        public void setReductionPercent(double reductionPercent) { this.reductionPercent = reductionPercent; }
        public String getOrderingCriteria() { return orderingCriteria; }
        public void setOrderingCriteria(String orderingCriteria) { this.orderingCriteria = orderingCriteria; }
        public String getMinimalityStatus() { return minimalityStatus; }
        public void setMinimalityStatus(String minimalityStatus) { this.minimalityStatus = minimalityStatus; }
        public EngineVerificationResult getFinalVerification() { return finalVerification; }
        public void setFinalVerification(EngineVerificationResult finalVerification) { this.finalVerification = finalVerification; }
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class EngineForensicReport {
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
        private EngineTrace greedyTrace;

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
        public EngineTrace getGreedyTrace() { return greedyTrace; }
        public void setGreedyTrace(EngineTrace greedyTrace) { this.greedyTrace = greedyTrace; }
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class EngineTrace {
        @JsonProperty("total_steps")
        private int totalSteps;
        @JsonProperty("steps")
        private List<EngineStep> steps;

        public int getTotalSteps() { return totalSteps; }
        public void setTotalSteps(int totalSteps) { this.totalSteps = totalSteps; }
        public List<EngineStep> getSteps() { return steps; }
        public void setSteps(List<EngineStep> steps) { this.steps = steps; }
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class EngineStep {
        @JsonProperty("step_index")
        private long stepIndex;
        @JsonProperty("action_description")
        private String actionDescription;
        @JsonProperty("heuristic_rule")
        private String heuristicRule;
        @JsonProperty("state_before")
        private String stateBefore;
        @JsonProperty("state_after")
        private String stateAfter;
        @JsonProperty("cumulative_objective")
        private double cumulativeObjective;

        public long getStepIndex() { return stepIndex; }
        public void setStepIndex(long stepIndex) { this.stepIndex = stepIndex; }
        public String getActionDescription() { return actionDescription; }
        public void setActionDescription(String actionDescription) { this.actionDescription = actionDescription; }
        public String getHeuristicRule() { return heuristicRule; }
        public void setHeuristicRule(String heuristicRule) { this.heuristicRule = heuristicRule; }
        public String getStateBefore() { return stateBefore; }
        public void setStateBefore(String stateBefore) { this.stateBefore = stateBefore; }
        public String getStateAfter() { return stateAfter; }
        public void setStateAfter(String stateAfter) { this.stateAfter = stateAfter; }
        public double getCumulativeObjective() { return cumulativeObjective; }
        public void setCumulativeObjective(double cumulativeObjective) { this.cumulativeObjective = cumulativeObjective; }
    }
}
