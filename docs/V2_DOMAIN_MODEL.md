# Greedy Breaker V2 — C++20 Domain Model Specification

## 1. Domain Types & Enums

### 1.1 Objective Direction
```cpp
namespace greedybreaker::core {

enum class ObjectiveDirection : uint8_t {
    MINIMIZE = 0,  // e.g. Coin Change (fewer coins), Huffman (shorter length)
    MAXIMIZE = 1   // e.g. Knapsack (higher value), Activity Selection (more activities)
};

}
```

### 1.2 Verification Status & Failure Taxonomy
```cpp
namespace greedybreaker::core {

enum class VerificationStatus : uint8_t {
    OPTIMAL = 0,              // Greedy achieved optimal objective
    FEASIBILITY_FAILURE = 1,  // Greedy failed to construct a valid solution
    OPTIMALITY_FAILURE = 2    // Greedy was feasible, but strictly suboptimal
};

enum class OracleConfidence : uint8_t {
    EXACT = 0,          // Guaranteed mathematical optimum (e.g. Dynamic Programming)
    APPROXIMATION = 1,  // Provable approximation bound
    HEURISTIC = 2       // Non-exact comparison baseline
};

}
```

### 1.3 Search Configurations
```cpp
namespace greedybreaker::search {

enum class SearchMode : uint8_t {
    ANY = 0,       // Terminate on first counterexample
    SMALLEST = 1,  // Search for the smallest counterexample under canonical order
    WORST = 2,     // Search for maximal objective gap
    PROFILE = 3    // Survey entire space and produce statistical failure distribution
};

enum class SearchStrategy : uint8_t {
    EXHAUSTIVE = 0,  // Deterministic Cartesian exploration
    RANDOMIZED = 1,  // Deterministic PRNG with explicit seed
    GUIDED = 2,      // Structural trap-driven candidate generation
    HYBRID = 3       // Multi-phase exploration
};

enum class SearchCoverage : uint8_t {
    EXHAUSTIVE = 0,
    PARTIAL = 1,
    RANDOMIZED_SAMPLE = 2
};

}
```

---

## 2. Core Entities

### 2.1 Decision Trace & Forensic Steps
```cpp
namespace greedybreaker::forensics {

struct DecisionStep {
    size_t step_index;
    std::string candidate_state;
    std::string chosen_action;
    std::string heuristic_rule;
    double instantaneous_cost;
    std::string state_summary;
};

struct DecisionTrace {
    std::vector<DecisionStep> steps;
    uint64_t total_decision_time_ns;
};

}
```

### 2.2 Execution Results
```cpp
namespace greedybreaker::core {

template <typename SolutionType>
struct GreedyResult {
    SolutionType solution;
    double objective_value;
    bool is_feasible;
    forensics::DecisionTrace trace;
    uint64_t execution_time_ns;
};

template <typename SolutionType>
struct OracleResult {
    SolutionType optimal_solution;
    double optimal_value;
    OracleConfidence confidence;
    std::string oracle_method;
    uint64_t states_evaluated;
    uint64_t execution_time_ns;
};

struct VerificationResult {
    VerificationStatus status;
    bool is_counterexample;
    double absolute_gap;
    double relative_gap;
    double deficit_or_overhead_percent;
};

}
```

---

## 3. Abstract Interfaces

### 3.1 Problem Definition Interface
```cpp
namespace greedybreaker::core {

template <typename InstanceType, typename SolutionType>
class IProblem {
public:
    virtual ~IProblem() = default;

    [[nodiscard]] virtual std::string problem_id() const = 0;
    [[nodiscard]] virtual std::string display_name() const = 0;
    [[nodiscard]] virtual ObjectiveDirection objective() const = 0;

    [[nodiscard]] virtual bool is_valid_instance(const InstanceType& instance) const = 0;
    [[nodiscard]] virtual std::string instance_to_string(const InstanceType& instance) const = 0;
};

}
```

### 3.2 Greedy Solver Interface
```cpp
namespace greedybreaker::core {

template <typename InstanceType, typename SolutionType>
class IGreedySolver {
public:
    virtual ~IGreedySolver() = default;

    [[nodiscard]] virtual std::string strategy_id() const = 0;
    [[nodiscard]] virtual std::string description() const = 0;

    [[nodiscard]] virtual GreedyResult<SolutionType> solve(
        const InstanceType& instance
    ) const = 0;
};

}
```

### 3.3 Exact Oracle Interface
```cpp
namespace greedybreaker::oracle {

template <typename InstanceType, typename SolutionType>
class IExactOracle {
public:
    virtual ~IExactOracle() = default;

    [[nodiscard]] virtual std::string oracle_id() const = 0;
    [[nodiscard]] virtual core::OracleConfidence confidence() const = 0;

    [[nodiscard]] virtual core::OracleResult<SolutionType> solve_exact(
        const InstanceType& instance
    ) const = 0;
};

}
```

### 3.4 Minimizer Interface
```cpp
namespace greedybreaker::minimization {

template <typename InstanceType>
class IMinimizer {
public:
    virtual ~IMinimizer() = default;

    [[nodiscard]] virtual InstanceType minimize(
        const InstanceType& failing_instance,
        const std::function<bool(const InstanceType&)>& failure_predicate
    ) const = 0;
};

}
```

### 3.5 Forensic Analyzer Interface
```cpp
namespace greedybreaker::forensics {

template <typename InstanceType, typename SolutionType>
class IForensicAnalyzer {
public:
    virtual ~IForensicAnalyzer() = default;

    [[nodiscard]] virtual ForensicReport analyze(
        const InstanceType& instance,
        const core::GreedyResult<SolutionType>& greedy,
        const core::OracleResult<SolutionType>& oracle,
        const core::VerificationResult& verification
    ) const = 0;
};

}
```

---

## 4. Reproducibility Data Contract

```json
{
  "reproducibility_token": {
    "experiment_id": "exp-20261004-coinchange-001",
    "problem": "coin_change",
    "greedy_strategy": "largest_denomination_first",
    "oracle_method": "dynamic_programming_exact",
    "search_strategy": "exhaustive",
    "search_mode": "smallest",
    "search_space_bounds": {
      "denomination_count": { "min": 3, "max": 4 },
      "denomination_range": { "min": 1, "max": 30 },
      "target_range": { "min": 1, "max": 50 }
    },
    "random_seed": 42,
    "algorithm_version": "2.0.0"
  },
  "search_summary": {
    "instances_evaluated": 1420,
    "counterexamples_discovered": 12,
    "search_coverage": "EXHAUSTIVE",
    "coverage_ratio": 1.0,
    "elapsed_time_ms": 34.2
  },
  "smallest_counterexample": {
    "instance": {
      "denominations": [1, 3, 4],
      "target": 6
    },
    "greedy_outcome": {
      "coins_used": [4, 1, 1],
      "count": 3,
      "feasible": true
    },
    "optimal_outcome": {
      "coins_used": [3, 3],
      "count": 2,
      "confidence": "EXACT"
    },
    "verification": {
      "status": "OPTIMALITY_FAILURE",
      "absolute_gap": 1.0,
      "relative_gap": 1.5,
      "overhead_percentage": 50.0
    },
    "forensics": {
      "divergence_step": 1,
      "greedy_choice": "Selected coin 4 (leaving remaining target 2)",
      "optimal_alternatives": ["Selected coin 3 (leaving remaining target 3)"],
      "causal_mechanism": "Greedily picking largest denomination 4 leaves residual target 2 which cannot be formed by coins >= 2, forcing two 1-coins."
    }
  }
}
```
