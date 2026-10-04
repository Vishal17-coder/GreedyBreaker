# Greedy Breaker V2 — Production Backend Architecture

**Version**: 2.0.0  
**Status**: APPROVED DESIGN  
**Author**: Antigravity Lead Architect  
**Target Runtime**: JVM (Java 21/25 LTS) / Spring Boot 3.4+  

---

## 1. Architectural Thesis & Boundary Principles

Greedy Breaker V2 is architected around a strict separation of concerns:
1. **Authoritative Algorithmic Core (C++20)**: Bare-metal speed, zero-overhead bit manipulation, microsecond-accurate hardware timers, cache-coherent DP recurrences, and deterministic reduction passes.
2. **Backend Orchestrator (Spring Boot 3 / JVM)**: Enterprise-grade REST gateway, asynchronous experiment lifecycle supervision, untrusted input sanitation, persistent state machine management, relational audit history, and subprocess isolation.
3. **Storage Boundary (PostgreSQL / JPA)**: Relational integrity for experiment entities, audit history, counterexample catalogs, and reproducibility metadata.

```text
+----------------------------------------------------------------------------------+
|                                CLIENT LAYER (HTTP/REST)                         |
+----------------------------------------------------------------------------------+
                                        │
                                   HTTP / JSON
                                        ▼
+----------------------------------------------------------------------------------+
|                    GREEDY BREAKER PRODUCTION BACKEND (SPRING BOOT 3)             |
|                                                                                  |
|   ┌──────────────────────────────────────────────────────────────────────────┐   |
|   │                         API Controllers & DTOs                           │   |
|   │   POST /experiments  │  POST /run  │  GET /result  │  POST /reproduce    │   |
|   └────────────────────────────────────┬─────────────────────────────────────┘   |
|                                        │                                         |
|   ┌────────────────────────────────────▼─────────────────────────────────────┐   |
|   │               Pre-Engine Validation Engine (Tier 1 Safety)               │   |
|   │   - Range validation (min <= max)   - Hard candidate & duration caps     │   |
|   │   - Search space size estimation    - Known strategy compatibility checks│   |
|   └────────────────────────────────────┬─────────────────────────────────────┘   |
|                                        │                                         |
|   ┌────────────────────────────────────▼─────────────────────────────────────┐   |
|   │             Experiment Lifecycle & State Machine Orchestrator            │   |
|   │   CREATED ──> VALIDATING ──> VALID ──> QUEUED ──> RUNNING                │   |
|   │        └──> COMPLETED / NO_COUNTEREXAMPLE / TIMED_OUT / FAILED           │   |
|   └──────────────────┬───────────────────────────────────────┬───────────────┘   |
|                      │                                       │                   |
|                      ▼                                       ▼                   |
|   ┌─────────────────────────────────────┐   ┌────────────────────────────────┐   |
|   │    Engine Process Supervisor        │   │    Spring Data JPA Services    │   |
|   │  - java.lang.ProcessBuilder         │   │  - ExperimentRepository        │   |
|   │  - Stdio JSON stream piping         │   │  - CounterexampleRepository    │   |
|   │  - Hard wall-clock timeout watchdog │   │  - ExecutionAuditRepository    │   |
|   │  - Non-zero exit & crash handling   │   │  - Dual PostgreSQL / H2 Engine │   |
|   └──────────────────┬──────────────────┘   └────────────────┬───────────────┘   |
+──────────────────────┼───────────────────────────────────────┼───────────────────+
                       │                                       │
                  JSON Stdio                              JDBC (SQL)
                       ▼                                       ▼
+─────────────────────────────────────────+   +────────────────────────────────────+
|        C++20 ENGINE EXECUTABLE          |   |        POSTGRESQL DATABASE         |
|         (greedybreaker_cli)             |   |  - experiments                     |
|                                         |   |  - experiment_configurations       |
|  - Candidate Generator                  |   |  - experiment_executions           |
|  - Heuristic Greedy Solvers             |   |  - counterexamples                 |
|  - Exact Ground-Truth DP Oracles        |   |  - forensic_reports                |
|  - Delta-Debugging Minimizer            |   |  - reproducibility_tokens          |
|  - Evidence-Based Forensic Analyzer     |   |                                    |
+─────────────────────────────────────────+   +────────────────────────────────────+
```

---

## 2. Technology Selection Evaluation & Justification

### 2.1 Framework Evaluation Matrix

| Criterion | Spring Boot 3 (JVM) | FastAPI (Python) | Node.js / Express | Rust (Actix-web) |
| :--- | :--- | :--- | :--- | :--- |
| **Type Safety & Compiler Guarantees** | **Strong** (Compile-time checked records, strict enums) | Medium (Pydantic runtime checking) | Weak / Opt-in (TypeScript compile-time only) | **Extremely Strong** |
| **Process Control & Isolation** | **Superior** (`ProcessBuilder`, `ProcessHandle`, OS destroy) | Good (`subprocess`, `asyncio.subprocess`) | Moderate (`child_process`, event-loop hazards) | High (`std::process`) |
| **Enterprise Persistence / ORM** | **Industry Benchmark** (Spring Data JPA, Hibernate, Flyway) | Moderate (SQLAlchemy, Alembic) | Moderate (Prisma, TypeORM) | Immature (Diesel, SeaORM) |
| **Concurrency Model** | **High Throughput** (Virtual Threads Project Loom / TaskExecutor) | Event-loop async (GIL constraints) | Single-threaded event loop | Thread pool / Actor |
| **Domain Validation** | **Declarative & Standard** (Jakarta Bean Validation 3.0) | Declarative (Pydantic) | Manual / Library-dependent (Zod, Joi) | Strong (Validator crate) |
| **Production Observability** | **Unmatched** (Spring Boot Actuator, Micrometer, Prometheus) | Community-driven packages | Community-driven packages | Library-dependent |

### 2.2 Framework Decision
**Selected Framework: Spring Boot 3.4+ on Java 21/25 LTS.**

**Justification**:
1. **Total Algorithmic Decoupling**: The algorithmic engine is written entirely in C++20. By selecting Spring Boot, there is zero temptation or risk of "leaking" greedy algorithms or approximations into the backend layer.
2. **Subprocess Supervision Resilience**: Java's `ProcessBuilder` API combined with modern `ProcessHandle` provides rock-solid OS-level process management: standard input piping without shell wrapping, detached process killing (`destroyForcibly()`), and precise wall-clock timeouts.
3. **Data Integrity & Relational Architecture**: Counterexamples, forensic divergence steps, and experiment configurations form deeply structured relational graphs. Spring Data JPA with PostgreSQL handles foreign keys, indexes, and JSONB persistence with transaction safety (`@Transactional`).
4. **Safety Against Shell Injection**: Java invokes processes via an array of string arguments (`String[]`), communicating directly with the operating system kernel (`CreateProcessW` on Windows, `execve` on Linux). It completely bypasses command shell interpreters (`cmd.exe`, `powershell.exe`, `/bin/sh`), eliminating command injection vulnerabilities by design.

---

## 3. C++ Engine Integration & Process Lifecycle

### 3.1 Invocation Protocol
Communication between the Spring Boot backend and the C++ engine occurs strictly over **JSON Standard I/O (stdio)**.

1. **No Temporary Files**: The backend writes the complete `SearchConfig` JSON directly into the spawned process's `stdin` and immediately closes the write channel (`outputStream.close()`), signaling EOF.
2. **Non-Blocking Output Buffering**: The backend reads `stdout` and `stderr` asynchronously using separate consumer threads or futures to avoid OS pipe deadlock.
3. **Timeout Watchdog**: The process execution is bounded by `process.waitFor(timeoutMs, TimeUnit.MILLISECONDS)`. If the timeout expires, the supervisor invokes `process.destroyForcibly()`, reclaims resources, and logs a `TIMED_OUT` event.
4. **Structured JSON Parsing**: Upon exit code `0`, `stdout` is parsed by Jackson into `EngineOutputDTO`. If non-zero, `stderr` is parsed into structured error details.

### 3.2 Error Containment Architecture

```text
Engine Event                  Backend Reaction                          HTTP Response
─────────────────────────────────────────────────────────────────────────────────────────────
Exit Code 0, CE Found         Parse payload, persist CE & Forensics     HTTP 200 OK (COMPLETED)
Exit Code 0, Zero CE          Parse payload, persist zero metrics       HTTP 200 OK (NO_COUNTEREXAMPLE)
Exit Code 0, Status Code 1    Persist empirical non-discovery           HTTP 200 OK (NO_COUNTEREXAMPLE)
Exit Code 0, Status Code 2    Candidate cap reached before exhaustion   HTTP 200 OK (SEARCH_INCOMPLETE)
Exit Code 0, Status Code 4    Invalid search bounds caught by engine    HTTP 400 Bad Request
Process Timeout Triggered     Process killed via destroyForcibly()      HTTP 200 OK (TIMED_OUT)
Exit Code Non-Zero (Crash)    Read stderr, capture signal               HTTP 500 Engine Failure
Malformed JSON Output         JSON parse exception caught               HTTP 502 Bad Gateway
```

---

## 4. Execution Safety & Input Sanitization (Untrusted Input Policy)

Every experiment request submitted to the REST API is treated as untrusted. The backend enforces two tiers of validation:

### 4.1 Tier 1: Pre-Engine Validation (Spring Boot)
Executed before the C++ engine is even invoked:
1. **Range Feasibility**: Validates that all ranges satisfy $\min \le \max$, all values are positive, and values stay within bounds ($N \le 50$, $W \le 100,000$, Target $\le 100,000$).
2. **Cardinality Hard Ceiling**: Computes an upper bound on search space cardinality $\hat{S}$. If $\hat{S} > 10^{12}$, rejects the request unless search strategy is set to `RANDOMIZED` or `GUIDED`.
3. **Candidate Cap**: Enforces an absolute maximum candidate evaluation cap of $100,000$ instances per single experiment run.
4. **Timeout Cap**: Enforces a maximum wall-clock runtime of $30,000\text{ ms}$ ($30\text{ seconds}$).
5. **Path Whitelist**: The C++ engine binary path is strictly configured via backend properties (`greedybreaker.engine.binary-path`) and can never be overridden by client requests.

### 4.2 Tier 2: Algorithmic Core Validation (C++ Engine)
Executed by `ProblemInterface::is_valid_instance()`:
- Strict denomination sort and deduplication.
- Capacity positivity checks.
- Activity start and finish time validity ($s_i < f_i$).
- Frequency positivity checks.

---

## 5. Persistence Architecture

### 5.1 Relational Boundary (Spring Data JPA & PostgreSQL)
- **`experiments`**: Experiment metadata, problem type, strategy, author, tags, current status, creation/execution timestamps.
- **`experiment_configurations`**: Immutable parameters, search space bounds, strategy IDs, oracle IDs, random seed, candidate limits, timeout settings.
- **`experiment_executions`**: Concrete execution runs, evaluated candidate counts, execution time ms, failure rates, coverage mode, coverage ratio.
- **`counterexamples`**: Minimal counterexamples discovered, original instance JSON, minimal instance JSON, greedy value, optimal value, absolute gap, relative gap, failure percent, minimality status.
- **`forensic_reports`**: Divergence step index, critical greedy action, optimal alternative action, causal consequence, evidence confidence rating.
- **`reproducibility_tokens`**: Complete normalized reproduction tokens ensuring identical recreation.

### 5.2 Standalone Testing & Dual Profile
- **Development & Testing Profile (`test`, `local`)**: Employs an embedded in-memory H2 database with standard PostgreSQL dialect compatibility mode (`MODE=PostgreSQL`). Enables 100% self-contained Maven test suites without external database prerequisites.
- **Production Profile (`prod`)**: Connects to external PostgreSQL 15+ with indexed JSONB columns and connection pooling via HikariCP.

---

## 6. Reproducibility Model

Reproducibility is a first-class feature of Greedy Breaker V2.
- When an experiment is executed, the backend stores the complete `reproducibility_token` emitted by the C++ engine.
- A client can request reproduction via `POST /api/v1/experiments/{id}/reproduce`.
- The system generates a **new reproduction experiment record** linked back to the original via `parent_experiment_id`, retaining the identical seed and search boundaries.
- The original experiment result is never overwritten. Both results are preserved side-by-side to allow audit comparison.

---

## 7. Future Deployment Model

1. **Containerized Architecture**: The Spring Boot backend package (`greedybreaker-backend.jar`) and the compiled C++ binary (`greedybreaker_cli`) are packaged inside a lightweight container image (e.g. Ubuntu LTS base with OpenJDK 21/25).
2. **Process Sandboxing**: The C++ engine runs as a non-root child process with restricted system capabilities (`setrlimit` CPU time, maximum virtual memory address space).
3. **Database**: PostgreSQL hosted on managed cloud instances or local container.
