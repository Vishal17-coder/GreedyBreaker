# GreedyBreaker Architecture

## Automated Counterexample Discovery for Greedy Algorithms

GreedyBreaker is an algorithm investigation and forensic analysis platform. Rather than simply visualizing textbook greedy algorithms, it algorithmically breaks greedy strategies by searching for inputs where greedy heuristics produce suboptimal outcomes.

---

## 1. System Architecture

```text
                 GREEDY BREAKER
                       │
                 Common Framework
                       │
        ┌──────────────┼──────────────┐
        ↓              ↓              ↓
   Coin Change   Activity Selection   Knapsack
                                     /       \
                                    /         \
                              Fractional      0/1
```

```text
                    GREEDY BREAKER ARCHITECTURE
                               │
                               ▼
                    C++20 AUTHORITATIVE CORE
           ┌───────────────────┼───────────────────┐
           ▼                   ▼                   ▼
    Problem Registry     Common Search       Forensic Engine
    (COIN_CHANGE,        (Exhaustive,        (Divergence,
     ACTIVITY,            Backtracking,       Metrics,
     KNAPSACK_01,         Randomized,         Step-by-step
     FRACTIONAL)          Heuristic)          Traces)
           │                   │                   │
           └───────────────────┼───────────────────┘
                               ▼
                    Problem Implementations
        ┌──────────────────────┼──────────────────────┐
        ▼                      ▼                      ▼
   Coin Change         Activity Selection         Knapsack
  (Minimization)         (Maximization)        (Maximization)
   • Greedy: Largest     • Greedy: Earliest     • 0/1 Knapsack:
     Coin First            Finish Time First      - Ratio Greedy (Breakable)
   • Oracle: DP          • Oracle: DP Interval    - Oracle: 2D DP O(nW)
     O(K * T)              Scheduling O(n log n)• Fractional Knapsack:
                         • Empirical: 0 Breaks    - Continuous Ratio (Optimal)
                                                  - Oracle: Continuous Relax.
```

> **Critical Architecture Principle**:
> The C++20 engine is the authoritative computational layer for all DAA algorithms, exact dynamic programming oracles, search engines, and forensic analysis. Problem logic is completely decoupled from the search engine via generic streaming generators and problem interfaces.

---

## 2. Milestone Roadmap

```text
Phase 1: C++20 Core Problem Engine [COMPLETE]
  ├── Problem Abstraction (GreedyProblem<Candidate, Item>)
  ├── Coin Change Heuristic (Largest Denomination First)
  ├── Exact DP Oracle (Unbounded Knapsack Recurrence)
  ├── Execution Traces (TraceStep, AlgorithmTrace)
  ├── First Divergence Detection
  ├── Failure Severity Evaluation (Gap, Ratio, Overhead)
  └── GoogleTest Validation (27 unit tests)
          ↓
Phase 2: Counterexample Search Engine [COMPLETE]
  ├── Search Abstraction (SearchEngine)
  ├── Search Modes (ANY, SMALLEST, WORST)
  ├── Strategies:
  │     ├── Exhaustive (Deterministic Cartesian enumeration)
  │     ├── Backtracking (Incremental with branch-and-bound pruning)
  │     ├── Randomized (Deterministic PRNG with seed reproducibility)
  │     └── Heuristic (Kozen-Zaks remainder traps)
  ├── Canonical Ordering: (Target ascending, Count ascending, Lexicographical)
  ├── Safety Limits & Cooperative Cancellation
  └── GoogleTest Validation (53 total unit tests)
          ↓
Phase 3: Multi-Problem Framework & DAA Expansion [COMPLETE]
  ├── Problem Registry (`ProblemType` enum & slug mapping)
  ├── Objective Direction (`ProblemObjective::Minimize` vs `Maximize`)
  ├── Activity Selection Problem:
  │     ├── Greedy: Earliest Finish Time First
  │     ├── Exact Oracle: DP Interval Scheduling OPT(j) = max(OPT(j-1), 1 + OPT(p(j)))
  │     └── Empirical Verification: 0 false counterexamples discovered
  ├── 0/1 Knapsack Problem:
  │     ├── Greedy: Value/Weight Ratio Descending (Heuristic, Breakable)
  │     ├── Exact Oracle: 2D Dynamic Programming O(nW)
  │     ├── Forensic Analysis: First-divergence & stranded-capacity rationale
  │     └── Genuine Discovery: Auto-discovers counterexamples across all strategies
  ├── Fractional Knapsack Problem:
  │     ├── Greedy: Continuous Value/Weight Ratio (Mathematically Optimal)
  │     ├── Exact Oracle: Continuous Relaxation Optimum
  │     └── Scientific Contrast: 0 counterexamples vs 0/1 Knapsack failures
  ├── Generic Search Architecture:
  │     ├── Unified `SearchEngine`, `SearchConfig`, `SearchResult`
  │     └── Streaming candidate generators (`stream_xxx_candidates`)
  └── Complete Test Suite: 80/80 GoogleTests passing
          ↓
Phase 4: FastAPI Integration & Persistence Layer [NEXT]
          ↓
Phase 5: React + TypeScript Research/Forensic UI [FUTURE]
```

---

## 3. Problem Abstraction and Registry

### 3.1 Common Problem Base (`GreedyProblem<CandidateType, ItemType>`)
Defined in `engine/include/greedybreaker/core/problem.hpp`:
```cpp
template <typename CandidateType, typename ItemType = int>
class GreedyProblem {
public:
    virtual std::string name() const = 0;
    virtual ProblemObjective objective() const = 0;
    virtual std::string default_strategy() const = 0;
    virtual std::vector<std::string> available_strategies() const = 0;

    virtual void validate_candidate(const CandidateType& candidate) const = 0;
    virtual Solution<ItemType> greedy_solution(const CandidateType& candidate, const std::string& strategy = "") const = 0;
    virtual Solution<ItemType> optimal_solution(const CandidateType& candidate) const = 0;
    virtual ComparisonResult compare_solutions(const Solution<ItemType>& greedy, const Solution<ItemType>& optimal) const;
    virtual DivergenceResult analyze_divergence(const CandidateType& candidate, const Solution<ItemType>& greedy, const Solution<ItemType>& optimal) const = 0;
    virtual ComplexityReport complexity_info() const = 0;
};
```

### 3.2 Problem Registry (`ProblemType`)
Defined in `engine/include/greedybreaker/core/registry.hpp`:
- `COIN_CHANGE`: slug `coin-change` (Minimization)
- `ACTIVITY_SELECTION`: slug `activity-selection` (Maximization)
- `KNAPSACK_01`: slug `knapsack-01` (Maximization)
- `KNAPSACK_FRACTIONAL`: slug `knapsack-fractional` (Maximization)

---

## 4. Generic Objective & Metrics Handling

Defined in `engine/include/greedybreaker/core/metrics.hpp` and implemented in `metrics.cpp`:
- **Minimization** (e.g. Coin Change):
  $$\text{Absolute Gap} = \text{Greedy} - \text{Optimal}$$
  $$\text{Ratio} = \frac{\text{Greedy}}{\text{Optimal}}$$
  $$\text{Overhead} = \frac{\text{Greedy} - \text{Optimal}}{\text{Optimal}} \times 100\%$$
- **Maximization** (e.g. Activity Selection, 0/1 Knapsack, Fractional Knapsack):
  $$\text{Absolute Gap} = \text{Optimal} - \text{Greedy}$$
  $$\text{Deficit Ratio} = \frac{\text{Optimal}}{\text{Greedy}}$$
  $$\text{Deficit Percentage} = \frac{\text{Optimal} - \text{Greedy}}{\text{Optimal}} \times 100\%$$

---

## 5. Problem Implementations & Forensic Oracles

### 5.1 Activity Selection
- **Domain**: Mutually compatible non-overlapping intervals $[s_i, f_i)$.
- **Validation**: Rejects empty sets, non-positive durations ($s \ge f$), negative times, and duplicate IDs.
- **Greedy Strategy**: Earliest Finish Time First (ties broken by earlier start, then smaller ID).
- **Exact DP Oracle**: Dynamic Programming recurrence:
  $$OPT(j) = \max(OPT(j-1), 1 + OPT(p(j)))$$
  where $p(j)$ is the latest compatible activity finishing before activity $j$ starts.
- **Empirical Scientific Verification**: Provably optimal. Bounded search exhausts the candidate space and discovers **0 counterexamples**, confirming empirical correctness without claiming universal unconstrained mathematical proof.

### 5.2 0/1 Knapsack
- **Domain**: Items $(v_i, w_i)$, Knapsack capacity $W$. Items chosen at most once.
- **Validation**: Rejects empty items, $W \le 0$, $w_i \le 0$, $v_i < 0$, and duplicate IDs.
- **Greedy Strategy**: Value/Weight ratio descending:
  $$\text{ratio}_i = \frac{v_i}{w_i}$$
  Deterministic tie-breaking: higher ratio $\to$ higher value $\to$ lower weight $\to$ lower item ID.
- **Exact DP Oracle**: 2D dynamic programming:
  $$dp[i][w] = \max(dp[i-1][w], v_{i-1} + dp[i-1][w - w_{i-1}])$$
  Time complexity $\Theta(nW)$, space $\Theta(nW)$ enabling exact subset reconstruction.
- **Forensic First-Divergence**: Pinpoints the exact step where greedy selected a high-ratio item that consumed critical capacity, blocking an optimal combination.

### 5.3 Fractional Knapsack
- **Domain**: Items $(v_i, w_i)$, Knapsack capacity $W$, allowing continuous fractions $0 \le x_i \le 1$.
- **Greedy Strategy**: Continuous value/weight ratio descending. Fully saturates capacity.
- **Exact Oracle**: Continuous relaxation global optimum.
- **Scientific Contrast**: Proves the mathematical divide: ratio greedy succeeds on Fractional Knapsack, but fails on 0/1 Knapsack due to indivisibility and stranded capacity.

---

## 6. Algorithmic Complexity Matrix

| Problem | Solver | Time Complexity | Space Complexity | Class / Property |
| :--- | :--- | :--- | :--- | :--- |
| **Coin Change** | Greedy (Largest First) | $O(K \log K + T / c_{\min})$ | $O(T / c_{\min})$ | NP-complete general, Heuristic |
| | Exact Oracle (DP) | $\Theta(K \cdot T)$ | $O(T)$ | Pseudo-polynomial DP |
| **Activity Selection** | Greedy (Earliest Finish) | $O(n \log n)$ | $O(n)$ | Matroid / Greedy-Choice Property |
| | Exact Oracle (DP) | $O(n \log n)$ or $O(n^2)$ | $O(n)$ | Dynamic Programming |
| **0/1 Knapsack** | Greedy (Ratio Descending) | $O(n \log n)$ | $O(n)$ | NP-hard, Heuristic (Breakable) |
| | Exact Oracle (DP) | $\Theta(n \cdot W)$ | $O(n \cdot W)$ | Pseudo-polynomial DP |
| **Fractional Knapsack** | Greedy (Continuous Ratio) | $O(n \log n)$ | $O(n)$ | Continuous Greedy-Choice Property |
| | Exact Oracle | $O(n \log n)$ | $O(n)$ | Continuous Global Optimum |

---

## 7. Verification and Testing

The C++20 engine is validated by 80 automated unit and integration tests under GoogleTest v1.14:
- `test_coin_change.cpp`: 18 tests (Coin Change engine, famous counterexamples, dead ends)
- `test_metrics.cpp`: 6 tests (Minimization and Maximization gap/ratio/overhead calculations)
- `test_traces.cpp`: 3 tests (Execution trace step structure and divergence results)
- `test_search_common.cpp`: 10 tests (SearchConfig boundaries, comparators, cancellation)
- `test_exhaustive_search.cpp`: 5 tests (ANY, SMALLEST, WORST modes, determinism)
- `test_backtracking_search.cpp`: 4 tests (Branch pruning, safety limits)
- `test_randomized_search.cpp`: 4 tests (PRNG seed reproducibility)
- `test_heuristic_search.cpp`: 3 tests (Kozen-Zaks vulnerability synthesis)
- `test_activity_selection.cpp`: 8 tests (Validation, CLRS instance, 0 counterexamples, search)
- `test_knapsack_01.cpp`: 10 tests (Validation, counterexamples, divergence, all search strategies)
- `test_knapsack_fractional.cpp`: 5 tests (Fractional selection, exact match, 0 counterexamples)
- `test_problem_registry.cpp`: 4 tests (Registry parsing, cross-problem search engine reuse)
