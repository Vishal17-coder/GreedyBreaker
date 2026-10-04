# Greedy Breaker V2 — Result & Analysis Experience

**Document Version**: 2.0.0  
**Status**: APPROVED DESIGN SPECIFICATION  
**Scope**: Comprehensive Specification for Result Presentation, Counterexample Inspection, Forensics, Minimization, Negative Evidence, and Reproducibility  

---

## 1. Executive Summary & Design Tenets

The **Result Workbench** (`/experiments/:id`) is the core interface of Greedy Breaker. Its purpose is to deliver immediate, indisputable clarity regarding whether a greedy strategy failed, by how much it failed, what concrete instance triggered the breakdown, and the exact step-by-step causal divergence that led to non-optimality.

### 1.1 The Six Tenets of Result Presentation
1. **Immediate Answer**: Within 3 seconds of viewing the page, the user must know whether a counterexample was found, the objective gap, and the relative difference.
2. **Side-by-Side Concrete Contrast**: Greedy solution vs. Ground-truth optimal solution presented in identical tabular formats to allow direct comparison without mental translation.
3. **Causal Forensic Timeline**: The greedy sequence is mapped step-by-step against optimal counterfactuals, highlighting the exact *Critical Divergence Point* where myopic selection sealed an inferior outcome.
4. **Transparent Minimization**: The reduction from raw generated candidate to minimal counterexample is explicitly demonstrated, with strict adherence to configured ordering criteria.
5. **Scholarly Honesty on Negative Results**: When no counterexample is found within bounds, the system states empirical non-discovery over bounded space—never asserting universal correctness.
6. **One-Click Linked Reproducibility**: Verification is transparent; reproducing an experiment creates an immutable linked execution that validates deterministic matching.

---

## 2. Layout Structure & State Routing

The Experiment Result page dynamically adapts its layout and cards based on the execution status and outcome:

```text
┌────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ BREADCRUMB: Experiments / EXP-2026-0042 (Coin Change)                                                  │
├────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│ PERSISTENT HEADER                                                                                      │
│ [Coin Change] Canonical Largest-First Heuristic                Status: [ OPTIMALITY COUNTEREXAMPLE ]   │
│ Created: Oct 4, 2026 • Duration: 142 ms • PRNG Seed: 42 • Version: 2.0.0      [ Reproduce Experiment ] │
├────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│ TABS:  [ Overview ]     [ Counterexample ]     [ Forensic Divergence ]     [ Evidence & Limits ]       │
├────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                                        │
│   ACTIVE TAB CONTENT AREA                                                                              │
│   (Dynamically renders based on State: Discovered vs No-Break vs Running vs Failed)                    │
│                                                                                                        │
└────────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

---

## 3. Discovered Counterexample Experience (Primary Path)

When an experiment discovers an instance where the greedy strategy produces a suboptimal solution (or infeasible solution), the `Overview` tab renders the following high-impact sections:

### 3.1 Hero Outcome Banner & Metric Quad

```text
┌────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ ⚠ OPTIMALITY COUNTEREXAMPLE VERIFIED                                                                   │
│ The greedy strategy 'Largest First' produced a suboptimal solution verified against Exact DP Oracle.  │
└────────────────────────────────────────────────────────────────────────────────────────────────────────┘

┌──────────────────────┬──────────────────────┬──────────────────────┬──────────────────────┐
│ GREEDY VALUE         │ OPTIMAL VALUE (DP)   │ ABSOLUTE GAP         │ RELATIVE PENALTY     │
│ 3 coins              │ 2 coins              │ +1 coin              │ 50.0% more coins     │
│ (Suboptimal)         │ (Ground Truth)       │ (Greedy - Optimal)   │ (Greedy / Optimal)   │
└──────────────────────┴──────────────────────┴──────────────────────┴──────────────────────┘
```

#### Visual & Copy Rules:
- **Status Indicator**: Neutral slate border with subtle amber/warning chip `OPTIMALITY COUNTEREXAMPLE` (or `FEASIBILITY VIOLATION` if greedy failed to form a valid solution).
- **Metric Cards**: Clean white cards with light border (`#E2E8F0`). Numbers rendered in bold monospace (`JetBrains Mono`). Subtext explicitly states the direction (e.g. `+1 coin` for minimization, `-6 value` for maximization).
- **Relative Penalty Calculation**:
  - Minimization: $\frac{\text{Greedy} - \text{Optimal}}{\text{Optimal}} \times 100\%$
  - Maximization: $\frac{\text{Optimal} - \text{Greedy}}{\text{Optimal}} \times 100\%$

---

## 4. Counterexample View (`/counterexample`)

The Counterexample tab exposes the concrete mathematical instance and presents a side-by-side solution breakdown.

### 4.1 Concrete Input Parameters Card
The instance is printed in human-readable terms with structured badges:

```text
┌────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ MINIMAL COUNTEREXAMPLE INSTANCE                                                                        │
│ Smallest verified counterexample under configured ordering (Element Count ASC, Target ASC).            │
├────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│  Denominations (3):  [ 1 ]  [ 3 ]  [ 4 ]                      Target Value:  6                         │
└────────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

For **0/1 Knapsack**, the card renders an explicit structured item table:
```text
┌────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ KNAPSACK INSTANCE (Capacity W = 50 kg)                                                                 │
├─────────┬──────────────┬──────────────┬────────────────────────┬───────────────────────────────────────┤
│ Item ID │ Weight (w)   │ Value (v)    │ Density (v/w)          │ Properties                            │
├─────────┼──────────────┼──────────────┼────────────────────────┼───────────────────────────────────────┤
│ Item 1  │ 30 kg        │ $120         │ 4.00 $/kg              │ Highest density                       │
│ Item 2  │ 20 kg        │ $70          │ 3.50 $/kg              │ Medium density                        │
│ Item 3  │ 20 kg        │ $70          │ 3.50 $/kg              │ Medium density                        │
└─────────┴──────────────┴──────────────┴────────────────────────┴───────────────────────────────────────┘
```

### 4.2 Side-by-Side Solution Contrast Table

Users see immediately how the two algorithms solved the exact same input:

```text
┌───────────────────────────────────────────┬───────────────────────────────────────────┐
│ GREEDY SOLUTION                           │ GROUND-TRUTH OPTIMAL SOLUTION (ORACLE)    │
│ Strategy: Largest Denomination First      │ Algorithm: Dynamic Programming O(K*T)     │
├───────────────────────────────────────────┼───────────────────────────────────────────┤
│ Selected Elements:                        │ Selected Elements:                        │
│  • 1 × Coin [ 4 ]                         │  • 2 × Coin [ 3 ]                         │
│  • 2 × Coin [ 1 ]                         │                                           │
├───────────────────────────────────────────┼───────────────────────────────────────────┤
│ Total Elements: 3 coins                   │ Total Elements: 2 coins                   │
│ Objective Value: 3                        │ Objective Value: 2                        │
│ Feasibility: Valid (Sum = 6)              │ Feasibility: Valid (Sum = 6)              │
├───────────────────────────────────────────┼───────────────────────────────────────────┤
│ Outcome: SUBOPTIMAL (Gap: +1 coin)        │ Outcome: OPTIMAL GROUND TRUTH             │
└───────────────────────────────────────────┴───────────────────────────────────────────┘
```

#### Copy & Presentation Guardrails:
- The Greedy column is marked with a subtle warning border indicator.
- The Optimal column is marked with a subtle slate/primary indicator.
- Both columns use identical visual rows (`Selected Elements`, `Total Elements`, `Objective Value`, `Feasibility`) so users can scan horizontally without friction.

---

## 5. Forensic Divergence Experience (`/forensics`)

The **Forensic Divergence Report** is the primary scientific differentiator of Greedy Breaker. It answers *why* the greedy choice failed by reconstructing the decision trace and pinpointing the bifurcation step.

### 5.1 Causal Divergence Sequence Wireframe

```text
┌────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ FORENSIC DECISION ANALYSIS                                                                             │
│ Ground-Truth Oracle: Dynamic Programming • Causal Divergence Step: Step 1                              │
├────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                                        │
│  [ STEP 1 ]  CRITICAL DIVERGENCE POINT ⚠                                                               │
│  Greedy Decision:    Selected Coin [ 4 ] (Largest available <= 6, remaining target = 2)                │
│  Optimal Choice:     Selected Coin [ 3 ] (Optimal subproblem branch)                                  │
│  Causal Impact:      Remaining capacity 2 cannot be satisfied with any coin >= 3, forcing greedy into   │
│                      using two 1-coins. Optimal preserves remaining capacity 3, matched by coin [3].    │
│                                                                                                        │
│  ───────────────────────────────────────────────────────────────────────────────────────────────────  │
│                                                                                                        │
│  [ STEP 2 ]                                                                                            │
│  Greedy Decision:    Selected Coin [ 1 ] (Remaining target = 1)                                        │
│  Optimal Choice:     Selected Coin [ 3 ] (Remaining target = 0 -> TERMINATED OPTIMAL)                 │
│                                                                                                        │
│  ───────────────────────────────────────────────────────────────────────────────────────────────────  │
│                                                                                                        │
│  [ STEP 3 ]                                                                                            │
│  Greedy Decision:    Selected Coin [ 1 ] (Remaining target = 0 -> TERMINATED GREEDY)                  │
│  Optimal Choice:     — (Already terminated)                                                           │
│                                                                                                        │
└────────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

### 5.2 Structured Divergence Cards

Each step in the forensic trace contains four explicit fields:
1. **Decision Index**: Zero-indexed or one-indexed chronological step number.
2. **Greedy Choice**: Concrete item/coin selected by the heuristic, along with the local heuristic evaluation criterion (e.g. `Selected Item 1: density = 4.00 $/kg`).
3. **Optimal Action**: The choice made along the optimal trajectory identified by backtracking the exact oracle.
4. **Causal Divergence Explanation**: Plain-language synthesis of the irreversible constraint violation or capacity starvation caused by the greedy choice.

#### Intellectual Honesty Rule:
If the core engine was unable to establish a causal explanation for a complex instance (e.g. in multi-dimensional constraints), the UI displays:
> *"The engine identified empirical divergence at Step 1, but causal derivation is indeterminate under the current oracle heuristic. Divergence is verified by state outcome."*
**Never fabricate an LLM-like hallucinated reason.**

---

## 6. Delta-Reduction Minimization Experience

Greedy Breaker minimizes discovered counterexamples so users do not have to debug 50-item instances when a 3-item instance breaks the algorithm.

### 6.1 Minimization Delta Card

```text
┌────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ COUNTEREXAMPLE MINIMIZATION RECORD                                                                     │
│ Minimization Criterion: Element Count ASC, Parameter Magnitude ASC                                     │
├────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                                        │
│      RAW CANDIDATE                       DELTA-REDUCTION                      MINIMAL INSTANCE         │
│   ┌──────────────────┐               ┌──────────────────────┐               ┌──────────────────┐       │
│   │ 5 Denominations  │      ───>     │ -2 Coins Removed     │      ───>     │ 3 Denominations  │       │
│   │ Target: 18       │  (Minimizer)  │ -12 Target Value     │   (Verified)  │ Target: 6        │       │
│   │ [1, 2, 4, 9, 12] │               │ Verification: PASS   │               │ [1, 3, 4]        │       │
│   └──────────────────┘               └──────────────────────┘               └──────────────────┘       │
│                                                                                                        │
│   ✓ Failure Preserved: Suboptimality confirmed (Greedy = 3 coins, Optimal = 2 coins)                   │
│   ✓ Delta-Reduction Passes: 4 passes in 12 ms                                                          │
│   Strict Scope Note: Smallest verified counterexample under the configured ordering.                   │
└────────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

#### Minimality Disclosure Guardrails:
- The UI must always display the label:
  > **"Smallest verified counterexample under the configured ordering."**
- The four ordering keys must be visibly listed:
  1. Primary: Denomination / Item Count ($N$).
  2. Secondary: Maximum parameter magnitude ($\max D_i$ or $\max W_i$).
  3. Tertiary: Parameter sum ($\sum D_i$).
  4. Quaternary: Target / Capacity value ($T$).
- Tooltip on Minimality: *"Greedy Breaker guarantees minimality with respect to the configured ordering and search space bounds. It does not assert unconstrained global uniqueness."*

---

## 7. No-Counterexample Experience (Empirical Invariance Path)

When an experiment concludes without discovering any counterexample within the configured space, the presentation is governed by strict scientific rigor.

### 7.1 Negative Invariance Screen Wireframe

```text
┌────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ ℹ NO COUNTEREXAMPLE DISCOVERED IN CONFIGURED BOUNDS                                                    │
│ Greedy heuristic 'Earliest Finish Time' produced optimal solutions for all 50,000 evaluated candidates.│
└────────────────────────────────────────────────────────────────────────────────────────────────────────┘

┌──────────────────────┬──────────────────────┬──────────────────────┬──────────────────────┐
│ CANDIDATES TESTED    │ SEARCH COVERAGE      │ ORACLE VERIFICATION  │ EXECUTION TIME       │
│ 50,000 / 50,000      │ 100.0% Exhaustive    │ Interval DP (Exact)  │ 412 ms               │
│ (Complete Space)     │ (Exhaustive Enum)    │ 0 Discrepancies      │ (121.3k tests/sec)   │
└──────────────────────┴──────────────────────┴──────────────────────┴──────────────────────┘

┌────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ SEARCH SPACE & EVIDENCE AUDIT                                                                          │
├────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│ • Search Strategy:         EXHAUSTIVE (Complete combinatorial traversal)                                │
│ • Evaluated Range:         Interval Count N ∈ [2, 8], Coordinates ∈ [1, 50]                           │
│ • Oracle Method:           Dynamic Programming Interval Schedule O(N log N)                           │
│ • Deterministic Seed:      42 (Engine Version: 2.0.0-RELEASE)                                          │
├────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│ SCIENTIFIC INTERPRETATION & SCOPE BOUNDARIES                                                           │
│                                                                                                        │
│  1. Empirical Non-Discovery ≠ Universal Mathematical Proof                                             │
│     This result establishes that no counterexample exists within the bounded space N ≤ 8 and coord ≤ 50.│
│     It does NOT mathematically prove optimality for unbounded N.                                       │
│                                                                                                        │
│  2. Known Theoretical Status (Problem Reference)                                                       │
│     Activity Selection with Earliest Finish Time is theoretically optimal for all inputs via the       │
│     Matroid Greedy Exchange Property (see Problem Documentation).                                      │
├────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│ NEXT EXPLORATORY ACTIONS:                                                                              │
│ [ Expand Search Bounds ]          [ Switch Strategy ]          [ View Problem Proof Details ]          │
└────────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

#### Critical Phrasing Rules:
- **STRICTLY PROHIBITED**: *"Greedy algorithm is proven correct"* or *"No flaws exist in this algorithm"*.
- **MANDATORY TEXT**: *"No counterexample was discovered within the configured search space."*
- **Explicit Separation**:
  - **Evidence**: 50,000 candidates evaluated, 0 discrepancies, 100% coverage of bounded space.
  - **Interpretation**: Non-discovery within finite boundaries. Theoretical context provided as external reference, not generated empirical claim.

---

## 8. Search Evidence & Limits Panel (`/evidence`)

Every experiment includes a dedicated `Evidence & Limits` tab that details the computational mechanics:

```text
┌────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ SEARCH AUDIT & EXECUTION EVIDENCE                                                                      │
├────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│ Metric                              Value                          Interpretation                      │
├─────────────────────────────────────┼──────────────────────────────┼───────────────────────────────────┤
│ Search Strategy                     RANDOMIZED_UNIFORM             Pseudo-random uniform sampling      │
│ Total Search Space Cardinality (|S|) 4,820,000 combinations        Calculated parameter product        │
│ Candidates Generated                10,000                         Limit capped by safety ceiling      │
│ Coverage Ratio                      0.207%                         Fraction of total space tested      │
│ Oracle Comparison Count             10,000 exact evaluations       100% verified against exact DP      │
│ Total Wall Time                     84 ms                          Subprocess latency included         │
│ Memory Peak                         14.2 MB                        Engine resident set size            │
│ Engine Binary Hash                  sha256:7b1e84f...              Authoritative C++ core release      │
└─────────────────────────────────────┴──────────────────────────────┴───────────────────────────────────┘
```

---

## 9. Reproducibility Experience (`/reproducibility`)

Reproducibility is a first-class feature of Greedy Breaker. Every completed experiment provides an auditable reproducibility manifest and a non-destructive reproduction flow.

### 9.1 Reproducibility Manifest Card

```text
┌────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ DETERMINISTIC REPRODUCIBILITY AUDIT                                                                    │
├────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│ Re-running this experiment with the identical configuration and seed will produce the identical        │
│ counterexample and forensic trace.                                                                     │
│                                                                                                        │
│ Config Hash:        0x8f3c4e1a92... (SHA-256 of normalized config)                                     │
│ PRNG Seed:          42                                                                                 │
│ Core Version:       2.0.0-RELEASE (C++20 GCC/MSVC)                                                     │
│ Schema Version:     v2                                                                                 │
│ CLI Reproduction:   ./greedybreaker_cli --config exp-2026-0042.json --seed 42                          │
├────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│ ACTIONS:                                                                                               │
│ [ Reproduce Experiment (New Run) ]     [ Download CLI Input JSON ]     [ Copy Raw Result JSON ]        │
└────────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

### 9.2 Non-Destructive Reproduction Behavior
When the user clicks **Reproduce Experiment**:
1. The backend triggers a brand-new execution (`POST /api/v1/experiments/{id}/reproduce`).
2. A new experiment ID is assigned (e.g. `exp-2026-0043`) with `parent_experiment_id = exp-2026-0042`.
3. The original experiment remains completely untouched and immutable.
4. When the reproduction finishes, the UI displays a green comparison banner:
   ```text
   ✓ REPRODUCTION COMPLETE: Exact Deterministic Match
   Compared against Parent Experiment (exp-2026-0042). Both runs yielded identical minimal instance [1, 3, 4],
   target 6, greedy value 3, optimal value 2, and identical divergence step 1.
   ```

---

## 10. Empty, Error, and Edge State Specifications

A robust research product must handle failure gracefully. Below are the visual and functional specifications for non-standard states:

### 10.1 Experiment Execution Error / Engine Failure
- **Trigger**: C++ core returns non-zero exit code or stderr traceback.
- **UI Presentation**:
  - Status Chip: `EXECUTION FAILED` (Rose/Red accent).
  - Diagnostic Box: Clean, monospaced collapsible container containing sanitized error message and exit status.
  - Action: `[ Edit Configuration ]` and `[ Copy Diagnostic Log ]`.
  - Microcopy: *"The algorithmic engine encountered an unrecoverable failure during candidate verification. No incomplete counterexamples were persisted."*

### 10.2 Timeout Limit Exceeded
- **Trigger**: Execution duration exceeds configured or global timeout (e.g. 30,000 ms).
- **UI Presentation**:
  - Status Chip: `TIMEOUT EXCEEDED` (Amber accent).
  - Metrics Shown: Evaluated candidate count up to timeout moment.
  - Recommendation Card: *"The search space or oracle complexity exceeded the execution budget. Suggestions: 1. Reduce parameter bounds; 2. Switch from Exhaustive to Randomized search; 3. Reduce item count."*
  - Action: `[ Rerun with Constrained Bounds ]`.

### 10.3 Cancelled Experiment
- **Trigger**: User clicked "Cancel" during active execution.
- **UI Presentation**:
  - Status Chip: `CANCELLED BY USER`.
  - Honest disclosure: Evaluated $N$ candidates before cancellation. No conclusions drawn.

### 10.4 Empty History State (`/experiments`)
- **UI Presentation**: Clean, centered slate card with an educational illustration or minimal diagram.
- **Headline**: *"No Experiments Executed Yet"*.
- **Body**: *"Greedy Breaker is ready to test greedy heuristics against exact oracles. Choose a problem family to run your first automated counterexample search."*
- **Primary CTA**: `[ Create Your First Experiment ]` (Deep blue button).
- **Quick-Start Chips**: `[ Coin Change Trap ]`, `[ Knapsack Density Failure ]`.

---

## 11. Component Hierarchy & State Mapping

The table below maps UI components directly to backend Spring Boot DTOs:

| UI Component | Data Source (Backend DTO) | Key Displayed Fields |
| :--- | :--- | :--- |
| **Persistent Header** | `ExperimentDetailDTO` | `id`, `problemType`, `strategyName`, `status`, `createdAt`, `seed` |
| **Hero Metric Quad** | `CounterexampleDTO` | `greedyObjectiveValue`, `optimalObjectiveValue`, `absoluteGap`, `relativeGap` |
| **Input Parameter Card** | `CounterexampleDTO` / `instanceData` | `instanceData` (typed problem instance map: coins, items, intervals) |
| **Solution Contrast Table** | `CounterexampleDTO` | `greedySolution`, `optimalSolution`, `greedyExecutionTimeNs` |
| **Forensic Timeline** | `ForensicReportDTO` | `criticalDivergenceStepIndex`, `divergenceType`, `explanation` |
| **Divergence Step Row** | `DecisionStepDTO` | `stepIndex`, `greedyChoice`, `optimalAlternative`, `causalConsequence` |
| **Minimization Card** | `DeltaReductionDTO` | `originalSize`, `minimizedSize`, `reductionMethod`, `passesCount` |
| **Search Evidence Card** | `ExperimentExecutionDTO` | `candidatesEvaluated`, `totalSearchSpaceSize`, `searchStrategy`, `coverage` |
| **Reproducibility Card** | `ExperimentDetailDTO` | `seed`, `configHash`, `engineVersion`, `parentExperimentId` |
