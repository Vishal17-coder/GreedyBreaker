# Greedy Breaker V2 — Experiment Lifecycle Specification

**Status**: APPROVED DESIGN  
**Version**: 2.0.0  

---

## 1. State Machine Definition

An experiment in Greedy Breaker moves through a deterministic, strictly defined state machine. State changes are persistent and audited with timestamps.

```text
       ┌───────────┐
       │  CREATED  │
       └─────┬─────┘
             │ validate()
             ▼
      ┌──────────────┐
      │  VALIDATING  │
      └──────┬───────┘
             ├──────────────────────────┐
      [Valid]│                          │[Invalid]
             ▼                          ▼
        ┌─────────┐               ┌───────────┐
        │  VALID  │               │  INVALID  │ (Terminal)
        └────┬────┘               └───────────┘
             │ run()
             ▼
        ┌─────────┐
        │ QUEUED  │
        └────┬────┘
             │ worker picks up
             ▼
        ┌─────────┐
   ┌───►│ RUNNING │
   │    └────┬────┘
   │         │
   │         ├───────────────────────┬────────────────────────┬──────────────────────┬─────────────────────────┐
   │         │ [Found CE]            │ [Zero CE in bounds]    │ [Candidate Cap Hit]  │ [Timeout Exceeded]      │ [Fatal Error]
   │         ▼                       ▼                        ▼                      ▼                         ▼
   │  ┌─────────────┐     ┌───────────────────┐     ┌───────────────────┐      ┌───────────┐              ┌───────────┐
   │  │  COMPLETED  │     │ NO_COUNTEREXAMPLE │     │ SEARCH_INCOMPLETE │      │ TIMED_OUT │              │  FAILED   │
   │  └─────────────┘     └───────────────────┘     └───────────────────┘      └───────────┘              └───────────┘
   │    (Terminal)             (Terminal)                (Terminal)              (Terminal)                 (Terminal)
   │
   └────── reproduce() generates a NEW Experiment in CREATED state
```

---

## 2. State Catalog & Descriptions

| State Enum | Terminal? | HTTP Code on Run | Description |
| :--- | :---: | :---: | :--- |
| `CREATED` | No | — | Experiment entity created with draft configuration; validation not yet performed. |
| `VALIDATING` | No | — | Structural and domain constraint validation is actively executing. |
| `VALID` | No | — | Configuration successfully passed all pre-engine safety and domain constraints. |
| `INVALID` | **Yes** | 422 Unprocessable | Configuration violated structural constraints (e.g., $min > max$, candidate limit exceeded). |
| `QUEUED` | No | 202 Accepted | Experiment accepted and waiting in the execution worker queue. |
| `RUNNING` | No | — | C++ engine subprocess actively running candidate evaluation and oracle verification. |
| `COMPLETED` | **Yes** | 200 OK | Process succeeded; one or more verified counterexamples were discovered and minimized. |
| `NO_COUNTEREXAMPLE` | **Yes** | 200 OK | Process succeeded; zero counterexamples discovered across the configured search space. |
| `SEARCH_INCOMPLETE` | **Yes** | 200 OK | Process halted cleanly because candidate cap was reached before the space was exhausted. |
| `FAILED` | **Yes** | 500 / 502 | Engine crashed, exited with non-zero status code, or emitted malformed output. |
| `TIMED_OUT` | **Yes** | 200 OK (Timed out) | Wall-clock limit exceeded; subprocess was forcefully terminated by supervisor. |
| `CANCELLED` | **Yes** | 200 OK | Operator or client cancelled the execution while QUEUED or RUNNING. |

---

## 3. State Transition Matrix

| Current State | Target State | Trigger / Condition | Permitted? |
| :--- | :--- | :--- | :---: |
| `CREATED` | `VALIDATING` | `POST /experiments/{id}/validate` or automatic run trigger | **YES** |
| `VALIDATING` | `VALID` | All pre-engine constraint checks pass | **YES** |
| `VALIDATING` | `INVALID` | Any constraint check fails | **YES** |
| `INVALID` | `VALIDATING` | Update configuration & re-validate | **YES** |
| `VALID` | `QUEUED` | `POST /experiments/{id}/run?async=true` | **YES** |
| `VALID` | `RUNNING` | `POST /experiments/{id}/run?async=false` | **YES** |
| `QUEUED` | `RUNNING` | Worker process starts engine subprocess | **YES** |
| `QUEUED` | `CANCELLED` | Client aborts before execution starts | **YES** |
| `RUNNING` | `COMPLETED` | Engine exit code 0, counterexample found & verified | **YES** |
| `RUNNING` | `NO_COUNTEREXAMPLE`| Engine exit code 0, zero counterexamples discovered | **YES** |
| `RUNNING` | `SEARCH_INCOMPLETE`| Engine exit code 0, candidate limit hit before space exhausted | **YES** |
| `RUNNING` | `TIMED_OUT` | Wall-clock execution exceeded configured timeout | **YES** |
| `RUNNING` | `FAILED` | Process crashed (SIGSEGV/abort) or exit code non-zero | **YES** |
| `RUNNING` | `CANCELLED` | Client cancels active run (`destroyForcibly()`) | **YES** |
| *Terminal State* | *Any State* | Attempting to mutate finished execution | **NO** (Must create reproduction) |

---

## 4. Failure Distinctions (No Collapse to HTTP 500)

A critical principle of Greedy Breaker V2 is that algorithmic non-discovery, user configuration errors, and platform errors must never collapse into generic HTTP 500 responses:

1. **User Range Error**: Min target is $20$, max target is $5$. Handled at the validation layer $\to$ State `INVALID`, HTTP **422 Unprocessable Entity**.
2. **Empirical Non-Discovery**: Earliest Finish Time tested over 300 instances; zero breaks found. This is a successful experiment demonstrating heuristic resilience $\to$ State `NO_COUNTEREXAMPLE`, HTTP **200 OK**.
3. **Budget Exhaustion**: Search space has $1,000,000$ instances, but candidate cap was set to $500$ instances. The engine evaluated $500$ instances and stopped $\to$ State `SEARCH_INCOMPLETE`, HTTP **200 OK**.
4. **Hard Timeout**: Subprocess ran past $5,000\text{ ms}$; supervisor killed it cleanly $\to$ State `TIMED_OUT`, HTTP **200 OK** (with partial evaluation counts).
5. **Platform Failure**: Missing binary or corrupted environment $\to$ State `FAILED`, HTTP **500 Internal Server Error**.
