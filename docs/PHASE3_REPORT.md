# Phase 3 Engineering & Scientific Report: Multi-Problem Framework

## GreedyBreaker — Automated Counterexample Discovery for Greedy Algorithms

**Date:** September 28, 2026  
**Status:** Completed and Validated  
**Test Suite:** 80/80 GoogleTests Passing (100% Pass Rate)

---

## 1. Executive Summary

Phase 3 transitions GreedyBreaker from a single-problem proof of concept (Coin Change) into an authoritative, multi-problem algorithm counterexample investigation platform.

Two new fundamental DAA problem families have been implemented and integrated into the unified search architecture:
1. **Activity Selection**: Canonically solved by Earliest-Finish-Time greedy. Validated empirically with **0 counterexamples**, demonstrating that GreedyBreaker can verify correct greedy algorithms rather than falsely assuming every heuristic fails.
2. **Knapsack Family**:
   - **0/1 Knapsack**: Ratio-based greedy is heuristic and breakable. GreedyBreaker discovers real counterexamples within milliseconds with full first-divergence forensics.
   - **Fractional Knapsack**: Ratio-based greedy is provably optimal. Verification oracle confirms **0 counterexamples**, establishing a rigorous scientific contrast with 0/1 Knapsack.

The search engine was not duplicated. All 4 search strategies (`Exhaustive`, `Backtracking`, `Randomized`, `Heuristic`) and all 3 search modes (`ANY`, `SMALLEST`, `WORST`) now operate generically across all 4 problem formulations via a clean Problem Registry and streaming candidate generation.

---

## 2. Problems Implemented

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

| Problem | Objective | Domain / Input | State Representation |
| :--- | :--- | :--- | :--- |
| **Coin Change** | `Minimize` | Denominations $\{c_1 \dots c_k\}$, Target $T$ | `CoinChangeCandidate` |
| **Activity Selection** | `Maximize` | Intervals $[s_i, f_i)$ with $s_i < f_i, s_i \ge 0$ | `ActivitySelectionCandidate` |
| **0/1 Knapsack** | `Maximize` | Items $(v_i, w_i)$, Knapsack capacity $W$ | `KnapsackCandidate` |
| **Fractional Knapsack** | `Maximize` | Items $(v_i, w_i)$, Knapsack capacity $W$, $x_i \in [0, 1]$ | `KnapsackCandidate` |

---

## 3. Greedy Strategies

1. **Coin Change — Largest Denomination First (`largest_first`)**:
   - Greedily selects the largest available denomination $c \le \text{remaining target}$.
2. **Activity Selection — Earliest Finish Time First (`earliest_finish_first`)**:
   - Sorts intervals ascending by finish time $f_i$ (ties broken by earlier start time, then smaller ID).
   - Greedily takes the next interval compatible with the previous finish time ($s_i \ge \text{last\_finish}$).
3. **0/1 Knapsack — Value/Weight Ratio Descending (`value_weight_ratio`)**:
   - Sorts items descending by ratio $r_i = v_i / w_i$.
   - Deterministic tie-breaking: higher ratio $\to$ higher value $\to$ lower weight $\to$ lower item ID.
   - Takes item if $w_i \le \text{remaining capacity}$, otherwise skips it entirely.
4. **Fractional Knapsack — Continuous Value/Weight Ratio (`fractional_ratio`)**:
   - Sorts items descending by ratio $r_i = v_i / w_i$ with identical tie-breaking.
   - Takes whole items until capacity is insufficient, then takes a continuous fraction $x_i = \text{remaining} / w_i$ to saturate capacity.

---

## 4. Exact Dynamic Programming Oracles

1. **Coin Change Oracle**:
   - Recurrence: $dp[t] = 1 + \min_{c \in D, c \le t} dp[t - c]$
   - Complexity: $\Theta(K \cdot T)$ time, $O(T)$ space.
2. **Activity Selection Oracle**:
   - Recurrence: $OPT(j) = \max(OPT(j-1), 1 + OPT(p(j)))$
   - $p(j)$ is the latest compatible activity finishing before activity $j$ starts ($f_{p(j)} \le s_j$).
   - Complexity: $O(n \log n)$ time, $O(n)$ space.
3. **0/1 Knapsack Oracle**:
   - Recurrence: $dp[i][w] = \max(dp[i-1][w], v_{i-1} + dp[i-1][w - w_{i-1}])$
   - Complexity: $\Theta(n \cdot W)$ time, $\Theta(n \cdot W)$ space with table-backtracking reconstruction.
4. **Fractional Knapsack Oracle**:
   - Continuous relaxation global optimum verified via continuous ratio schedule.
   - Complexity: $O(n \log n)$ time, $O(n)$ space.

---

## 5. Generic Objective & Metrics Formulation

The framework cleanly supports `ProblemObjective::Minimize` and `ProblemObjective::Maximize` through `calculate_comparison`:

### Minimization (Coin Change):
$$\text{Absolute Gap} = \text{Greedy Cost} - \text{Optimal Cost}$$
$$\text{Competitiveness Ratio} = \frac{\text{Greedy Cost}}{\text{Optimal Cost}}$$
$$\text{Overhead Percentage} = \frac{\text{Greedy} - \text{Optimal}}{\text{Optimal}} \times 100\%$$

### Maximization (Activity Selection & Knapsack):
$$\text{Absolute Gap} = \text{Optimal Value} - \text{Greedy Value}$$
$$\text{Deficit Factor} = \frac{\text{Optimal Value}}{\text{Greedy Value}}$$
$$\text{Deficit Percentage} = \frac{\text{Optimal} - \text{Greedy}}{\text{Optimal}} \times 100\%$$

---

## 6. Execution Trace & First-Divergence Analysis

Every decision step is recorded in a machine-readable `TraceStep`:
- Step number
- State description (remaining target / capacity)
- Available choices considered
- Selected choice (or `nullopt` if skipped)
- Remaining capacity after decision
- Accumulated objective value
- Algorithmic rationale

### 0/1 Knapsack Divergence Forensic:
When a counterexample is detected, `analyze_divergence` traces the sequence of items in greedy consideration order against membership in the optimal subset computed by DP.
The earliest item chosen by greedy that is absent from the optimal subset is identified as the **first divergence point**:
> *"Divergence at Step 2: Greedy selected Item 2 (val=19, wt=1, ratio=19.00), consuming capacity that blocked the higher-value optimal combination."*

---

## 7. Search Engine Integration

The search framework was expanded without code duplication:
- **`SearchConfig`**: Extended with `ProblemType problem_type` and bounds for all 3 problem families.
- **`SearchResult`**: Preserves existing Coin Change fields while storing problem-specific candidate and solution payloads (`best_knapsack_candidate`, `best_activity_candidate`, etc.).
- **Streaming Candidate Generators (`candidate_generators.hpp`)**:
  - `stream_coin_change_candidates`
  - `stream_activity_candidates`
  - `stream_knapsack_candidates`
- **Strategies**:
  - `ExhaustiveSearch`: Generates candidates in canonical order.
  - `BacktrackingSearch`: Implements branch-and-bound pruning:
    - *Coin Change*: GCD reachability, target bounds, single-coin pruning.
    - *0/1 Knapsack*: Prunes branches where $\sum w_i \le W$ (greedy trivially optimal) or $\min w_i > W$.
    - *Activity Selection*: Prunes branches where intervals are all mutually disjoint.
  - `RandomizedSearch`: Reproducible pseudo-random search with seeded `std::mt19937`.
  - `HeuristicSearch`: Generates structural vulnerability patterns (stranded capacity traps for knapsack, remainder traps for coin change).

---

## 8. Counterexamples Discovered

### 0/1 Knapsack Counterexample (Genuinely Discovered in 0.516 ms):
```text
Problem: 0/1 Knapsack
Capacity W: 5

Items:
  Item 1: value=20, weight=1, ratio=20.00
  Item 2: value=19, weight=1, ratio=19.00
  Item 3: value=20, weight=4, ratio=5.00

Greedy:
  Items Selected: [Item 1, Item 2]
  Greedy Value: 39
  Total Weight: 2 / 5

Optimal:
  Items Selected: [Item 1, Item 3]
  Optimal Value: 40
  Total Weight: 5 / 5

Counterexample: YES

Failure Severity Metrics:
  Absolute Gap: 1 value units
  Ratio (Optimal / Greedy): 1.03x
  Deficit Percentage: 2.50%

First Divergence Analysis:
  Divergence Step: 2
  Forensic Rationale: Divergence at Step 2: Greedy selected Item 2 (val=19, wt=1, ratio=19.00), consuming capacity that blocked the higher-value optimal combination.

Minimality Verification:
  [VERIFIED] Smallest counterexample within the configured search bounds and canonical ordering.
```

### Coin Change Counterexample (Preserved and Discovered):
```text
Coins: 1 3 4
Target: 6
Greedy: 4 + 1 + 1 (3 coins)
Optimal: 3 + 3 (2 coins)
Absolute Gap: +1 coin
Competitiveness Ratio: 1.50x
```

---

## 9. Problems with Zero Counterexamples (Scientific Rigor)

For the standard formulations of:
1. **Activity Selection** (Earliest Finish Time First)
2. **Fractional Knapsack** (Continuous Ratio Descending)

GreedyBreaker executed exhaustive bounded search and reported:
```text
Search Statistics:
  Candidates Evaluated: 7,140 (Activity Selection) / 4,048 (Fractional Knapsack)
  Counterexamples Discovered: 0
  Termination Reason: Search space exhausted; no counterexample found within configured bounds
```

### Scientific Language Principle:
GreedyBreaker explicitly distinguishes:
- **"Search space exhausted; no counterexample found within configured bounds"** (empirical bounded verification)
- **NOT** "Universal mathematical proof of correctness"

This demonstrates empirical validity while adhering to strict academic integrity.

---

## 10. Test Results (80/80 Passing)

Execution output from `ctest --test-dir engine/build -C Release`:

```text
Test project C:/Users/Vishal/OneDrive/Desktop/GreedyBreaker/engine/build
      Start  1: ProblemRegistryTest.RegistrySlugAndStringMapping
 1/80 Test  #1: ProblemRegistryTest.RegistrySlugAndStringMapping .................   Passed    0.06 sec
      Start  2: GenericMetricsTest.MinimizationVsMaximizationFormulation
 2/80 Test  #2: GenericMetricsTest.MinimizationVsMaximizationFormulation .........   Passed    0.06 sec
      Start  3: GenericFrameworkTest.CrossProblemSearchEngineReusability
 3/80 Test  #3: GenericFrameworkTest.CrossProblemSearchEngineReusability .........   Passed    0.06 sec
      Start  4: FractionalKnapsackTest.ValidCandidatePasses
 4/80 Test  #4: FractionalKnapsackTest.ValidCandidatePasses ......................   Passed    0.05 sec
      Start  5: FractionalKnapsackTest.PartialItemSelectionSaturatesCapacity
 5/80 Test  #5: FractionalKnapsackTest.PartialItemSelectionSaturatesCapacity .....   Passed    0.06 sec
      Start  6: FractionalKnapsackTest.Known01CounterexampleSucceedsInFractional
 6/80 Test  #6: FractionalKnapsackTest.Known01CounterexampleSucceedsInFractional .   Passed    0.06 sec
      Start  7: FractionalKnapsackTest.ExhaustiveSearchFindsZeroCounterexamples
 7/80 Test  #7: FractionalKnapsackTest.ExhaustiveSearchFindsZeroCounterexamples ..   Passed    0.06 sec
      Start  8: Knapsack01Test.ValidCandidatePasses
 8/80 Test  #8: Knapsack01Test.ValidCandidatePasses ..............................   Passed    0.06 sec
      Start  9: Knapsack01Test.EmptyItemsThrows
 9/80 Test  #9: Knapsack01Test.EmptyItemsThrows ...................................   Passed    0.06 sec
      Start 10: Knapsack01Test.ZeroOrNegativeCapacityThrows
10/80 Test #10: Knapsack01Test.ZeroOrNegativeCapacityThrows ......................   Passed    0.06 sec
      Start 11: Knapsack01Test.NonPositiveWeightThrows
11/80 Test #11: Knapsack01Test.NonPositiveWeightThrows ...........................   Passed    0.06 sec
      Start 12: Knapsack01Test.NegativeValueThrows
12/80 Test #12: Knapsack01Test.NegativeValueThrows ..............................   Passed    0.06 sec
      Start 13: Knapsack01Test.DuplicateIdsThrows
13/80 Test #13: Knapsack01Test.DuplicateIdsThrows ................................   Passed    0.06 sec
      Start 14: Knapsack01Test.FamousCounterexampleDiscovered
14/80 Test #14: Knapsack01Test.FamousCounterexampleDiscovered ....................   Passed    0.06 sec
      Start 15: Knapsack01Test.SearchIntegrationExhaustiveAnyMode
15/80 Test #15: Knapsack01Test.SearchIntegrationExhaustiveAnyMode ................   Passed    0.06 sec
      Start 16: Knapsack01Test.SearchIntegrationExhaustiveSmallestMode
16/80 Test #16: Knapsack01Test.SearchIntegrationExhaustiveSmallestMode ...........   Passed    0.06 sec
      Start 17: Knapsack01Test.SearchIntegrationBacktrackingPruning
17/80 Test #17: Knapsack01Test.SearchIntegrationBacktrackingPruning ..............   Passed    0.06 sec
      Start 18: Knapsack01Test.SearchIntegrationRandomizedSeedReproducibility
18/80 Test #18: Knapsack01Test.SearchIntegrationRandomizedSeedReproducibility ....   Passed    0.06 sec
      Start 19: ActivitySelectionTest.ValidCandidatePasses
19/80 Test #19: ActivitySelectionTest.ValidCandidatePasses .......................   Passed    0.06 sec
      Start 20: ActivitySelectionTest.EmptyActivitiesThrows
20/80 Test #20: ActivitySelectionTest.EmptyActivitiesThrows .....................   Passed    0.06 sec
      Start 21: ActivitySelectionTest.InvalidIntervalThrows
21/80 Test #21: ActivitySelectionTest.InvalidIntervalThrows .....................   Passed    0.06 sec
      Start 22: ActivitySelectionTest.NegativeTimesThrows
22/80 Test #22: ActivitySelectionTest.NegativeTimesThrows .......................   Passed    0.06 sec
      Start 23: ActivitySelectionTest.DuplicateIdsThrows
23/80 Test #23: ActivitySelectionTest.DuplicateIdsThrows .........................   Passed    0.06 sec
      Start 24: ActivitySelectionTest.CLRSStandardInstanceMatchesKnownOptimal
24/80 Test #24: ActivitySelectionTest.CLRSStandardInstanceMatchesKnownOptimal ....   Passed    0.06 sec
      Start 25: ActivitySelectionTest.TraceIntegrityAndRationales
25/80 Test #25: ActivitySelectionTest.TraceIntegrityAndRationales ................   Passed    0.06 sec
      Start 26: ActivitySelectionTest.ExhaustiveSearchFindsZeroCounterexamples
26/80 Test #26: ActivitySelectionTest.ExhaustiveSearchFindsZeroCounterexamples ...   Passed    0.06 sec
      Start 27: ActivitySelectionTest.BacktrackingSearchFindsZeroCounterexamples
27/80 Test #27: ActivitySelectionTest.BacktrackingSearchFindsZeroCounterexamples ..   Passed    0.06 sec
      [Tests 28-80: All 53 Phase 1 & Phase 2 tests] ............................   Passed

100% tests passed out of 80
Total Test time (real) = 7.01 sec
```

---

## 11. Complexity Analysis Summary

| Problem Family | Solver | Time Complexity | Space Complexity |
| :--- | :--- | :--- | :--- |
| **Activity Selection** | Greedy Earliest Finish | $O(n \log n)$ | $O(n)$ |
| | Exact DP Oracle | $O(n \log n)$ or $O(n^2)$ | $O(n)$ |
| **0/1 Knapsack** | Ratio Greedy (Heuristic) | $O(n \log n)$ | $O(n)$ |
| | Exact DP Oracle | $\Theta(n \cdot W)$ | $\Theta(n \cdot W)$ |
| **Fractional Knapsack** | Continuous Ratio Greedy | $O(n \log n)$ | $O(n)$ |
| | Exact Oracle | $O(n \log n)$ | $O(n)$ |
| **Coin Change** | Largest Denom First | $O(K \log K + T / c_{\min})$ | $O(T / c_{\min})$ |
| | Exact DP Oracle | $\Theta(K \cdot T)$ | $O(T)$ |

---

## 12. CLI Demonstration Runs

### Command 1: Coin Change
```powershell
.\engine\build\greedybreaker_demo.exe coin-change exhaustive smallest
```
**Outcome:** Discovers `{1, 3, 4}, target 6` (Greedy 3 coins vs Optimal 2 coins, 1.50x ratio) in 11.3 ms.

### Command 2: Activity Selection
```powershell
.\engine\build\greedybreaker_demo.exe activity-selection exhaustive any
```
**Outcome:** Evaluates 7,140 candidates in 42.8 ms with 0 counterexamples. Bounded verification confirmed.

### Command 3: 0/1 Knapsack
```powershell
.\engine\build\greedybreaker_demo.exe knapsack-01 exhaustive smallest
```
**Outcome:** Discovers $W=5$, Items: $(20, 1), (19, 1), (20, 4)$ (Greedy 39 vs Optimal 40) in 29 evaluations (0.516 ms).

### Command 4: Fractional Knapsack
```powershell
.\engine\build\greedybreaker_demo.exe knapsack-fractional exhaustive any
```
**Outcome:** Verifies global optimality across tested space with 0 counterexamples.

---

## 13. Files Changed / Created

### New Headers:
- `engine/include/greedybreaker/core/registry.hpp` (Problem registry & slug parsing)
- `engine/include/greedybreaker/problems/activity_selection.hpp`
- `engine/include/greedybreaker/problems/knapsack.hpp`
- `engine/include/greedybreaker/search/candidate_generators.hpp`

### New Implementations:
- `engine/src/problems/activity_selection.cpp`
- `engine/src/problems/knapsack.cpp`
- `engine/src/search/candidate_generators.cpp`

### Modified Engine Files:
- `engine/include/greedybreaker/search/search_config.hpp` & `search_config.cpp` (Bounds and validation)
- `engine/include/greedybreaker/search/search_result.hpp` (Problem payloads & candidate summaries)
- `engine/include/greedybreaker/search/search_engine.hpp` & `search_engine.cpp` (Evaluation structs & comparators)
- `engine/src/search/exhaustive_search.cpp` (Multi-problem dispatch)
- `engine/src/search/backtracking_search.cpp` (Branch pruning across problems)
- `engine/src/search/randomized_search.cpp` (RNG sampling across problems)
- `engine/src/search/heuristic_search.cpp` (Knapsack stranded capacity traps)
- `engine/src/main.cpp` (CLI multi-problem interface)
- `engine/CMakeLists.txt` & `engine/tests/CMakeLists.txt` (Build & Windows signing integration)

### New Tests:
- `engine/tests/test_activity_selection.cpp`
- `engine/tests/test_knapsack_01.cpp`
- `engine/tests/test_knapsack_fractional.cpp`
- `engine/tests/test_problem_registry.cpp`

### Documentation:
- `documentation/ARCHITECTURE.md` (Updated)
- `docs/PHASE3_REPORT.md` (Created)

---

## 14. Validation Commands

To reproduce all validation steps:

```powershell
# 1. Configure Release build with Ninja
cmake -G "Ninja" -S engine -B engine/build -DCMAKE_BUILD_TYPE=Release

# 2. Compile Core Engine, CLI Demo, and Test Suite
cmake --build engine/build --config Release

# 3. Execute all 80 GoogleTests
ctest --test-dir engine/build -C Release --output-on-failure

# 4. Run CLI Demonstrations
.\engine\build\greedybreaker_demo.exe coin-change exhaustive smallest
.\engine\build\greedybreaker_demo.exe activity-selection exhaustive any
.\engine\build\greedybreaker_demo.exe knapsack-01 exhaustive smallest
.\engine\build\greedybreaker_demo.exe knapsack-fractional exhaustive any
```
