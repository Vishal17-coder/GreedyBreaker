# Greedy Breaker V2 — Experiment Creation Flow

**Status**: APPROVED DESIGN SPECIFICATION  
**Document Version**: 2.0.0  

---

## 1. Flow Design Principles

1. **Guided, Linear Progression**: 4 clear, focused steps preventing cognitive overload.
2. **Dynamic Problem-Aware Filtering**: Only heuristics, oracles, and parameter bounds valid for the selected problem family are presented.
3. **Live Pre-Flight Feedback**: Real-time cardinality estimation ($\hat{S}$) updates as the user adjusts numeric bounds, warning of impractical search spaces before execution begins.
4. **Zero Raw JSON**: Typed, labeled form inputs with validation tooltips and sensible defaults.
5. **Pre-Run Confidence Review**: A clear summary screen ensuring users know exactly what computational work will be performed before starting.

```text
┌──────────────┐     ┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│    STEP 1    │ ──> │    STEP 2    │ ──> │    STEP 3    │ ──> │    STEP 4    │
│   Problem    │     │ Search Space │     │ Engine Mode  │     │ Review & Run │
│  & Strategy  │     │  Parameters  │     │   & Limits   │     │ (Pre-Flight) │
└──────────────┘     └──────────────┘     └──────────────┘     └──────────────┘
```

---

## 2. Step 1: Problem Family & Greedy Heuristic Selection

### 2.1 Problem Selection Grid
The user is presented with 5 structured problem cards:
- **Coin Change (Fewest Coins)**: Minimization problem. Classic largest-first heuristic.
- **0/1 Knapsack Problem**: Maximization problem with discrete items. Value density heuristic.
- **Activity Selection Problem**: Maximization problem with intervals. Earliest finish time heuristic.
- **Fractional Knapsack Problem**: Continuous relaxation contrast.
- **Optimal Prefix Code (Huffman Coding)**: Tree optimization problem. Shannon-Fano contrast.

Each card displays:
- Title and short mathematical definition.
- Objective badge (`MINIMIZE` or `MAXIMIZE`).
- Breakability indicator (e.g. `Breakable under canonical greedy` vs `Optimal under earliest finish`).

### 2.2 Strategy & Oracle Selectors
Upon selecting a problem, the greedy strategy and exact oracle dropdowns are dynamically populated:
- **Coin Change**:
  - Strategy: `Largest Denomination First` (Default)
  - Ground-Truth Oracle: `Dynamic Programming O(K*T)` (Default) / `BFS Shortest Path`
- **0/1 Knapsack**:
  - Strategy: `Value Density Descending (v_i / w_i)` (Default) / `Max Value First` / `Min Weight First`
  - Ground-Truth Oracle: `2D DP Table O(N*W)` (Default) / `Exhaustive Subset Search O(2^N)`

---

## 3. Step 2: Search-Space Configuration & Live Estimator

### 3.1 Problem-Specific Form Inputs
The form dynamically adapts to the selected problem family:

#### Coin Change Parameters
- **Denomination Count ($K$)**: Min [3] to Max [3] coins (Range slider / number inputs).
- **Denomination Range ($D$)**: Values from [1] to [10] (Base coin 1 is always included).
- **Target Value ($T$)**: Target values from [1] to [20].

#### 0/1 Knapsack Parameters
- **Item Count ($N$)**: [3] to [4] items.
- **Item Weight ($W_i$)**: [1] to [20] kg.
- **Item Value ($V_i$)**: [1] to [50] units.
- **Knapsack Capacity ($C$)**: [10] to [30] kg.

### 3.2 Real-Time Search-Space Estimator Widget
A sticky sidebar widget re-calculates search space cardinality $\hat{S}$ on every keystroke:

```text
┌─────────────────────────────────────────────────────────────┐
│  Search Space Estimation                                    │
│                                                             │
│  Estimated Instances (Ŝ):     360 configurations            │
│  Estimated Evaluation Time:   < 2 milliseconds              │
│  Computational Complexity:    Low                           │
│  Recommended Strategy:        EXHAUSTIVE                    │
│                                                             │
│  ● Manageable for exhaustive evaluation                     │
└─────────────────────────────────────────────────────────────┘
```

### 3.3 Large Search-Space Warning Callout
If the user configures parameters yielding $\hat{S} > 10^7$ instances:

```text
┌─────────────────────────────────────────────────────────────┐
│  ⚠️ High Cardinality Warning                                │
│                                                             │
│  This search space contains ~4.2 × 10⁸ candidate instances. │
│  Exhaustive search is impractical and will hit candidate    │
│  limits early.                                              │
│                                                             │
│  Recommended Action:                                        │
│  Switch Search Strategy to RANDOMIZED or GUIDED search,     │
│  or narrow parameter ranges.                                │
└─────────────────────────────────────────────────────────────┘
```

---

## 4. Step 3: Search Engine Mode & Execution Limits

### 4.1 Search Mode
Cards explaining the objective of the discovery process:
- **`ANY`** (Default): Halts immediately on the very first verified counterexample discovered. Fast and practical.
- **`SMALLEST`**: Continues evaluating candidates to isolate the smallest instance in the configured space.
- **`WORST`**: Maximizes the gap deficit metric (largest percentage optimality loss).
- **`PROFILE`**: Evaluates the full sample budget to compute empirical failure rates and gap distributions.

### 4.2 Search Strategy
- **`EXHAUSTIVE`**: Systematically evaluates every Cartesian combination in lexicographical order.
- **`RANDOMIZED`**: Uniformly samples instances using a deterministic pseudo-random number generator (PRNG).
- **`GUIDED`**: Targets mathematically known vulnerability families (e.g. density trap triads, non-canonical coin pairs).
- **`HYBRID`**: Combines guided vulnerability templates with randomized fuzzing.

### 4.3 Computational Safety Limits
- **Candidate Evaluation Budget**: Default `1,000` instances (Hard system ceiling: `100,000`).
- **Maximum Wall-Clock Duration**: Default `5,000 ms` ($5\text{ s}$) (Hard system ceiling: `30,000 ms`).
- **Random PRNG Seed**: Default `42` (Editable numeric input with `[ Randomize ]` button).

---

## 5. Step 4: Pre-Flight Review & Launch

Before triggering subprocess execution, a clean, high-density summary card is presented:

```text
┌────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│  Review Experiment Configuration                                                                       │
│                                                                                                        │
│  Problem Family:       0/1 Knapsack Problem                                                            │
│  Greedy Strategy:      Value Density Descending (v_i / w_i)                                            │
│  Ground-Truth Oracle:  Dynamic Programming 2D Table O(N*W)                                              │
│  Search Mode:          ANY (Halt on first verified failure)                                            │
│  Search Strategy:      RANDOMIZED (Seed: 12345)                                                        │
│  Search Space Bounds:  Items: [3, 4] • Weights: [1, 20] • Values: [1, 50] • Capacity: [10, 30]        │
│  Estimated Work:       Evaluating up to 500 candidates (Timeout: 5.0 seconds)                          │
│                                                                                                        │
│  [ Edit Configuration ]                                                  [ RUN EXPERIMENT ▶ ]          │
└────────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

Clicking **"Run Experiment"** transitions the UI seamlessly into the live progress view (`/experiments/:id/running`).
