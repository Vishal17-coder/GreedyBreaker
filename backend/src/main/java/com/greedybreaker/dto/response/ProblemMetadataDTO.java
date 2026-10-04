package com.greedybreaker.dto.response;

import java.util.List;

public class ProblemMetadataDTO {
    private String id;
    private String name;
    private String description;
    private String objective;
    private List<StrategyInfo> strategies;
    private List<OracleInfo> oracles;

    public ProblemMetadataDTO() {}

    public ProblemMetadataDTO(String id, String name, String description, String objective,
                              List<StrategyInfo> strategies, List<OracleInfo> oracles) {
        this.id = id;
        this.name = name;
        this.description = description;
        this.objective = objective;
        this.strategies = strategies;
        this.oracles = oracles;
    }

    public static class StrategyInfo {
        private String id;
        private String name;
        private boolean isDefault;

        public StrategyInfo() {}
        public StrategyInfo(String id, String name, boolean isDefault) {
            this.id = id;
            this.name = name;
            this.isDefault = isDefault;
        }

        public String getId() { return id; }
        public void setId(String id) { this.id = id; }
        public String getName() { return name; }
        public void setName(String name) { this.name = name; }
        public boolean isDefault() { return isDefault; }
        public void setDefault(boolean aDefault) { isDefault = aDefault; }
    }

    public static class OracleInfo {
        private String id;
        private String name;
        private String confidence;
        private boolean isDefault;

        public OracleInfo() {}
        public OracleInfo(String id, String name, String confidence, boolean isDefault) {
            this.id = id;
            this.name = name;
            this.confidence = confidence;
            this.isDefault = isDefault;
        }

        public String getId() { return id; }
        public void setId(String id) { this.id = id; }
        public String getName() { return name; }
        public void setName(String name) { this.name = name; }
        public String getConfidence() { return confidence; }
        public void setConfidence(String confidence) { this.confidence = confidence; }
        public boolean isDefault() { return isDefault; }
        public void setDefault(boolean aDefault) { isDefault = aDefault; }
    }

    public String getId() { return id; }
    public void setId(String id) { this.id = id; }
    public String getName() { return name; }
    public void setName(String name) { this.name = name; }
    public String getDescription() { return description; }
    public void setDescription(String description) { this.description = description; }
    public String getObjective() { return objective; }
    public void setObjective(String objective) { this.objective = objective; }
    public List<StrategyInfo> getStrategies() { return strategies; }
    public void setStrategies(List<StrategyInfo> strategies) { this.strategies = strategies; }
    public List<OracleInfo> getOracles() { return oracles; }
    public void setOracles(List<OracleInfo> oracles) { this.oracles = oracles; }
}
