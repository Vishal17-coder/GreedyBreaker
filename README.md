# Greedy Breaker: Automated Counterexample Discovery for Greedy Algorithms

> **An automated algorithmic forensics platform for discovering, verifying, minimizing, and explaining counterexamples that cause greedy heuristics to produce suboptimal solutions.**

---

## 1. Project Overview

Greedy algorithms make locally optimal choices at each decision step in hopes of finding a global optimum. While greedy methods achieve optimal solutions on problem structures admitting matroid or optimal substructure properties, heuristic adaptations to non-matroid problems frequently produce suboptimal solutions or unfeasible outcomes.

**Greedy Breaker** is an algorithmic research platform that systematically explores parameter spaces, verifies heuristic decisions against ground-truth exact oracles, minimizes discovered counterexamples via delta-reduction, and performs forensic divergence analysis to locate the exact step where greedy strategies fail.

---

## 2. Core Research Question

> **When does a greedy strategy fail, why does it fail, and how can we prove and reproduce that failure?**

Greedy Breaker provides mathematical witnesses and deterministic execution traces rather than theoretical approximations or stochastic heuristics.

---

## 3. Key Capabilities

- **Automated Counterexample Discovery:** Systematic candidate generation across configurable combinatorial parameter spaces.
- **Authoritative Ground-Truth Verification:** Dual-execution against exact oracles (Dynamic Programming, Branch & Bound, Continuous Relaxations, Exact Tree DP).
- **Delta-Debugging Minimization:** Automatic reduction of redundant problem elements while strictly preserving the counterexample witness property.
- **Algorithmic Forensics:** Step-by-step decision tracing isolating the critical step index where the greedy choice diverged from the optimal path.
- **Deterministic Reproducibility:** Every experiment generates an immutable audit record with cryptographic tokens and PRNG seeds for replay verification.

---

## 4. Supported Problem Families

| Problem Family | Greedy Strategies | Authoritative Exact Oracle | Counterexample Discovery |
| :--- | :--- | :--- | :--- |
| **Coin Change** | Largest Denomination First | Dynamic Programming (DP) / BFS | Canonical non-matroid currency systems (e.g., coins `[1, 3, 4]`, target `6`) |
| **0/1 Knapsack** | Value Density ($v_i / w_i$), Max Value, Min Weight | Dynamic Programming 2D Table ($O(NW)$) | Capacity stranding divergence under discrete integer constraints |
| **Fractional Knapsack** | Value Density (Optimal), Min Weight | Continuous LP Relaxation Optimum | Proves Min Weight heuristic fails; certifies Value Density optimality |
| **Activity Selection** | Earliest Finish (Optimal), Shortest Interval | Interval Scheduling DP ($O(N^2)$) | Proves Shortest Interval fails; certifies Earliest Finish optimality |
| **Huffman Coding** | Standard Huffman (Optimal), Shannon-Fano | Exact Min-Cost Prefix Tree (DP) | Proves top-down Shannon-Fano heuristic fails; certifies bottom-up Huffman |

---

## 5. Architecture

Greedy Breaker enforces a strict three-tier decoupled architecture:

```
┌─────────────────────────────────────────────────────────────┐
│  React 19 + TypeScript + Vite UI (Research Workbench)       │
│  - 4-Step Guided Experiment Wizard                          │
│  - Deep Result Workbench: Hero Metrics, Forensics, Audit    │
│  - Deterministic Replay & Parent-Child Experiment Lineage   │
└──────────────────────────────┬──────────────────────────────┘
                               │ JSON REST API (/api/v1)
┌──────────────────────────────▼──────────────────────────────┐
│  Spring Boot 3.3.4 Production Backend                       │
│  - Experiment State Machine (CREATED → RUNNING → COMPLETED) │
│  - C++ Engine Process Supervisor (Pipe draining & timeouts) │
│  - Server-side Bounds Validation & Combinatorial Estimator  │
│  - JPA / H2 Storage for Immutable Experiment Records        │
└──────────────────────────────┬──────────────────────────────┘
                               │ CLI Stdin/Stdout JSON Protocol
┌──────────────────────────────▼──────────────────────────────┐
│  Authoritative C++20 Discovery & Forensics Engine           │
│  - Candidate Generators (Exhaustive, Randomized, Guided)    │
│  - Exact Mathematical Oracles (DP, Exact BFS, Branch&Bound) │
│  - Step-by-Step Decision Trace & Divergence Forensics       │
│  - Delta-Debugging Minimal Counterexample Reducer           │
└─────────────────────────────────────────────────────────────┘
```

---

## 6. Technology Stack

- **Algorithmic Engine:** C++20, CMake 3.20+, Ninja / MSVC / GCC, GoogleTest, `nlohmann/json`.
- **Backend API:** Java 21 (LTS standard), Spring Boot 3.3.4, Spring Data JPA, H2 Database (with PostgreSQL compatibility mode).
- **Frontend Workbench:** React 19, TypeScript 5.8, Vite 8, React Router 7, Vanilla CSS design system.

---

## 7. How Counterexample Discovery Works

1. **Parameter Space Definition:** The user configures bounds (e.g., item count, weight range, value range, capacity).
2. **Candidate Generation:** Candidates are systematically generated using Exhaustive Enumeration, Uniform Randomized Sampling, Guided Boundary Search, or Hybrid Search.
3. **Dual Execution:** Each candidate instance is solved independently by:
   - The greedy heuristic under test.
   - The authoritative exact oracle.
4. **Verification & Certification:** An objective comparator assesses whether the greedy solution is feasible and optimal. If $V_{\text{greedy}} \neq V_{\text{optimal}}$, the instance is certified as an `OPTIMALITY_FAILURE` counterexample.

---

## 8. Exact Oracle Concept

The exact oracle represents mathematical ground truth. Unlike heuristics, exact oracles are guaranteed to compute optimal solutions:
- **0/1 Knapsack:** $O(NW)$ Dynamic Programming table.
- **Coin Change:** $O(KT)$ Dynamic Programming table and Breadth-First Search shortest path.
- **Activity Selection:** $O(N^2)$ Dynamic Programming interval partition.
- **Fractional Knapsack:** Exact continuous linear programming relaxation.
- **Huffman Coding:** Exact priority queue prefix tree and optimal tree DP.

---

## 9. Forensic Analysis

When an optimality failure is discovered, Greedy Breaker executes dual-trace analysis:
1. **Decision Trace:** Captures every greedy choice, active heuristic rule, state before/after, and cumulative objective.
2. **Critical Divergence Step:** Pinpoints the exact decision index where the greedy strategy departed from the optimal trajectory.
3. **Observed Consequence:** Documents the structural state consequence (e.g., capacity stranded, high-value element excluded) without fabricating speculative causal explanations.

---

## 10. Minimization

Discovered counterexamples often contain non-essential elements that obscure the root algorithmic failure. Greedy Breaker applies a delta-debugging minimization algorithm:
- Elements (items, coins, intervals) are sequentially removed or scaled down.
- Each reduced instance is re-verified against the exact oracle.
- If the reduction preserves the optimality failure, it becomes the new baseline.
- **Outcome:** The smallest verified counterexample under the configured ordering criteria (minimal item count, minimal parameters, lexicographical order).

---

## 11. Reproducibility

- Every experiment records an immutable configuration snapshot: problem, greedy strategy, exact oracle, parameter bounds, candidate limit, execution timeout, and PRNG seed.
- Clicking **Reproduce Experiment** generates a new linked experiment record (`parent_experiment_id`) and executes the engine with the original seed.
- The system verifies deterministic reproduction: instances evaluated, counterexample count, and gap metrics must match identically.

---

## 12. Installation & Prerequisites

### Prerequisites
- **C++ Compiler:** C++20 compliant compiler (GCC 11+, Clang 14+, or MSVC 2022+).
- **CMake:** Version 3.20 or newer.
- **Java Development Kit:** JDK 21 (LTS standard).
- **Node.js & npm:** Node.js 18 or newer.

---

## 13. Run Instructions

### 1. Build C++ Engine
```powershell
cd engine
cmake -B build
cmake --build build --config Release
```

### 2. Start Spring Boot Backend
```powershell
cd backend
cmd.exe /c mvnw.cmd spring-boot:run
```
*Backend runs on port `8080` (API base: `http://localhost:8080/api/v1`).*

### 3. Start React Frontend
```powershell
cd frontend
npm install
npm run dev
```
*Frontend runs on port `5173` (`http://localhost:5173`) and proxies `/api` to the backend.*

---

## 14. Testing

### Engine Tests (32 / 32 Passed)
```powershell
cd engine
ctest --test-dir build --output-on-failure
```

### Backend Tests (16 / 16 Passed)
```powershell
cd backend
cmd.exe /c mvnw.cmd test
```

### Frontend Build & Lint (Zero Errors)
```powershell
cd frontend
npm run build
npm run lint
```

---

## 15. Limitations

- **Bounded Empirical Search:** A result of `NO_COUNTEREXAMPLE` indicates that no counterexample exists within the tested parameter bounds and candidate budget. It does not constitute a mathematical proof of correctness for unbounded problem instances.
- **Combinatorial Space Ceilings:** Exhaustive search spaces exceeding $10^8$ candidates require randomized sampling or reduced parameter ranges to prevent execution timeouts.

---

## 16. Project Structure

```text
GreedyBreaker/
├── engine/                       # C++20 Algorithmic Engine
│   ├── include/greedybreaker/    # Header files (problems, search, forensics, minimization)
│   ├── src/                      # Core engine implementations and CLI entrypoint
│   └── tests/                    # GoogleTest test suites (32 unit/integration tests)
├── backend/                      # Spring Boot 3.3.4 Production Backend
│   ├── src/main/java/            # Controllers, Services, Entities, Repositories, DTOs
│   ├── src/test/java/            # Integration & unit test suites (16 tests)
│   └── pom.xml                   # Maven project specification (Java 21 target)
├── frontend/                     # React 19 + TypeScript + Vite Workbench
│   ├── src/pages/                # Home, Experiments, NewExperiment, Detail, Problems, Docs
│   ├── src/components/           # Navigation, StatusBadge, MetricCard, Shared
│   ├── src/api.ts                # Strongly typed REST client
│   └── src/index.css             # Research workbench design system
└── docs/                         # Formal architectural & design specifications
```
