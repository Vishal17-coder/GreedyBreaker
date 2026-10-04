# Greedy Breaker V2 — Product UX Specification

**Document Version**: 2.0.0  
**Status**: APPROVED DESIGN SPECIFICATION  
**Product Title**: **Greedy Breaker: Automated Counterexample Discovery for Greedy Algorithms**  
**Role**: Foundational UX Specification for Frontend Implementation  

---

## 1. Product Thesis & Experience Vision

### 1.1 Purpose & User Goal
Greedy Breaker is a professional algorithm-research and verification workbench. Its singular purpose is:
> **Configure an experiment, let Greedy Breaker automatically investigate a greedy strategy against exact ground-truth oracles, and understand whether, when, and why it can be broken.**

The interface must feel like an **algorithm experimentation laboratory**, not an AI chatbot, marketing landing page, or decorative KPI dashboard. Users are computer science researchers, algorithm engineers, students, and educators who value:
- **Intellectual Honesty**: Clear boundaries between mathematical proof and empirical non-discovery.
- **Cognitive Clarity**: Data-dense yet calm, distraction-free presentations of inputs, outputs, and decision traces.
- **Scientific Rigor**: Deterministic reproducibility, explicit ordering criteria for minimality, and evidence-grounded forensic deductions.

### 1.2 Experience Pillars
1. **Calm, High-Contrast Light Theme**: A crisp, neutral canvas with deliberate typography and purposeful accent colors. No dark-mode neon glows, no gratuitous gradients, no glassmorphism cards.
2. **Honest Algorithmic Feedback**: Transparent communication of search-space size, candidate limits, and execution progress without fabricated loading percentages.
3. **Evidence-Driven Divergence**: Visualizing the exact decision step where a greedy heuristic diverges from global optimality, accompanied by the concrete mathematical consequence.
4. **Reproducible By Design**: Every experiment is a permanent, immutable artifact that can be reproduced with a single click, yielding verifiable deterministic equivalence.

---

## 2. Core User Journey Map

```text
┌─────────────────────────────────────────────────────────────────────────────────┐
│                                    1. HOME                                      │
│  Understand purpose in seconds: What fails, why greedy fails, how to break it   │
└────────────────────────────────────────┬────────────────────────────────────────┘
                                         │ Click "New Experiment"
                                         ▼
┌─────────────────────────────────────────────────────────────────────────────────┐
│                        2. EXPERIMENT CREATION (GUIDED)                          │
│                                                                                 │
│   Step 1: Choose Problem Family (e.g. Coin Change, 0/1 Knapsack)                │
│   Step 2: Choose Heuristic Strategy (dynamically filtered by problem)           │
│   Step 3: Define Search Space (problem-specific ranges + live size estimator)   │
│   Step 4: Choose Search Engine Mode & Strategy (ANY, SMALLEST, WORST, PROFILE)  │
│   Step 5: Pre-Flight Review (cardinality warning, candidate cap, seed)          │
└────────────────────────────────────────┬────────────────────────────────────────┘
                                         │ Click "Run Experiment"
                                         ▼
┌─────────────────────────────────────────────────────────────────────────────────┐
│                             3. EXPERIMENT PROGRESS                              │
│  Honest state-based progress: Preparing -> Evaluating -> Verifying -> Completed │
└────────────────────────────────────────┬────────────────────────────────────────┘
                                         │ Automatic Transition on Completion
                                         ▼
┌─────────────────────────────────────────────────────────────────────────────────┐
│                               4. RESULT WORKBENCH                               │
│                                                                                 │
│   ┌───────────────────────────────────┐   ┌──────────────────────────────────┐  │
│   │ Case A: COUNTEREXAMPLE DISCOVERED │   │ Case B: NO BREAKS IN BOUNDS      │  │
│   │ - Objective comparison (Gap/Ratio)│   │ - Empirical non-discovery note   │  │
│   │ - Minimal vs Original Instance    │   │ - Search space coverage audit    │  │
│   │ - Side-by-side solution breakdown │   │ - Candidate evaluation count     │  │
│   │ - Step-by-step decision sequence  │   │ - Recommendation to expand bounds│  │
│   │ - Critical divergence isolation   │   └──────────────────────────────────┘  │
│   └───────────────────────────────────┘                                         │
└────────────────────────────────────────┬────────────────────────────────────────┘
                                         │ Action: "Reproduce Experiment"
                                         ▼
┌─────────────────────────────────────────────────────────────────────────────────┐
│                             5. REPRODUCIBILITY AUDIT                            │
│  Executes linked reproduction, confirms deterministic match, preserves history  │
└─────────────────────────────────────────────────────────────────────────────────┘
```

---

## 3. High-Level Screen Catalog

| Screen ID | Screen Name | Route / URL | Primary Purpose |
| :--- | :--- | :--- | :--- |
| `SCR-01` | **Home / Launchpad** | `/` | Immediate orientation, problem cards, primary CTA to launch experiment. |
| `SCR-02` | **Experiment Creator** | `/experiments/new` | Step-by-step guided configuration wizard with real-time pre-flight feedback. |
| `SCR-03` | **Experiment Progress** | `/experiments/:id/running` | Live execution monitoring with honest state indicators and evaluation counters. |
| `SCR-04` | **Counterexample View** | `/experiments/:id` | The flagship result screen: Gap metrics, minimal instance, solution contrast, forensics. |
| `SCR-05` | **No-Counterexample View**| `/experiments/:id` | Scholarly report of empirical non-discovery over bounded space without claiming universal proof. |
| `SCR-06` | **Experiment History** | `/experiments` | Clean, searchable catalog of past experiments with status chips and gap metrics. |
| `SCR-07` | **Problem Explorer** | `/problems` | Reference catalog of 5 problem families, heuristics, and mathematical properties. |

---

## 4. Key UX Conventions & Guardrails

### 4.1 Plain Language over Internal JSON
Users must never be forced to edit raw JSON blobs to configure an experiment. Every search-space parameter is exposed as a typed, labeled input with inline validation and sensible unit indicators (e.g. `Weight Range: [1] to [20] kg`, `Target: [1] to [50]`).

### 4.2 Separation of Empirical Evidence from Proof
The UI must strictly enforce the following textual rules:
- **Discovered Failure**: Labeled as *"Verified Optimality Counterexample"* or *"Verified Feasibility Failure"*.
- **No Failure**: Labeled as *"No counterexample discovered within configured bounds"*.
- **Banned Phrase**: The UI must **never** state *"Algorithm is proven correct"* based on an empirical search.

### 4.3 Minimality Language
Whenever minimized counterexamples are presented:
- The exact terminology must read:
  > **Smallest verified counterexample under the configured ordering.**
- The configured ordering criteria must be visibly disclosed (e.g. `1. Element count; 2. Max parameter; 3. Parameter sum; 4. Lexicographical`).
- The UI must never claim "global unconstrained minimality" unless mathematically proven.

### 4.4 Non-Destructive Reproduction
Clicking **Reproduce Experiment** creates a distinct new experiment record (`exp-repro-...`) with `parent_experiment_id` pointing to the original. The original result is never overwritten. Both results are displayed side-by-side with a green checkmark confirming **Deterministic Match**.
