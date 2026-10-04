# Greedy Breaker V2 — Architecture Specification & Decision Record

## 1. System Identity and Architectural Thesis

**Project Title**: Greedy Breaker: Automated Counterexample Discovery for Greedy Algorithms  
**Core Purpose**: Automatically discover, verify, minimize, and analyze counterexamples that cause greedy algorithms to produce non-optimal solutions.

Greedy Breaker is built on the principle that the algorithmic core must be strictly separated from presentation, orchestration, and persistence. The computational truth of the system is deterministic, mathematically rigorous, and independently executable.

---

## 2. Architecture Decision Record (ADR-001)

### 2.1 ADR Summary
| Decision Point | Choice | Rationale |
| :--- | :--- | :--- |
| **Algorithmic Engine** | **C++20** | Zero-cost abstractions, deterministic execution timing, no garbage collection pauses, SIMD/cache-efficient candidate evaluation, strong value semantics, and standard concepts. |
| **Execution Boundary** | **Subprocess Stdio Protocol (JSON)** | Absolute crash resilience, memory safety containment, process-level CPU/timeout controls, no cross-language FFI/JNI ABI fragility, and independent CLI testability. |
| **Backend Supervisor** | **Production REST Service** | Decoupled orchestration layer managing experiment lifecycles, rate limiting, persistence, and async task execution. |
| **Database** | **PostgreSQL (JSONB)** | Relational integrity for experiment metadata, indexed JSONB for flexible instance/trace storage, deterministic queryability. |
| **Search Architecture** | **Streaming Generator + Filter + Pipeline** | Memory footprint $O(1)$ per candidate evaluation; no buffering of unpromising search spaces. |
| **Minimization Strategy** | **Delta-Debugging with Explicit Hierarchy** | Deterministic component reduction under a formal order predicate. |
| **Forensic Model** | **Evidence-Based Divergence Identification** | Identifies the earliest step where greedy diverges from all optimal choice sets; never fabricates causal prose. |

---

### 2.2 Deep Architectural Justifications

#### 1. Why C++20 for the Algorithm Engine
- **Algorithmic Throughput**: Search spaces evaluate up to $10^6$ candidate instances. In C++20, candidate generation and exact DP recurrence evaluations run at bare-metal speed without runtime bytecode overhead.
- **Timing Accuracy**: Measuring algorithmic execution time requires sub-microsecond resolution (`std::chrono::steady_clock`). Managed language runtimes introduce JIT compilation warm-up and garbage collection interrupts that invalidate forensic metrics.
- **Modern Standards**: Modern C++20 provides type safety with Concepts (`std::integral`, `std::floating_point`), `std::span` for zero-allocation views, structured bindings, and `<ranges>`.
- **Zero Python/AI in Core**: Python is barred from the algorithmic core. LLMs must never determine counterexample validity or fabricate causal explanations.

#### 2. Engine / Backend Communication Mechanism
- **Subprocess Isolation**: The C++ engine compiles into a self-contained, standalone executable (`greedybreaker_core`).
- **Communication Protocol**:
  - Input: Well-formed JSON passed via standard input or CLI argument `--config <path>`.
  - Output: Structured JSON payload emitted on standard output.
  - Diagnostics: Telemetry, warnings, and error messages emitted on standard error.
- **Failure Containment**: If a search exceeds bounds or consumes excessive memory, the OS-level process supervisor terminates the process cleanly without corrupting the API server or database connections.

#### 3. Backend Architecture
- The backend serves as an experiment orchestrator:
  - Validates client search boundaries against system safety limits.
  - Spawns and supervises `greedybreaker_core` subprocesses with configurable CPU and memory quotas.
  - Ingests structured JSON output and commits results to PostgreSQL.
  - Exposes RESTful endpoints for experiment submission, polling, status inspection, and reproducible re-execution.

#### 4. Persistence Architecture
- Relational schema in PostgreSQL:
  - `experiments`: Tracks metadata, parameters, search space bounds, seed, status, and summary metrics.
  - `counterexamples`: Stores the minimal failing instances, greedy value, optimal value, gap, and severity ratio.
  - `forensic_traces`: Stores step-by-step greedy decision traces, exact oracle traces, and divergence points.
- Storage efficiency: Raw non-failing candidates during exhaustive search are NOT persisted. Only discovered counterexamples, summary statistics, and audit metadata are stored.

#### 5. Module Boundaries & Decoupling
The C++ engine is strictly decomposed into 8 orthogonal modules:
1. `core/`: Base types, problem interface, solution representation, objective direction, error codes.
2. `problems/`: Concrete problem definitions (Coin Change, 0/1 Knapsack, Activity Selection, Fractional Knapsack, Huffman Coding).
3. `oracle/`: Exact algorithms providing ground truth (Dynamic Programming, Branch and Bound, Linear Relaxation).
4. `search/`: Instance generation, bounding, search-space size estimation, and search strategies (Exhaustive, Randomized, Guided, Hybrid).
5. `verification/`: Feasibility and optimality checking.
6. `minimization/`: Systematic instance reduction maintaining failure.
7. `forensics/`: Decision trace capture, divergence analysis, and metric calculation.
8. `serialization/`: JSON schema validation, input parsing, and result serialization.

---

## 3. High-Level System Architecture Diagram

```text
+-------------------------------------------------------------------------------+
|                             CLIENT / USER INTERFACE                           |
|                    (Designed in Claude Sonnet - Future Phase)                 |
+-------------------------------------------------------------------------------+
                                        |
                                   HTTP / REST
                                        v
+-------------------------------------------------------------------------------+
|                              BACKEND SUPERVISOR                               |
|                  (Experiment Orchestration & API Gateway)                     |
|  - Request Validation & Safety Bounds                                         |
|  - Asynchronous Task Supervisor                                               |
|  - Process Spawner & Timeout Enforcement                                      |
+-------------------------------------------------------------------------------+
              |                                                 |
         JSON Stdio                                       SQL Persistence
              v                                                 v
+-----------------------------------+         +---------------------------------+
|         C++20 ENGINE              |         |           POSTGRESQL            |
|       (Standalone Binary)         |         |  - Experiments                  |
|                                   |         |  - Counterexamples              |
|  +-----------------------------+  |         |  - Forensic Traces              |
|  |       Problem Registry      |  |         |  - Reproducibility Metadata     |
|  +-----------------------------+  |         +---------------------------------+
|  |    Search Space & Estimate  |  |
|  +-----------------------------+  |
|  |     Candidate Generator     |  |
|  +-----------------------------+  |
|  |   Greedy Solver + Tracing   |  |
|  +-----------------------------+  |
|  |      Exact Oracle (DP)      |  |
|  +-----------------------------+  |
|  |     Verifier (Feas./Opt.)   |  |
|  +-----------------------------+  |
|  |   Minimization Pipeline     |  |
|  +-----------------------------+  |
|  |      Forensic Engine        |  |
|  +-----------------------------+  |
|  |     JSON Serialization      |  |
|  +-----------------------------+  |
+-----------------------------------+
```

---

## 4. Algorithmic Pipeline & Lifecycle

```text
                  +-------------------------+
                  |  Experiment Definition  |
                  +-------------------------+
                               |
                               v
                  +-------------------------+
                  | Search Space Estimation |
                  +-------------------------+
                               |
                               v
                  +-------------------------+
                  |  Candidate Generation   |
                  +-------------------------+
                               |
                               v
            +------------------+------------------+
            |                                     |
            v                                     v
+-----------------------+             +-----------------------+
|  Greedy Solver (G)    |             |   Exact Oracle (O)    |
|  - Decision Trace     |             |   - Dynamic Program   |
|  - Objective Value    |             |   - Proven Optimum    |
|  - Feasibility Check  |             |   - Exact Status      |
+-----------------------+             +-----------------------+
            |                                     |
            +------------------+------------------+
                               |
                               v
                  +-------------------------+
                  |  Verification Engine    |
                  |  - Feasibility Failure? |
                  |  - Optimality Failure?  |
                  +-------------------------+
                               |
                   [Counterexample Found?]
                     /                 \
                   YES                 NO
                   /                     \
                  v                       v
      +----------------------+   +----------------------+
      |    Minimization      |   | Next Search Step or  |
      | - Delta-Reduction    |   | Terminate (No Break) |
      | - Preserve Failure   |   +----------------------+
      | - Canonical Order    |
      +----------------------+
                  |
                  v
      +----------------------+
      |   Forensic Analysis  |
      | - Divergence Step    |
      | - Gap & Severity     |
      | - Evidence Report    |
      +----------------------+
                  |
                  v
      +----------------------+
      | JSON Result Payload  |
      +----------------------+
```

---

## 5. Experiment Lifecycle States

An experiment moves through strictly typed states:
1. `CREATED`: Validated configuration received by API.
2. `QUEUED`: Awaiting execution resource slot.
3. `ESTIMATING`: Computing search space size and checking tractability.
4. `RUNNING`: C++ engine executing candidate exploration and verification.
5. `MINIMIZING`: Active counterexample undergoing reduction.
6. `ANALYZING`: Running forensic divergence and trace extraction.
7. `COMPLETED`: Execution succeeded; structured result saved.
8. `EXHAUSTED_NO_COUNTEREXAMPLE`: Search completed over bounded space with zero failures detected.
9. `TIMED_OUT`: Search hit configured hard time limit.
10. `FAILED`: Unrecoverable error (e.g., invalid instance geometry, process crash).

---

## 6. Directory Layout of C++ Core

```text
engine/
├── CMakeLists.txt
├── include/
│   └── greedybreaker/
│       ├── core/
│       │   ├── types.hpp
│       │   ├── objective.hpp
│       │   ├── problem_interface.hpp
│       │   ├── solution.hpp
│       │   ├── verification_result.hpp
│       │   └── error.hpp
│       ├── problems/
│       │   ├── coin_change.hpp
│       │   ├── knapsack_01.hpp
│       │   ├── activity_selection.hpp
│       │   ├── fractional_knapsack.hpp
│       │   └── huffman_coding.hpp
│       ├── oracle/
│       │   ├── oracle_interface.hpp
│       │   ├── dp_coin_change.hpp
│       │   ├── dp_knapsack_01.hpp
│       │   ├── dp_activity_selection.hpp
│       │   ├── continuous_fractional_knapsack.hpp
│       │   └── exact_huffman.hpp
│       ├── search/
│       │   ├── search_space.hpp
│       │   ├── search_mode.hpp
│       │   ├── search_strategy.hpp
│       │   ├── search_engine.hpp
│       │   └── estimators.hpp
│       ├── minimization/
│       │   ├── minimizer_interface.hpp
│       │   ├── canonical_ordering.hpp
│       │   └── delta_minimizer.hpp
│       ├── forensics/
│       │   ├── decision_trace.hpp
│       │   ├── divergence_detector.hpp
│       │   ├── metrics_calculator.hpp
│       │   └── forensic_report.hpp
│       └── serialization/
│           ├── json_parser.hpp
│           └── json_serializer.hpp
├── src/
│   ├── main.cpp (CLI and Stdio driver)
│   ├── core/
│   ├── problems/
│   ├── oracle/
│   ├── search/
│   ├── minimization/
│   ├── forensics/
│   └── serialization/
└── tests/
    ├── CMakeLists.txt
    ├── unit/
    │   ├── test_coin_change.cpp
    │   ├── test_knapsack_01.cpp
    │   ├── test_activity_selection.cpp
    │   ├── test_fractional_knapsack.cpp
    │   ├── test_huffman_coding.cpp
    │   ├── test_minimization.cpp
    │   └── test_forensics.cpp
    ├── integration/
    │   ├── test_discovery_pipeline.cpp
    │   └── test_cross_validation.cpp
    └── fixtures/
        └── canonical_instances.hpp
```

---

## 7. Testing Strategy

1. **Unit Tests**:
   - Verification of exact DP oracles on mathematically solved instances.
   - Verification of greedy solvers and decision trace capture.
   - Feasibility check vs Optimality check distinction.
   - Search space size calculation formulas.
2. **Oracle Cross-Validation**:
   - Small instance cross-checking: Dynamic Programming vs Exhaustive Recursive Branch-and-Bound.
3. **Regression Tests**:
   - Known canonical failures (e.g. Coin Change $\{1,3,4\}, T=6$, Knapsack ratio failure) must be verified on every build.
4. **Negative Invariance Tests**:
   - Activity Selection (Earliest Finish) and Fractional Knapsack (Value/Weight) must produce 0 counterexamples across bounded spaces.
