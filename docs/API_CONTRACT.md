# Greedy Breaker V2 — REST API Contract Specification

**API Version**: `v1`  
**Base Path**: `/api/v1`  
**Media Type**: `application/json`  
**Status**: APPROVED DESIGN  

---

## 1. Overview & General Conventions

All endpoints follow strict RESTful conventions:
- Resource collections use plural nouns (`/experiments`, `/problems`).
- Sub-resources and operational triggers use standard HTTP verbs (`POST /run`, `POST /validate`, `POST /reproduce`).
- Response envelopes for mutations include resource IDs, operational timestamps, and status codes.
- Error payloads follow RFC 7807 (`application/problem+json` format with `title`, `status`, `detail`, `timestamp`, and `errors`).
- All timestamps are ISO 8601 UTC strings (`YYYY-MM-DDTHH:mm:ss.sssZ`).

---

## 2. Problem Registry Endpoints

### 2.1 List Supported Problem Families
`GET /api/v1/problems`

#### Response (`200 OK`)
```json
[
  {
    "id": "coin_change",
    "name": "Coin Change (Fewest Coins)",
    "description": "Find the minimum number of coins to make change for a target value.",
    "objective": "MINIMIZE",
    "strategies": [
      { "id": "largest_denomination_first", "name": "Largest Denomination First", "default": true }
    ],
    "oracles": [
      { "id": "dp_exact", "name": "Dynamic Programming Unbounded Knapsack O(K*T)", "confidence": "EXACT", "default": true },
      { "id": "bfs_exact", "name": "Breadth-First Search Exact Shortest Path", "confidence": "EXACT", "default": false }
    ]
  },
  {
    "id": "knapsack_01",
    "name": "0/1 Knapsack Problem",
    "description": "Maximize total value of items placed in knapsack without exceeding capacity.",
    "objective": "MAXIMIZE",
    "strategies": [
      { "id": "value_density_descending", "name": "Value Density Descending (v_i / w_i)", "default": true },
      { "id": "max_value_first", "name": "Max Value First", "default": false },
      { "id": "min_weight_first", "name": "Min Weight First", "default": false }
    ],
    "oracles": [
      { "id": "dp_exact", "name": "Dynamic Programming 2D Table O(N*W)", "confidence": "EXACT", "default": true },
      { "id": "exhaustive_exact", "name": "Exhaustive Subset Search O(2^N)", "confidence": "EXACT", "default": false }
    ]
  },
  {
    "id": "activity_selection",
    "name": "Activity Selection Problem",
    "description": "Maximize the count of non-overlapping activities scheduled in a single room.",
    "objective": "MAXIMIZE",
    "strategies": [
      { "id": "earliest_finish_time", "name": "Earliest Finish Time (Optimal Heuristic)", "default": true },
      { "id": "shortest_interval", "name": "Shortest Interval Duration (Breakable Heuristic)", "default": false }
    ],
    "oracles": [
      { "id": "dp_exact", "name": "Interval Scheduling DP O(N^2)", "confidence": "EXACT", "default": true }
    ]
  },
  {
    "id": "fractional_knapsack",
    "name": "Fractional Knapsack Problem",
    "description": "Maximize value where arbitrary fractions of items may be selected.",
    "objective": "MAXIMIZE",
    "strategies": [
      { "id": "value_density_descending", "name": "Value Density Descending (Optimal Heuristic)", "default": true },
      { "id": "min_weight_first", "name": "Min Weight First (Breakable Heuristic)", "default": false }
    ],
    "oracles": [
      { "id": "continuous_lp_exact", "name": "Continuous Linear Programming Relaxation Optimum", "confidence": "EXACT", "default": true }
    ]
  },
  {
    "id": "huffman_coding",
    "name": "Optimal Prefix Code (Huffman Coding)",
    "description": "Minimize expected code length for symbol transmission.",
    "objective": "MINIMIZE",
    "strategies": [
      { "id": "standard_huffman", "name": "Standard Two-Lowest Frequency Merge (Optimal Heuristic)", "default": true },
      { "id": "shannon_fano", "name": "Shannon-Fano Top-Down Cumulative Split (Breakable Heuristic)", "default": false }
    ],
    "oracles": [
      { "id": "priority_queue_exact", "name": "Optimal Prefix Tree Ground Truth", "confidence": "EXACT", "default": true }
    ]
  }
]
```

---

## 3. Experiment Endpoints

### 3.1 Create Experiment
`POST /api/v1/experiments`

#### Request Body
```json
{
  "name": "Coin Change Greedy Break Test",
  "description": "Automated discovery of counterexamples on 3-coin denomination systems",
  "problem_type": "coin_change",
  "strategy_id": "largest_denomination_first",
  "oracle_id": "dp_exact",
  "mode": "ANY",
  "strategy": "EXHAUSTIVE",
  "random_seed": 42,
  "max_candidates": 1000,
  "max_time_ms": 5000,
  "search_space": {
    "denomination_count": { "min": 3, "max": 3 },
    "denomination_range": { "min": 1, "max": 10 },
    "target_range": { "min": 1, "max": 20 }
  }
}
```

#### Response (`201 Created`, Location: `/api/v1/experiments/exp-a1b2c3d4`)
```json
{
  "id": "exp-a1b2c3d4",
  "name": "Coin Change Greedy Break Test",
  "status": "CREATED",
  "problem_type": "coin_change",
  "strategy_id": "largest_denomination_first",
  "oracle_id": "dp_exact",
  "mode": "ANY",
  "strategy": "EXHAUSTIVE",
  "random_seed": 42,
  "max_candidates": 1000,
  "max_time_ms": 5000,
  "created_at": "2026-10-04T12:00:00.000Z",
  "updated_at": "2026-10-04T12:00:00.000Z"
}
```

---

### 3.2 Validate Experiment Configuration
`POST /api/v1/experiments/{id}/validate`

#### Response (`200 OK` - Valid)
```json
{
  "experiment_id": "exp-a1b2c3d4",
  "is_valid": true,
  "status": "VALID",
  "estimated_space_size": 360,
  "warnings": [],
  "validation_errors": []
}
```

#### Response (`422 Unprocessable Entity` - Invalid)
```json
{
  "experiment_id": "exp-a1b2c3d4",
  "is_valid": false,
  "status": "INVALID",
  "estimated_space_size": 0,
  "warnings": [],
  "validation_errors": [
    {
      "field": "search_space.target_range",
      "rejected_value": "{min: 20, max: 10}",
      "message": "Minimum value (20) cannot exceed maximum value (10)."
    }
  ]
}
```

---

### 3.3 Execute Experiment Run
`POST /api/v1/experiments/{id}/run`

#### Request Query Parameters
- `async`: `boolean` (default `false`). If `false`, executes synchronously and returns final result; if `true`, queues experiment and returns HTTP 202 Accepted.

#### Response (`200 OK` - Synchronous Run Completed)
```json
{
  "experiment_id": "exp-a1b2c3d4",
  "status": "COMPLETED",
  "has_counterexample": true,
  "instances_evaluated": 31,
  "counterexamples_found": 1,
  "failure_rate": 0.032258,
  "elapsed_time_ms": 0.8506,
  "coverage": "EXHAUSTIVE",
  "coverage_ratio": 0.1476,
  "execution_metrics": {
    "execution_time_ns": 21200,
    "execution_time_ms": 0.0212,
    "states_evaluated": 3
  },
  "created_at": "2026-10-04T12:00:00.000Z",
  "completed_at": "2026-10-04T12:00:01.000Z"
}
```

---

### 3.4 Get Experiment Details
`GET /api/v1/experiments/{id}`

#### Response (`200 OK`)
```json
{
  "id": "exp-a1b2c3d4",
  "name": "Coin Change Greedy Break Test",
  "description": "Automated discovery of counterexamples on 3-coin denomination systems",
  "problem_type": "coin_change",
  "status": "COMPLETED",
  "configuration": {
    "strategy_id": "largest_denomination_first",
    "oracle_id": "dp_exact",
    "mode": "ANY",
    "strategy": "EXHAUSTIVE",
    "random_seed": 42,
    "max_candidates": 1000,
    "max_time_ms": 5000,
    "search_space": {
      "denomination_count": { "min": 4, "max": 4 },
      "denomination_range": { "min": 1, "max": 10 },
      "target_range": { "min": 6, "max": 6 }
    }
  },
  "summary": {
    "has_counterexample": true,
    "counterexamples_count": 1,
    "instances_evaluated": 31,
    "worst_gap": 1.0
  },
  "created_at": "2026-10-04T12:00:00.000Z",
  "updated_at": "2026-10-04T12:00:01.000Z"
}
```

---

### 3.5 Get Experiment Status
`GET /api/v1/experiments/{id}/status`

#### Response (`200 OK`)
```json
{
  "experiment_id": "exp-a1b2c3d4",
  "status": "COMPLETED",
  "instances_evaluated": 31,
  "counterexamples_found": 1,
  "elapsed_time_ms": 0.8506,
  "message": "Experiment completed successfully with verified counterexample"
}
```

---

### 3.6 Get Experiment Full Results
`GET /api/v1/experiments/{id}/result`

#### Response (`200 OK`)
```json
{
  "experiment_id": "exp-a1b2c3d4",
  "status": "COMPLETED",
  "search_result": {
    "instances_evaluated": 31,
    "counterexamples_found": 1,
    "failure_rate": 0.032258,
    "worst_gap": 1.0,
    "average_gap": 1.0,
    "coverage": "EXHAUSTIVE",
    "coverage_ratio": 0.1476,
    "elapsed_time_ms": 0.8506,
    "best_greedy": {
      "feasible": true,
      "objective_value": 3.0,
      "items_selected": [4, 1, 1],
      "solution_description": "Used 3 coins: [4, 1, 1]"
    },
    "best_oracle": {
      "confidence": "EXACT",
      "optimal_value": 2.0,
      "items_selected": [3, 3],
      "oracle_method": "Dynamic Programming Unbounded Knapsack O(K*T)",
      "solution_description": "Exact optimum: 2 coins: [3, 3]"
    },
    "best_verification": {
      "status": "OPTIMALITY_FAILURE",
      "is_counterexample": true,
      "greedy_value": 3.0,
      "optimal_value": 2.0,
      "absolute_gap": 1.0,
      "relative_gap": 1.5,
      "failure_percent": 50.0
    }
  },
  "reproducibility_token": {
    "engine_version": "2.0.0",
    "experiment_id": "exp-a1b2c3d4",
    "mode": "ANY",
    "oracle_id": "dp_exact",
    "problem_type": "coin_change",
    "random_seed": 42,
    "strategy": "EXHAUSTIVE",
    "strategy_id": "largest_denomination_first"
  }
}
```

---

### 3.7 Get Counterexample Details
`GET /api/v1/experiments/{id}/counterexamples`

#### Response (`200 OK`)
```json
{
  "experiment_id": "exp-a1b2c3d4",
  "has_counterexample": true,
  "original_instance": {
    "denominations": [1, 3, 4, 7],
    "target": 6
  },
  "minimal_instance": {
    "denominations": [1, 3, 4],
    "target": 6
  },
  "reduction_steps": 1,
  "reduction_percent": 24.96,
  "minimality_status": "Smallest verified counterexample under the configured ordering",
  "ordering_criteria": "1. Minimal element count; 2. Minimal max parameter; 3. Minimal parameter sum; 4. Lexicographical",
  "final_verification": {
    "status": "OPTIMALITY_FAILURE",
    "is_counterexample": true,
    "greedy_value": 3.0,
    "optimal_value": 2.0,
    "absolute_gap": 1.0,
    "relative_gap": 1.5,
    "failure_percent": 50.0
  }
}
```

---

### 3.8 Get Forensic Divergence Report
`GET /api/v1/experiments/{id}/forensics`

#### Response (`200 OK`)
```json
{
  "experiment_id": "exp-a1b2c3d4",
  "problem_type": "coin_change",
  "divergence_step_index": 0,
  "critical_greedy_action": "Selected coin 4",
  "optimal_alternative_action": "Optimal selection uses coin 3",
  "causal_consequence": "Greedily selecting coin 4 left a residual amount requiring 2 additional coins, exceeding optimal total 2.0.",
  "evidence_confidence": "HIGH_CONFIDENCE_EVIDENCE",
  "minimality_status": "Smallest verified counterexample under the configured ordering",
  "greedy_trace": {
    "total_steps": 3,
    "steps": [
      {
        "step_index": 0,
        "action_description": "Selected coin 4",
        "heuristic_rule": "Pick largest denomination <= remaining target",
        "state_before": "Remaining target: 6",
        "state_after": "Remaining target: 2",
        "cumulative_objective": 1.0
      },
      {
        "step_index": 1,
        "action_description": "Selected coin 1",
        "heuristic_rule": "Pick largest denomination <= remaining target",
        "state_before": "Remaining target: 2",
        "state_after": "Remaining target: 1",
        "cumulative_objective": 2.0
      },
      {
        "step_index": 2,
        "action_description": "Selected coin 1",
        "heuristic_rule": "Pick largest denomination <= remaining target",
        "state_before": "Remaining target: 1",
        "state_after": "Remaining target: 0",
        "cumulative_objective": 3.0
      }
    ]
  }
}
```

---

### 3.9 Reproduce Experiment
`POST /api/v1/experiments/{id}/reproduce`

#### Description
Creates and executes a new reproduction experiment based on the original experiment's reproducibility token. Does NOT mutate or overwrite the original experiment record.

#### Response (`201 Created`, Location: `/api/v1/experiments/exp-repro-998877`)
```json
{
  "reproduction_experiment_id": "exp-repro-998877",
  "original_experiment_id": "exp-a1b2c3d4",
  "status": "COMPLETED",
  "is_deterministic_match": true,
  "match_summary": "All metrics, discovered instances, objectives, and divergence steps matched the original experiment identically.",
  "created_at": "2026-10-04T12:05:00.000Z"
}
```

---

### 3.10 List Experiment History
`GET /api/v1/experiments?status=COMPLETED&problem_type=coin_change&page=0&size=20`

#### Response (`200 OK`)
```json
{
  "content": [
    {
      "id": "exp-a1b2c3d4",
      "name": "Coin Change Greedy Break Test",
      "problem_type": "coin_change",
      "status": "COMPLETED",
      "has_counterexample": true,
      "instances_evaluated": 31,
      "created_at": "2026-10-04T12:00:00.000Z"
    }
  ],
  "page": 0,
  "size": 20,
  "total_elements": 1,
  "total_pages": 1
}
```
