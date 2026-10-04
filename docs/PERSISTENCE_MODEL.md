# Greedy Breaker V2 — Persistence Model Specification

**Status**: APPROVED DESIGN  
**Version**: 2.0.0  
**Target RDBMS**: PostgreSQL 15+ (Production) / In-Memory H2 in PostgreSQL Mode (Testing)  

---

## 1. Entity-Relationship Architecture

```text
+-----------------------+           1:1           +-------------------------------+
|      Experiment       | ─────────────────────── |    ExperimentConfiguration    |
| - id (PK, UUID/String)|                         | - experiment_id (FK, PK)      |
| - name                |                         | - strategy_id                 |
| - description         |                         | - oracle_id                   |
| - problem_type        |                         | - mode                        |
| - status (Enum)       |                         | - strategy                    |
| - parent_id (FK, self)|                         | - random_seed                 |
| - created_at          |                         | - max_candidates              |
| - updated_at          |                         | - max_time_ms                 |
+-----------┬-----------+                         | - search_space_json (JSONB)   |
            │                                     +-------------------------------+
            │ 1:N
            ▼
+-----------------------+           1:1           +-------------------------------+
|  ExperimentExecution  | ─────────────────────── |      ReproducibilityToken     |
| - id (PK, UUID)       |                         | - id (PK, UUID)               |
| - experiment_id (FK)  |                         | - execution_id (FK)           |
| - status (Enum)       |                         | - token_json (JSONB)          |
| - instances_eval      |                         +-------------------------------+
| - counterexamples_cnt |
| - failure_rate        |
| - elapsed_time_ms     |           1:1           +-------------------------------+
| - worst_gap           | ─────────────────────── |        ForensicReport         |
| - average_gap         |                         | - id (PK, UUID)               |
| - coverage (Enum)     |                         | - execution_id (FK)           |
| - coverage_ratio      |                         | - divergence_step_index       |
+-----------┬-----------+                         | - critical_greedy_action      |
            │                                     | - optimal_alt_action          |
            │ 0..1 (if CE found)                  | - causal_consequence          |
            ▼                                     | - evidence_confidence         |
+-----------------------+                         | - trace_steps_json (JSONB)    |
|    Counterexample     |                         +-------------------------------+
| - id (PK, UUID)       |
| - execution_id (FK)   |
| - original_json(JSONB)|
| - minimal_json (JSONB)|
| - greedy_value        |
| - optimal_value       |
| - absolute_gap        |
| - relative_gap        |
| - failure_percent     |
| - reduction_steps     |
| - reduction_percent   |
| - minimality_status   |
| - ordering_criteria   |
+-----------------------+
```

---

## 2. Table Schemas & DDL (PostgreSQL)

```sql
-- 1. Experiments
CREATE TABLE experiments (
    id VARCHAR(64) PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    description TEXT,
    problem_type VARCHAR(64) NOT NULL,
    status VARCHAR(32) NOT NULL,
    parent_experiment_id VARCHAR(64) REFERENCES experiments(id) ON DELETE SET NULL,
    created_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW()
);
CREATE INDEX idx_experiments_problem_type ON experiments(problem_type);
CREATE INDEX idx_experiments_status ON experiments(status);
CREATE INDEX idx_experiments_created_at ON experiments(created_at DESC);

-- 2. Experiment Configurations
CREATE TABLE experiment_configurations (
    experiment_id VARCHAR(64) PRIMARY KEY REFERENCES experiments(id) ON DELETE CASCADE,
    strategy_id VARCHAR(64) NOT NULL,
    oracle_id VARCHAR(64) NOT NULL,
    mode VARCHAR(32) NOT NULL,
    strategy VARCHAR(32) NOT NULL,
    random_seed BIGINT NOT NULL,
    max_candidates BIGINT NOT NULL,
    max_time_ms BIGINT NOT NULL,
    search_space_json JSONB NOT NULL
);

-- 3. Experiment Executions
CREATE TABLE experiment_executions (
    id VARCHAR(64) PRIMARY KEY,
    experiment_id VARCHAR(64) NOT NULL REFERENCES experiments(id) ON DELETE CASCADE,
    status VARCHAR(32) NOT NULL,
    has_counterexample BOOLEAN NOT NULL DEFAULT FALSE,
    instances_evaluated BIGINT NOT NULL DEFAULT 0,
    counterexamples_found BIGINT NOT NULL DEFAULT 0,
    failure_rate DOUBLE PRECISION NOT NULL DEFAULT 0.0,
    worst_gap DOUBLE PRECISION NOT NULL DEFAULT 0.0,
    average_gap DOUBLE PRECISION NOT NULL DEFAULT 0.0,
    elapsed_time_ms DOUBLE PRECISION NOT NULL DEFAULT 0.0,
    coverage VARCHAR(32) NOT NULL DEFAULT 'PARTIAL',
    coverage_ratio DOUBLE PRECISION NOT NULL DEFAULT 0.0,
    states_evaluated BIGINT NOT NULL DEFAULT 0,
    execution_time_ns BIGINT NOT NULL DEFAULT 0,
    status_code INT NOT NULL DEFAULT 0,
    status_message TEXT,
    started_at TIMESTAMP WITH TIME ZONE NOT NULL,
    completed_at TIMESTAMP WITH TIME ZONE NOT NULL
);
CREATE INDEX idx_executions_experiment ON experiment_executions(experiment_id);

-- 4. Discovered Counterexamples
CREATE TABLE counterexamples (
    id VARCHAR(64) PRIMARY KEY,
    execution_id VARCHAR(64) NOT NULL REFERENCES experiment_executions(id) ON DELETE CASCADE,
    original_instance_json JSONB NOT NULL,
    minimal_instance_json JSONB NOT NULL,
    greedy_value DOUBLE PRECISION NOT NULL,
    optimal_value DOUBLE PRECISION NOT NULL,
    absolute_gap DOUBLE PRECISION NOT NULL,
    relative_gap DOUBLE PRECISION NOT NULL,
    failure_percent DOUBLE PRECISION NOT NULL,
    reduction_steps INT NOT NULL DEFAULT 0,
    reduction_percent DOUBLE PRECISION NOT NULL DEFAULT 0.0,
    minimality_status VARCHAR(255) NOT NULL,
    ordering_criteria TEXT NOT NULL
);
CREATE INDEX idx_counterexamples_execution ON counterexamples(execution_id);

-- 5. Forensic Reports
CREATE TABLE forensic_reports (
    id VARCHAR(64) PRIMARY KEY,
    execution_id VARCHAR(64) NOT NULL REFERENCES experiment_executions(id) ON DELETE CASCADE,
    divergence_step_index BIGINT NOT NULL,
    critical_greedy_action TEXT NOT NULL,
    optimal_alternative_action TEXT NOT NULL,
    causal_consequence TEXT NOT NULL,
    evidence_confidence VARCHAR(64) NOT NULL,
    trace_steps_json JSONB NOT NULL
);
CREATE INDEX idx_forensics_execution ON forensic_reports(execution_id);

-- 6. Reproducibility Tokens
CREATE TABLE reproducibility_tokens (
    id VARCHAR(64) PRIMARY KEY,
    execution_id VARCHAR(64) NOT NULL REFERENCES experiment_executions(id) ON DELETE CASCADE,
    token_json JSONB NOT NULL
);
```

---

## 3. Storage Efficiency Rationale

During a search evaluating $10,000$ candidates, the engine does NOT stream or persist non-counterexample candidate instances.
- **Persisted Data**: Only the experiment parameters, summary statistics, minimal counterexamples, and forensic divergence steps are committed to storage.
- **Storage Profile**: Average storage per completed experiment is $< 10\text{ KB}$, allowing millions of experiment records without database degradation.
