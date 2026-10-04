package com.greedybreaker.model;

import com.greedybreaker.model.enums.SearchMode;
import com.greedybreaker.model.enums.SearchStrategy;
import jakarta.persistence.*;

@Entity
@Table(name = "experiment_configurations")
public class ExperimentConfiguration {

    @Id
    @Column(name = "experiment_id", length = 64, nullable = false)
    private String experimentId;

    @OneToOne
    @MapsId
    @JoinColumn(name = "experiment_id")
    private Experiment experiment;

    @Column(name = "strategy_id", length = 64, nullable = false)
    private String strategyId;

    @Column(name = "oracle_id", length = 64, nullable = false)
    private String oracleId;

    @Enumerated(EnumType.STRING)
    @Column(name = "mode", length = 32, nullable = false)
    private SearchMode mode;

    @Enumerated(EnumType.STRING)
    @Column(name = "strategy", length = 32, nullable = false)
    private SearchStrategy strategy;

    @Column(name = "random_seed", nullable = false)
    private Long randomSeed;

    @Column(name = "max_candidates", nullable = false)
    private Long maxCandidates;

    @Column(name = "max_time_ms", nullable = false)
    private Long maxTimeMs;

    @Column(name = "search_space_json", columnDefinition = "TEXT", nullable = false)
    private String searchSpaceJson;

    public ExperimentConfiguration() {}

    // Getters and Setters
    public String getExperimentId() { return experimentId; }
    public void setExperimentId(String experimentId) { this.experimentId = experimentId; }

    public Experiment getExperiment() { return experiment; }
    public void setExperiment(Experiment experiment) {
        this.experiment = experiment;
        if (experiment != null) {
            this.experimentId = experiment.getId();
        }
    }

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

    public String getSearchSpaceJson() { return searchSpaceJson; }
    public void setSearchSpaceJson(String searchSpaceJson) { this.searchSpaceJson = searchSpaceJson; }
}
