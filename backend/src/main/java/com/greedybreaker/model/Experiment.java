package com.greedybreaker.model;

import com.greedybreaker.model.enums.ExperimentStatus;
import com.greedybreaker.model.enums.ProblemType;
import jakarta.persistence.*;
import java.time.Instant;
import java.util.ArrayList;
import java.util.List;

@Entity
@Table(name = "experiments")
public class Experiment {

    @Id
    @Column(name = "id", length = 64, nullable = false)
    private String id;

    @Column(name = "name", nullable = false)
    private String name;

    @Column(name = "description", columnDefinition = "TEXT")
    private String description;

    @Enumerated(EnumType.STRING)
    @Column(name = "problem_type", length = 64, nullable = false)
    private ProblemType problemType;

    @Enumerated(EnumType.STRING)
    @Column(name = "status", length = 32, nullable = false)
    private ExperimentStatus status;

    @Column(name = "parent_experiment_id", length = 64)
    private String parentExperimentId;

    @Column(name = "created_at", nullable = false, updatable = false)
    private Instant createdAt;

    @Column(name = "updated_at", nullable = false)
    private Instant updatedAt;

    @OneToOne(mappedBy = "experiment", cascade = CascadeType.ALL, fetch = FetchType.EAGER)
    private ExperimentConfiguration configuration;

    @OneToMany(mappedBy = "experiment", cascade = CascadeType.ALL, fetch = FetchType.LAZY)
    private List<ExperimentExecution> executions = new ArrayList<>();

    public Experiment() {
        this.createdAt = Instant.now();
        this.updatedAt = Instant.now();
        this.status = ExperimentStatus.CREATED;
    }

    public Experiment(String id, String name, ProblemType problemType) {
        this();
        this.id = id;
        this.name = name;
        this.problemType = problemType;
    }

    @PreUpdate
    public void onUpdate() {
        this.updatedAt = Instant.now();
    }

    // Getters and Setters
    public String getId() { return id; }
    public void setId(String id) { this.id = id; }

    public String getName() { return name; }
    public void setName(String name) { this.name = name; }

    public String getDescription() { return description; }
    public void setDescription(String description) { this.description = description; }

    public ProblemType getProblemType() { return problemType; }
    public void setProblemType(ProblemType problemType) { this.problemType = problemType; }

    public ExperimentStatus getStatus() { return status; }
    public void setStatus(ExperimentStatus status) { this.status = status; }

    public String getParentExperimentId() { return parentExperimentId; }
    public void setParentExperimentId(String parentExperimentId) { this.parentExperimentId = parentExperimentId; }

    public Instant getCreatedAt() { return createdAt; }
    public void setCreatedAt(Instant createdAt) { this.createdAt = createdAt; }

    public Instant getUpdatedAt() { return updatedAt; }
    public void setUpdatedAt(Instant updatedAt) { this.updatedAt = updatedAt; }

    public ExperimentConfiguration getConfiguration() { return configuration; }
    public void setConfiguration(ExperimentConfiguration configuration) {
        this.configuration = configuration;
        if (configuration != null) {
            configuration.setExperiment(this);
        }
    }

    public List<ExperimentExecution> getExecutions() { return executions; }
    public void setExecutions(List<ExperimentExecution> executions) { this.executions = executions; }

    public void addExecution(ExperimentExecution execution) {
        this.executions.add(execution);
        execution.setExperiment(this);
    }
}
