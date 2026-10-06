# Greedy Breaker V2 — Final Release Report

---

## 1. Product Status

**Greedy Breaker V2: Automated Counterexample Discovery for Greedy Algorithms** is a production-ready academic research and algorithmic engineering platform. The platform automates the discovery, exact verification, delta-debugging minimization, decision-trace forensic analysis, and deterministic reproduction of counterexamples that break greedy heuristics.

---

## 2. Architecture

The system operates across three decoupled, authoritative tiers:

```
┌─────────────────────────────────────────────────────────────┐
│  React 19 + TypeScript + Vite UI (Research Workbench)       │
│  - 4-Step Guided Experiment Wizard                          │
│  - Hero Metric Quad, Deep Forensics, Minimization Inspector │
│  - Parent-Child Experiment Reproduction Lineage             │
└──────────────────────────────┬──────────────────────────────┘
                               │ JSON REST API (/api/v1)
┌──────────────────────────────▼──────────────────────────────┐
│  Spring Boot 3.3.4 Production Backend                       │
│  - State Machine Supervisor (CREATED → RUNNING → COMPLETED) │
│  - C++ Subprocess Supervisor (Pipes, timeouts, exit codes)  │
│  - Server-Side Range Validation & Combinatorial Estimator   │
│  - JPA / H2 Persistence with Immutable Records              │
└──────────────────────────────┬──────────────────────────────┘
                               │ CLI Stdin/Stdout JSON Protocol
┌──────────────────────────────▼──────────────────────────────┐
│  Authoritative C++20 Algorithmic Engine                     │
│  - Exact Oracles (DP Tables, BFS, LP Relaxation, Tree DP)   │
│  - Candidate Generators (Exhaustive, Randomized, Guided)    │
│  - Step-by-Step Decision Trace & Divergence Forensics       │
│  - Delta-Debugging Minimal Counterexample Reducer           │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. Automated Test Suite Validation

### 🔬 C++20 Engine
- **Command:** `ctest --test-dir engine/build --output-on-failure`
- **Results:** **32 / 32 Passed (100%)**
- **Test Suites:**
  - `ForensicsTest` (Coin change & Knapsack divergence identification)
  - `MinimizationTest` (Delta-reduction of non-essential elements)
  - `SearchEngineTest` (Exhaustive, randomized, profile modes, seed reproducibility)
  - `CoinChangeTest` (Canonical failure, optimal verification, BFS/DP cross-validation)
  - `Knapsack01Test` (Density failure, optimal verification, DP cross-validation)
  - `FractionalKnapsackTest` (Density optimality certification, min-weight failure)
  - `ActivitySelectionTest` (Earliest finish optimality certification, shortest interval failure)
  - `HuffmanCodingTest` (Huffman optimality certification, Shannon-Fano failure)
  - `SerializationTest` & `VerificationResultTest` (Roundtrips, status mapping)

### ☕ Spring Boot Backend
- **Command:** `cmd.exe /c mvnw.cmd test`
- **Results:** **16 / 16 Passed (100%)**
- **Test Suites:**
  - `ExperimentEndToEndIntegrationTest`: 6 end-to-end integration tests with the real C++ engine
  - `ExperimentValidatorTest`: 4 tests for range bounds and candidate limits
  - `ErrorMappingTest`: 3 tests for exception translation
  - `ExperimentStatusTransitionTest`: 2 tests for state machine integrity
  - `ResultMappingTest`: 1 test for DTO conversion

### ⚡ React Frontend
- **Command:** `npm run build`
  - TypeScript Typecheck: **0 errors**
  - Vite Production Bundle: **Built in 369ms (349 kB bundle)**
- **Command:** `npm run lint` (`oxlint`): **0 errors**

---

## 4. End-to-End Workflow Validation

The complete pipeline was executed live against the running application:

$$\text{Create} \longrightarrow \text{Configure} \longrightarrow \text{Review} \longrightarrow \text{Run} \longrightarrow \text{Search} \longrightarrow \text{Exact Oracle} \longrightarrow \text{Counterexample} \longrightarrow \text{Minimization} \longrightarrow \text{Forensics} \longrightarrow \text{Reproduction}$$

### A. Live Verified Counterexample (0/1 Knapsack)
- **Experiment ID:** `exp-15c0fe3d`
- **Problem:** `knapsack_01` (0/1 Knapsack Problem)
- **Greedy Strategy:** `value_density_descending` ($v_i / w_i$)
- **Ground-Truth Oracle:** `dp_exact` (Dynamic Programming 2D Table)
- **Candidate Search Mode:** `ANY` / `RANDOMIZED` (Seed: `42`)
- **Evaluated Candidates:** 6 instances evaluated before discovery
- **Discovered Results:**
  - **Status:** `COMPLETED` (`has_counterexample: true`)
  - **Verification Status:** `OPTIMALITY_FAILURE`
  - **Greedy Value:** `64.0` (Suboptimal)
  - **Optimal Value:** `67.0` (Ground Truth)
  - **Absolute Gap:** `3.0`
  - **Suboptimality Penalty:** `4.48%`
- **Delta-Debugging Minimization:**
  - Original candidate: 4 items (Total weight: 34, capacity: 16)
  - Minimized witness: **2 items** (Capacity: 8, Items: `[{v: 37, w: 8}, {v: 27, w: 5}]`)
  - Reduction passes: 7 passes (**50% reduction in problem size**)
  - Minimality criterion: *Smallest verified counterexample under the configured ordering*
- **Forensic Investigation:**
  - Critical divergence: Step 0 (`Selected Item 3 (v=27.0, w=5.0)`)
  - Optimal alternative: Optimal solution excludes Item 3 to preserve capacity for higher total value set.
  - Observed consequence: Inclusion of Item 3 stranded capacity, preventing inclusion of the optimal items.
  - Evidence confidence: `HIGH_CONFIDENCE_EVIDENCE`
  - Greedy trace: 4 full execution steps with heuristic rules, capacity before/after, and cumulative objective.

### B. Live No-Counterexample Validation (Activity Selection)
- **Experiment ID:** `exp-483e1c7d`
- **Problem:** `activity_selection` (Activity Selection Problem)
- **Greedy Strategy:** `earliest_finish_time`
- **Ground-Truth Oracle:** `dp_exact`
- **Candidate Search Mode:** `PROFILE` / `RANDOMIZED` (Seed: `777`)
- **Evaluated Candidates:** 100 instances evaluated
- **Counterexamples Found:** 0
- **Status:** `NO_COUNTEREXAMPLE`
- **Presentation:** UI displays `"No Counterexample Discovered Within Configured Bounds"` with explicit boundary notice (empirical bounded result, not universal proof). Counterexample and Forensics tabs render clean empty-state summaries without hanging.

### C. Live Deterministic Reproduction
- **Original Experiment:** `exp-15c0fe3d`
- **Reproduction Action:** `POST /api/v1/experiments/exp-15c0fe3d/reproduce`
- **Reproduction ID:** `exp-repro-e290e416` (`parent_experiment_id: exp-15c0fe3d`)
- **Deterministic Match:** `is_deterministic_match: true`
- **Integrity:** Original experiment record is immutable and preserved; child record reproduces identical metrics.

### D. Input Validation & Security Safeguards
- Client-side numeric validation in 4-step wizard.
- Server-side range enforcement (`min <= max`, positive bounds, system ceiling checks).
- Out-of-bounds configurations rejected with HTTP 422 Unprocessable Entity.
- Subprocess timeout and pipe-draining supervisor protects system against infinite loops.

---

## 5. Known Limitations

1. **Empirical Bound vs. Mathematical Proof:** Empirical search is inherently bounded by the configured parameter ranges and candidate budget. A `NO_COUNTEREXAMPLE` outcome proves correctness within the tested subset, but mathematical proof for unbounded spaces requires formal matroid theory.
2. **Combinatorial Growth:** Spaces exceeding $10^8$ candidates require randomized sampling or tight parameter bounds to prevent execution timeout.

---

## 6. Final Status

**FINAL STATUS: READY FOR DEMONSTRATION AND SUBMISSION**
