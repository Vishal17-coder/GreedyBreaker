# GreedyBreaker — Phase 2 Completion Report

## C++20 Counterexample Search Engine

**Date:** 2026-09-28  
**Phase:** Phase 2 (Search Engine)  
**Status:** Complete & Fully Validated  

---

## 1. Executive Summary

Phase 2 of **GreedyBreaker** implements an automated counterexample discovery engine in **C++20**. The system no longer requires manual input of known failure instances like `{1, 3, 4}, target = 6`. Instead, it automatically navigates the candidate input space, executes the greedy heuristic, executes the exact dynamic programming oracle, identifies sub-optimality, analyzes the first decision point of divergence, evaluates failure severity metrics, and reports verified counterexamples.

---

## 2. Search Engine Architecture

```text
                           SearchEngine (Abstract Base)
                                       │
        ┌───────────────────┬──────────┴─────────┬──────────────────┐
        ▼                   ▼                    ▼                  ▼
ExhaustiveSearch   BacktrackingSearch    RandomizedSearch    HeuristicSearch
```

### Core Abstractions (`engine/include/greedybreaker/search/`):
* **`SearchMode`**: `ANY` (fast exit), `SMALLEST` (minimal within bounds), `WORST` (maximize severity).
* **`SearchStrategy`**: `EXHAUSTIVE`, `BACKTRACKING`, `RANDOMIZED`, `HEURISTIC`.
* **`SearchConfig`**: Strongly typed parameter configuration with boundary validation and safety caps.
* **`SearchResult`**: Complete telemetry (`candidates_generated`, `candidates_evaluated`, `nodes_explored`, `branches_pruned`, `counterexamples_found`, `duration_ms`), verified counterexample candidate, greedy solution, optimal solution, comparison metrics, first divergence, and minimality verification.
* **`CandidateEvaluation evaluate_candidate()`**: Unified evaluation pipeline shared by all four search strategies.

---

## 3. Search Modes

| Mode | Objective | Behavior | Minimality Status |
| :--- | :--- | :--- | :--- |
| **`ANY`** | Find any counterexample as fast as possible. | Terminates immediately on the first candidate where greedy cost exceeds optimal cost or greedy fails. | `smallest_verified = false` |
| **`SMALLEST`** | Find the minimal counterexample according to canonical ordering. | For `EXHAUSTIVE` and `BACKTRACKING`, candidate traversal is strictly ordered by canonical criteria; the first counterexample discovered is guaranteed to be minimal within bounds. | `smallest_verified = true` (within bounds) |
| **`WORST`** | Maximize failure severity. | Searches the configured bounds (or up to safety limit), keeping the counterexample with highest ratio ($g / opt$), then highest gap ($g - opt$). | `smallest_verified = false` |

---

## 4. Search Strategies

### A. Exhaustive Search (`ExhaustiveSearch`)
* Systematically enumerates Cartesian candidate space in canonical order:
  1. Target $T \in [min\_target .. max\_target]$ ascending
  2. Denomination count $k \in [min\_coin\_count .. max\_coin\_count]$ ascending
  3. Denomination combinations $d_1 < d_2 < \dots < d_k$ in lexicographical order
* Deterministic and complete over the bounded search space.

### B. Backtracking Search (`BacktrackingSearch`)
* Recursively constructs candidate subsets with real mathematical pruning:
  * **Rule 1 (Single-Coin Pruning)**: Sets of size $k = 1$ are always optimal under change-making; pruned immediately.
  * **Rule 2 (Target Bound Pruning)**: If first coin $d_1 > T$, all subsequent coins are $> T$; sub-tree pruned immediately.
  * **Rule 3 (Capacity Pruning)**: If remaining available integers in range cannot fill remaining $k - |D|$ slots, pruned.
  * **Rule 4 (GCD Reachability Pruning)**: By Bézout's identity, if $T \pmod{\gcd(d_1, \dots, d_k)} \neq 0$, target cannot be formed by any linear combination; pruned before expensive DP evaluation.
* Telemetry records `nodes_explored` and `branches_pruned`.

### C. Randomized Search (`RandomizedSearch`)
* Pseudo-random candidate generation using C++20 `std::mt19937`.
* Driven by a configurable integer `random_seed` (default 42).
* **100% Bit-Exact Reproducibility**: Tested and verified that identical seeds and configurations produce identical candidate sequences and identical counterexamples across independent runs.

### D. Heuristic Search (`HeuristicSearch`)
* Deterministic algorithmic search exploiting the **Kozen-Zaks theorem** on non-canonical currency failure:
  * Tests "Double-Coin Remainder Traps": pairs $(a, b)$ with $a < b < 2a$ at target $T = 2a$.
  * At $T = 2a$, optimal uses $a + a = 2a$ (cost 2). Greedy takes $b$, leaving $2a - b$ which requires multiple small coins (cost $\ge 3$).
  * Discovers counterexamples in as few as **2 candidate evaluations**.
  * Honestly reports `smallest_verified = false` because it traverses by trap priority rather than size ordering.

---

## 5. Candidate Verification Pipeline

Every generated candidate passes through the authoritative pipeline:

```text
Candidate Input (Denominations, Target)
            │
            ▼
    validate_candidate()
            │
            ▼
     greedy_solution()
            │
            ▼
    optimal_solution()  (DP Oracle)
            │
            ▼
    compare_solutions()
            │
      Is Counterexample?
         /          \
       YES           NO ──▶ Next Candidate
        │
        ▼
analyze_divergence()
        │
        ▼
Update SearchResult / Check Termination
```

---

## 6. Canonical Ordering for Minimality

For Coin Change, candidate $A = (D_A, T_A)$ is smaller than $B = (D_B, T_B)$ ($A < B$) iff:
1. $T_A < T_B$ (smaller target)
2. $|D_A| < |D_B|$ (fewer denominations)
3. $D_A < D_B$ (lexicographical order of sorted denominations)

---

## 7. Complexity Analysis

| Strategy | Candidate Space Complexity | Evaluation Cost per Candidate | Overall Asymptotic Time |
| :--- | :--- | :--- | :--- |
| **Exhaustive** | $O\left(\sum_{T} \sum_{k} \binom{R}{k}\right)$ where $R = max\_d - min\_d + 1$ | $O(K \cdot T)$ (DP Oracle) | $O\left(T_{\max} \cdot \binom{R}{k_{\max}} \cdot K \cdot T_{\max}\right)$ |
| **Backtracking** | Same worst-case bound, reduced by $O(GCD)$ and target pruning | $O(K \cdot T)$ | Significantly lower practical constant factor |
| **Randomized** | $O(I)$ where $I = \text{iterations}$ | $O(K \cdot T)$ | $O(I \cdot K \cdot T)$ |
| **Heuristic** | $O(R^2)$ trap pairs tested first | $O(K \cdot T)$ | $O(R^2 \cdot K \cdot T)$ (rapid discovery in $O(1)$ to $O(R)$ trials) |

---

## 8. Test Results (53/53 Passed)

Built and executed via CMake + Ninja with GCC 16 (C++20):

```text
[==========] Running 53 tests from 8 test suites.
[----------] 3 tests from HeuristicSearchTest (Passed)
[----------] 4 tests from RandomizedSearchTest (Passed)
[----------] 4 tests from BacktrackingSearchTest (Passed)
[----------] 5 tests from ExhaustiveSearchTest (Passed)
[----------] 10 tests from SearchCommonTest (Passed)
[----------] 18 tests from CoinChangeTest (Passed)
[----------] 3 tests from TraceTest (Passed)
[----------] 6 tests from MetricsTest (Passed)
[==========] 53 tests from 8 test suites ran. (35 ms total)
[  PASSED  ] 53 tests. (100% success rate)
```

### Key Verification Tests:
* **Exhaustive Smallest**: Verified that exhaustive search automatically discovers $\{1, 3, 4\}$, target 6 as the canonical smallest counterexample under $1 \in D$.
* **Exhaustive Worst**: Verified that search across target 6..15 maximizes severity to ratio 3.50x (overhead 250%).
* **Backtracking Pruning**: Verified that `branches_pruned > 0` and `nodes_explored > candidates_evaluated`.
* **Randomized Reproducibility**: Verified that two separate runs with seed 12345 produce bit-exact identical candidates, evaluations, and discovered counterexamples.
* **Heuristic Rapidity**: Verified that heuristic search discovers a counterexample in $\le 50$ evaluations.
* **Safety Limits**: Verified that setting `max_candidates = 10` halts search immediately with `search_limit_reached = true`.
* **Cancellation**: Verified that the cooperative cancellation callback aborts cleanly.

---

## 9. CLI Demonstration Outputs

### Run 1: Exhaustive Smallest (Default)
`.\engine\build\greedybreaker_demo.exe exhaustive smallest`
```text
Problem: Coin Change
Strategy: EXHAUSTIVE
Mode: SMALLEST

Search Bounds:
  Denominations: [1..10]
  Coin Count: [2..4]
  Target Range: [1..30]
  Max Safety Limit: 50000 candidates

Searching for counterexamples...

Search Statistics:
  Candidates Generated: 663
  Candidates Evaluated: 663
  Counterexamples Discovered: 1
  Execution Time: 14.332 ms
  Termination Reason: Smallest verified counterexample found (SMALLEST mode)

----------------------------------------------------
           COUNTEREXAMPLE DISCOVERED                
----------------------------------------------------

Coins: 1 3 4
Target: 6

Greedy:
  4 + 1 + 1
  Cost: 3 coins

Optimal:
  3 + 3
  Cost: 2 coins

Failure Severity Metrics:
  Absolute Gap: +1 coin(s)
  Competitiveness Ratio: 1.50x
  Overhead: 50.00%

First Divergence Analysis:
  Divergence Step: 1
  State at Divergence: remaining target 6
  Greedy Choice: 4
  Optimal-Compatible Choice(s): {3}

Forensic Rationale:
  FIRST DIVERGENCE AT STEP 1: At remaining target 6, greedy selected denomination 4 (the locally largest valid coin). However, selecting 4 leaves remaining target 2, which requires 2 additional coins (projected total 3 coins). In contrast, choosing an optimal coin from {3} requires only 2 coins in total from this state (global total 2 coins). This greedy decision irreversibly forced an extra 1 coin(s).

Minimality Verification:
  [VERIFIED] Smallest counterexample within the configured search bounds and canonical ordering.
```

### Run 2: Backtracking Smallest
`.\engine\build\greedybreaker_demo.exe backtracking smallest`
* Telemetry: `Candidates Generated: 663`, `Nodes Explored: 897`, `Branches Pruned: 45`.
* Discovered: `{1, 3, 4}, target = 6` in **11.5 ms**.

### Run 3: Randomized Search (Seed 42)
`.\engine\build\greedybreaker_demo.exe randomized any`
* Telemetry: `Candidates Evaluated: 13`, `Iterations: 13`, `Execution Time: 0.493 ms`.
* Discovered: `{1, 4, 5, 7}, target = 9`. Greedy: 7 + 1 + 1 (3 coins), Optimal: 4 + 5 (2 coins).
* Minimality: `[UNVERIFIED] Not proven minimal (mode=ANY, strategy=RANDOMIZED)`.

### Run 4: Heuristic Search
`.\engine\build\greedybreaker_demo.exe heuristic any`
* Telemetry: `Candidates Evaluated: 2`, `Execution Time: 0.679 ms`.
* Discovered: `{1, 3, 4}, target = 6`.

### Run 5: Exhaustive Worst
`.\engine\build\greedybreaker_demo.exe exhaustive worst 6 15`
* Telemetry: `Candidates Evaluated: 1290`, `Counterexamples Discovered: 164`.
* Worst Discovered: `{1, 7, 8}, target = 14`.
* Greedy: 8 + 1 + 1 + 1 + 1 + 1 + 1 (7 coins) vs Optimal: 7 + 7 (2 coins).
* Ratio: **3.50x**, Gap: **+5 coins**, Overhead: **250.00%**.

---

## 10. Known Limitations

* Search is bounded by configuration parameters to prevent combinatorial explosion.
* Minimality is provable only when the deterministic ordering is traversed exhaustively (`smallest_verified = true`). Randomized and heuristic modes report `smallest_verified = false`.
* Coin Change is currently the only problem plugged into the search engine (Phase 3 will add Activity Selection and Knapsack).

---

## 11. Files Created / Modified

### Created:
* `engine/include/greedybreaker/search/search_types.hpp`
* `engine/include/greedybreaker/search/search_config.hpp`
* `engine/include/greedybreaker/search/search_result.hpp`
* `engine/include/greedybreaker/search/search_engine.hpp`
* `engine/include/greedybreaker/search/exhaustive_search.hpp`
* `engine/include/greedybreaker/search/backtracking_search.hpp`
* `engine/include/greedybreaker/search/randomized_search.hpp`
* `engine/include/greedybreaker/search/heuristic_search.hpp`
* `engine/src/search/search_config.cpp`
* `engine/src/search/search_engine.cpp`
* `engine/src/search/exhaustive_search.cpp`
* `engine/src/search/backtracking_search.cpp`
* `engine/src/search/randomized_search.cpp`
* `engine/src/search/heuristic_search.cpp`
* `engine/tests/test_search_common.cpp`
* `engine/tests/test_exhaustive_search.cpp`
* `engine/tests/test_backtracking_search.cpp`
* `engine/tests/test_randomized_search.cpp`
* `engine/tests/test_heuristic_search.cpp`
* `docs/PHASE2_REPORT.md`

### Modified:
* `engine/CMakeLists.txt` (added search source files)
* `engine/tests/CMakeLists.txt` (added search test files)
* `engine/src/main.cpp` (upgraded CLI to interactive search engine demo)
* `documentation/ARCHITECTURE.md` (updated architecture with Phase 2)

---

## 12. Non-Implementation Confirmation

Phase 3+ features were **NOT** implemented:
* Activity Selection was not implemented.
* Knapsack was not implemented.
* FastAPI / REST endpoints were not implemented.
* React / Vite / Tailwind / Recharts were not implemented.
* SQLite / persistence was not implemented.

---

## 13. Exact Commands Used

```powershell
# 1. Build
cmake -G "Ninja" -S engine -B engine/build -DCMAKE_BUILD_TYPE=Release
cmake --build engine/build --config Release

# 2. Test
ctest --test-dir engine/build -C Release --output-on-failure

# 3. CLI Demonstration
.\engine\build\greedybreaker_demo.exe exhaustive smallest
.\engine\build\greedybreaker_demo.exe backtracking smallest
.\engine\build\greedybreaker_demo.exe randomized any
.\engine\build\greedybreaker_demo.exe heuristic any
.\engine\build\greedybreaker_demo.exe exhaustive worst 6 15
```
