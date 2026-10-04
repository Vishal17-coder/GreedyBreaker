# Greedy Breaker V2 — Product Specification

## 1. Product Identity

**Official Title**: Greedy Breaker: Automated Counterexample Discovery for Greedy Algorithms  
**System Mission**: Automatically discover, verify, minimize, and analyze counterexamples that cause greedy algorithms to produce non-optimal solutions.

### What Greedy Breaker Is:
- A rigorous, scientific, automated discovery engine for algorithmic counterexamples.
- A forensic analysis platform dissecting why and where greedy heuristics fail.
- A research tool providing mathematically grounded, reproducible failure instances.

### What Greedy Breaker Is NOT:
- NOT a generic algorithm visualization toy.
- NOT a hardcoded demonstration with static slides.
- NOT an AI chatbot or LLM wrapper.
- NOT a dashboard crowded with cosmetic widgets and glowing dark-mode cards.
- NOT a Python-based algorithm sandbox.

---

## 2. The Core Pipeline

The platform processes experiments through 4 strictly segregated architectural phases:

1. **Discovery**:
   - Explores the bounded instance space via defined search strategies.
   - Generates candidate instances meeting all problem validity constraints.
2. **Verification**:
   - Executes the greedy heuristic and the exact mathematical oracle.
   - Categorizes failures into **Feasibility Failure** or **Optimality Failure**.
   - Ensures only certified `EXACT` oracles issue counterexample verdicts.
3. **Minimization**:
   - Reduces discovered counterexamples to minimal failing instances.
   - Applies deterministic reduction operations under a canonical ordering predicate.
4. **Forensics**:
   - Extracts decision traces from both the greedy heuristic and the exact solution.
   - Identifies the earliest divergence point where greedy made an irreversible non-optimal choice.
   - Computes objective gaps, relative deficits, and stranded resource metrics.

---

## 3. Initial Supported Problem Families

| Problem Family | Objective Direction | Default Greedy Heuristic | Exact Oracle Method | Scientific Role |
| :--- | :--- | :--- | :--- | :--- |
| **Coin Change** | `MINIMIZE` (Coins) | Largest Denomination First | Dynamic Programming $O(K \cdot T)$ | Demonstrates simple breakability on non-canonical currency systems. |
| **0/1 Knapsack** | `MAXIMIZE` (Value) | Value/Weight Ratio Descending | 2D Dynamic Programming $O(N \cdot W)$ | Demonstrates breakable optimization heuristic with stranded capacity. |
| **Activity Selection** | `MAXIMIZE` (Count) | Earliest Finish Time First | Dynamic Programming $O(N^2)$ or $O(N \log N)$ | Demonstrates provably optimal greedy behavior (0 breaks discovered). |
| **Fractional Knapsack** | `MAXIMIZE` (Value) | Value/Weight Ratio Descending | Continuous Relaxation Exact Optimum | Contrasts directly with 0/1 Knapsack; proves optimality under continuity. |
| **Huffman Coding** | `MINIMIZE` (Cost) | Two Smallest Frequencies Merge | Exact Optimal Prefix Code | Demonstrates greedy tree/graph construction and alternative heuristic breaks. |

---

## 4. Search System Specification

### 4.1 Search Modes
1. **`ANY`**: Terminates immediately upon discovering the first verified counterexample.
2. **`SMALLEST`**: Explores the candidate space to find the smallest verified counterexample according to canonical problem ordering.
3. **`WORST`**: Searches for the instance maximizing the failure metric (largest absolute or relative gap).
4. **`PROFILE`**: Evaluates the full bounded search space and produces aggregate failure statistics:
   - Evaluated instances count
   - Counterexamples discovered
   - Empirical failure rate
   - Mean objective gap
   - Maximum objective gap
   - Search coverage ratio

### 4.2 Search Strategies
1. **Exhaustive**: Systematic Cartesian generation across all parameters within bounded ranges.
2. **Randomized**: Deterministic pseudo-random exploration initialized from an explicit seed.
3. **Guided**: Failure-driven search focusing generation on regions around observed near-misses or structural traps.
4. **Hybrid**: Phased exploration (e.g. rapid randomized sampling followed by localized exhaustive neighborhood expansion).

### 4.3 Search Space Estimation & Bounds Safety
Before running any experiment, the engine computes:
- $\hat{S}$: Estimated cardinality of the candidate space.
- Estimated oracle operations cost.
- Safety check: If $\hat{S} > S_{\text{max}}$, the engine rejects unbounded exhaustive searches and requires bounded limits or randomized sampling.

---

## 5. Verification & Failure Taxonomy

Every candidate evaluated by the engine is classified into one of three outcomes:
1. **`OPTIMAL`**: Greedy objective equals exact optimal objective ($G = O$).
2. **`FEASIBILITY_FAILURE`**: The greedy strategy failed to construct a valid solution (e.g. Coin Change unable to form target, or exceeding constraint).
3. **`OPTIMALITY_FAILURE`**: The greedy strategy produced a valid, feasible solution, but the objective is strictly worse than the exact optimum:
   - For `MAXIMIZE`: $G < O$
   - For `MINIMIZE`: $G > O$

---

## 6. Minimization Specification

Upon discovering a verified counterexample $C$, the minimization engine executes delta-reduction passes:
- **Order Criteria Hierarchy**:
  1. Primary: Minimum element count ($N$).
  2. Secondary: Minimum maximum parameter value ($\max p$).
  3. Tertiary: Minimum sum of parameters ($\sum p$).
  4. Quaternary: Lexicographical tie-breaking.
- **Invariant**: Every reduction step is verified by re-running the exact oracle. The instance is kept only if the failure condition is preserved.
- **Reporting Guarantee**: Attributed as *"Smallest verified counterexample under the configured ordering"*.

---

## 7. Forensic Analysis Specification

The forensic report contains strictly evidence-backed facts:
- **Instance Parameters**: Exact input specification.
- **Greedy Trace**: Step-by-step decisions, candidate choices, selected element, and heuristic rule applied.
- **Optimal Trace / Representation**: The exact optimal selection set.
- **First Divergence Point**: Decision step $k$ where the greedy choice was no longer compatible with any optimal solution.
- **Causal Evidence**:
  - Coin Change: Stranded target requiring multiple smaller denominations.
  - 0/1 Knapsack: Stranded weight capacity where items could not be packed.
- **Integrity Guarantee**: If no causal evidence can be derived without speculation, the field is marked `UNAVAILABLE`. No LLM-generated fiction.

---

## 8. Reproducibility Guarantee

Every experiment produces a deterministic reproducibility token containing:
- `experiment_id`
- `problem_type`
- `strategy_id`
- `oracle_id`
- `search_space_bounds`
- `search_strategy`
- `random_seed`
- `engine_version`
Re-running the engine with these exact parameters must regenerate identical results deterministically.

---

## 9. Error States

The engine enforces strict, unambiguous error states:
- `NO_COUNTEREXAMPLE_FOUND`: Search completed across specified bounds; zero failures detected.
- `SEARCH_INCOMPLETE`: Hit candidate limit or time limit before exhaustive space was completed.
- `ORACLE_FAILURE`: Oracle exceeded memory/time quotas on oversized instance.
- `INVALID_CONFIGURATION`: Contradictory bounds or invalid parameters.
- `EXECUTION_TIMED_OUT`: Hard wall-clock limit triggered.

---

## 10. Future UI Design Requirements (Handoff to Claude Sonnet)

When the C++ engine and backend API are completed, the UI will be implemented according to these principles:
- **Visual Tone**: Clean, calm, high-contrast, light or neutral palette.
- **Tone**: Academic research instrument / high-end developer developer tool.
- **Visual Bans**: No neon gradients, no dark glow, no giant glassmorphism cards, no decorative irrelevant charts.
- **Essential Visualizations**:
  - Decision step timeline showing where greedy and optimal diverge.
  - Interactive parameter bounding controls.
  - Side-by-side solution comparison (Greedy vs Optimal).
  - Search progress and coverage telemetry.
