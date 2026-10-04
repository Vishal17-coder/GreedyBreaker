# GreedyBreaker V2 — Product Redesign

## 1. Current Status
- Prototype safely archived on branch `prototype-phase1-3-archive` and under `archive/prototype-phase1-3/`.
- V2 Architecture, Product Specification, and Domain Model established in `docs/`.
- Active engine implementation starting according to approved V2 specification.

## 2. Product Vision
A scientific research and forensic analysis platform dedicated to automated counterexample discovery for greedy algorithms. The system systematically searches bounded problem spaces, evaluates greedy heuristics against certified exact mathematical oracles, isolates minimal counterexamples, and produces evidence-backed forensic divergence reports.

## 3. Core Problem
Greedy algorithms make locally optimal choices at each decision point with the hope of finding a global optimum. While textbooks often prove the correctness of canonical algorithms (e.g. Kruskal's, Dijkstra's, Huffman), greedy heuristics frequently fail on slight problem variations or non-canonical parameter spaces. Finding counterexamples manually is tedious, prone to human bias, and difficult to minimize. Greedy Breaker automates this entire pipeline.

## 4. Target Users
- **Algorithm Researchers**: Investigating novel heuristics and determining exact boundary conditions where greedy heuristics cease to be optimal.
- **Computer Science Educators & Students**: Exploring why canonical greedy strategies fail on specific structures (e.g. non-canonical coin sets, 0/1 knapsack capacity traps).
- **Software Engineers**: Verifying whether a greedy optimization heuristic is safe for production use under bounded inputs.

## 5. Core DAA Contribution
- Formal separation of Discovery, Verification, Minimization, and Forensics.
- Rigorous verification distinguishing Feasibility Failure from Optimality Failure.
- Delta-debugging minimization producing canonical smallest counterexamples under explicit ordering hierarchies.
- Evidence-based forensic trace analysis identifying the first irreversible divergence step without hallucinating causal prose.

## 6. Research Questions
1. Can automated search spaces discover minimal counterexamples faster and more comprehensively than manual construction?
2. What are the earliest structural divergence points between greedy heuristics and exact oracles?
3. How does search strategy (exhaustive vs randomized vs guided) affect discovery rates and counterexample severity?
4. How do provably optimal algorithms (Activity Selection, Fractional Knapsack) perform under bounded search when subjected to the same search engine?

## 7. Counterexample Definition
A verified counterexample is a valid problem instance for which the greedy strategy produces a feasible solution whose objective is strictly worse than the exact optimum:
- For `MAXIMIZE`: $G < O$
- For `MINIMIZE`: $G > O$
Where $G = \text{Greedy Objective}$ and $O = \text{Exact Optimal Objective}$. If the oracle is not certified `EXACT`, no counterexample verdict may be issued.

## 8. Forensic Analysis Model
- Step-by-step decision tracing for both greedy execution and optimal solution reconstruction.
- First meaningful divergence identification: the earliest decision step where the greedy choice is incompatible with all optimal solutions.
- Quantification of objective gap (absolute, relative ratio, overhead/deficit percentage).
- Structural rationale: identifying stranded resources (e.g. stranded knapsack capacity, stranded change remainder).

## 9. Search Methodology
- **Modes**:
  - `ANY`: Halt on first verified failure.
  - `SMALLEST`: Search for smallest counterexample under canonical order.
  - `WORST`: Search for maximal objective gap.
  - `PROFILE`: Full space exploration reporting failure rates, mean gap, and coverage.
- **Strategies**:
  - `Exhaustive`: Complete Cartesian enumeration across bounds.
  - `Randomized`: Deterministic PRNG with stored seed.
  - `Guided`: Structural trap-focused candidate generation.
  - `Hybrid`: Phased randomized sampling with localized exhaustive search.
- **Search Space Estimation**: Mathematical estimation of space cardinality and oracle cost prior to execution.

## 10. Supported Problem Families
1. **Coin Change**: Minimization. Breakable greedy (largest coin first) vs Exact DP.
2. **0/1 Knapsack**: Maximization. Breakable greedy (value/weight ratio) vs Exact 2D DP.
3. **Activity Selection**: Maximization. Optimal greedy (earliest finish) vs Exact DP / alternative heuristics.
4. **Fractional Knapsack**: Maximization. Optimal greedy (value/weight ratio) vs Exact continuous LP / alternative heuristics.
5. **Huffman Coding**: Minimization. Optimal greedy (tree merge) vs Exact prefix code evaluation / alternative heuristics.

## 11. Experimental Methodology
- Deterministic, high-resolution execution timing (`std::chrono::steady_clock`).
- Reproducibility token saved for every run (seed, bounds, strategies, version).
- Cross-validation suite comparing DP against recursive branch-and-bound on small instances.

## 12. Production Architecture
Three-tier architecture:
1. High-Performance C++20 Core Algorithm Engine (`greedybreaker_core`).
2. Production Backend Supervisor (API, validation, process supervision, asynchronous job queue).
3. Relational Storage Layer (PostgreSQL with indexed JSONB).

## 13. Technology Stack

### Algorithmic implementation
The actual algorithms MUST be implemented in:
- C++20
Python must NOT be used for algorithm implementation.

### Frontend
The final frontend is expected to use a modern TypeScript-based web stack.
Do not install or create it yet.

### Backend
The backend will be selected based on the final architecture.
Candidates may include:
- Production REST API supervisor (Python FastAPI / Java Spring Boot / Node TypeScript) with process containment.

### Database
PostgreSQL relational database with JSONB support for instance and trace payloads.

### Deployment
Containerized Docker deployment with isolated worker processes.

## 14. Backend Architecture
Process supervisor executing C++ engine with OS-level quotas (timeouts, memory caps). Exposes REST endpoints for experiment creation, status polling, result retrieval, and reproduction.

## 15. C++/Engine Boundary
Subprocess standard I/O communication over strict JSON protocol. Guarantees complete failure isolation and independent CLI testability.

## 16. Data Model
- `experiments`: id, problem, strategy, oracle, bounds, seed, status, timing.
- `counterexamples`: experiment_id, instance, greedy_val, optimal_val, gap, is_minimal.
- `forensic_traces`: experiment_id, decision_steps, divergence_index, evidence.

## 17. API Design
RESTful endpoints:
- `POST /api/v2/experiments`: Submit and validate experiment.
- `GET /api/v2/experiments/{id}`: Poll status and metrics.
- `GET /api/v2/experiments/{id}/counterexample`: Retrieve minimal counterexample.
- `GET /api/v2/experiments/{id}/forensics`: Retrieve divergence report and traces.
- `POST /api/v2/experiments/{id}/reproduce`: Re-run with identical seed and bounds.

## 18. UX / UI Principles

The final UI MUST NOT look like a stereotypical AI-generated dashboard.

Avoid:
- excessive dark themes
- neon colors
- glowing gradients
- giant glassmorphism cards
- excessive rounded containers
- unnecessary animations
- decorative charts
- excessive visual effects
- overcrowded dashboards

The target visual direction is:
- light or neutral
- professional
- calm
- highly readable
- simple
- distinctive without being flashy
- accessible
- research/product oriented
- intuitive for first-time users
- appropriate for a real deployed product

Graphics should exist only when they communicate algorithmic information.
Examples:
- decision timelines
- greedy vs optimal comparison
- search-space exploration
- counterexample structure
- divergence visualization
- severity comparison

## 19. Deployment Architecture
Docker Compose managing API Gateway, Background Worker pool, and PostgreSQL instance.

## 20. Security Considerations
- Strict boundary validation on input parameter ranges.
- Search space estimation preventing runaway computational attacks.
- Process sandboxing and timeout enforcement on C++ subprocess execution.

## 21. Scalability Considerations
- Memory footprint $O(1)$ per evaluated candidate (streaming evaluation).
- Persistent storage restricted to discovered counterexamples and summary metrics.

## 22. Testing Strategy
- Unit tests: greedy algorithms, exact DP oracles, objective calculations, delta-minimizer.
- Cross-validation tests: DP vs branch-and-bound exhaustive evaluation.
- Regression tests: Canonical counterexample verification.
- Invariance tests: 0 false counterexamples on optimal algorithms.

## 23. Evaluation Criteria
- Absolute mathematical correctness of exact oracles.
- Deterministic reproducibility of counterexamples from stored seeds.
- Verified minimality under explicit canonical ordering.

## 24. Future Extensions
- Dynamic plugin system for user-defined C++ problem implementations.
- Symbolic counterexample synthesis using SMT solvers (Z3).
- Optional AI-assisted plain-language explanation layer (strictly decoupled from mathematical verification).
