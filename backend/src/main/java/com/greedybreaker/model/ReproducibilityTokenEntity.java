package com.greedybreaker.model;

import jakarta.persistence.*;

@Entity
@Table(name = "reproducibility_tokens")
public class ReproducibilityTokenEntity {

    @Id
    @Column(name = "id", length = 64, nullable = false)
    private String id;

    @OneToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "execution_id", nullable = false)
    private ExperimentExecution execution;

    @Column(name = "token_json", columnDefinition = "TEXT", nullable = false)
    private String tokenJson;

    public ReproducibilityTokenEntity() {}

    public ReproducibilityTokenEntity(String id, ExperimentExecution execution, String tokenJson) {
        this.id = id;
        this.execution = execution;
        this.tokenJson = tokenJson;
    }

    public String getId() { return id; }
    public void setId(String id) { this.id = id; }

    public ExperimentExecution getExecution() { return execution; }
    public void setExecution(ExperimentExecution execution) { this.execution = execution; }

    public String getTokenJson() { return tokenJson; }
    public void setTokenJson(String tokenJson) { this.tokenJson = tokenJson; }
}
