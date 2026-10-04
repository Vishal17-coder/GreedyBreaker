# Greedy Breaker V2 — Information Architecture

**Status**: APPROVED DESIGN SPECIFICATION  
**Document Version**: 2.0.0  

---

## 1. Global Navigation Architecture

The global navigation is deliberately minimal, horizontal, and anchored at the top of the viewport. It prioritizes rapid experiment creation and clean orientation without complex multi-tiered menus or sidebars.

```text
┌────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│  [Logo] Greedy Breaker       Home      Experiments      Problems      Docs          [ + New Experiment ]│
└────────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

### 1.1 Navigation Items
| Nav Item | Target Route | Purpose | Keyboard Shortcut |
| :--- | :--- | :--- | :---: |
| **Brand / Logo** | `/` | Returns to launchpad with brief overview and quick-start actions | `G then H` |
| **Home** | `/` | Orientation, quick-start problem cards, system status | — |
| **Experiments** | `/experiments` | Filterable, searchable table of all past runs with status badges | `G then E` |
| **Problems** | `/problems` | Catalog of 5 problem families, strategy breakdowns, and oracles | `G then P` |
| **Docs** | `/docs` | In-app algorithmic references, taxonomy guides, and API specs | `G then D` |
| **CTA Button** | `/experiments/new` | Primary colored action button: creates a new experiment wizard | `C` or `N` |

---

## 2. Route & View Taxonomy

```text
/
├── /                                            # Home / Launchpad
├── /experiments                                 # Experiment Catalog (History)
│   ├── /new                                     # Experiment Creation Wizard
│   └── /:id                                     # Experiment Result Workbench
│       ├── (default: /overview)                 # Result Overview, Gap & Minimality
│       ├── /counterexample                      # Instance breakdown, Greedy vs Optimal
│       ├── /forensics                           # Decision step sequence & divergence
│       ├── /evidence                            # Search coverage, metrics, & oracles
│       └── /reproducibility                     # PRNG seed, token, & reproduction run
├── /problems                                    # Problem Family Explorer
│   └── /:problemId                              # Problem Deep Dive & Strategy Details
└── /docs                                        # Reference Documentation
```

---

## 3. Active Experiment Workbench Sub-Navigation

An individual experiment result is rich in evidence. To prevent overwhelming the user with a monolithic scroll, the Experiment Workbench provides clean tabbed sub-navigation with persistent header metadata.

### 3.1 Persistent Experiment Header
```text
┌────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│  ◄ Back to Experiments                                                                                 │
│                                                                                                        │
│  Coin Change (Fewest Coins) • Canonical Trap Break              [ OPTIMALITY COUNTEREXAMPLE ]          │
│  ID: exp-a1b2c3d4 • Created: Oct 4, 2026, 16:21 UTC • Strategy: Largest First • Seed: 42             │
│                                                                                                        │
│  [ Overview ]   [ Counterexample ]   [ Forensic Divergence ]   [ Evidence & Metrics ]   [ Reproduce ]  │
└────────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

### 3.2 Sub-View Breakdown

#### 1. Overview Tab (`/experiments/:id`)
- **Executive Summary Box**: Large callout displaying failure status (`OPTIMALITY_FAILURE` or `NO_COUNTEREXAMPLE`).
- **Key Metrics Triad**:
  - Heuristic Objective Value vs Exact Optimal Value.
  - Absolute Gap & Relative Deficit Percent ($50.0\%$).
  - Reduction Percent achieved by minimizer ($24.96\%$).
- **Compact Decision Insight**: Single-sentence forensic summary: *"Selecting coin 4 stranded remaining target at 2, requiring 2 additional coins."*

#### 2. Counterexample Tab (`/experiments/:id/counterexample`)
- **Instance Parameters Panel**: Formatted clean table of problem parameters (e.g. Denominations $\{1, 3, 4\}$, Target $6$).
- **Minimization Delta Card**: Comparison of Original Discovered Instance vs Minimized Instance with step count and failure preservation badge.
- **Side-by-Side Solution Breakdown**:
  - Column A: Greedy Solution composition ($4 + 1 + 1 = 3\text{ coins}$).
  - Column B: Exact Optimal Solution composition ($3 + 3 = 2\text{ coins}$).

#### 3. Forensic Divergence Tab (`/experiments/:id/forensics`)
- **Step-by-Step Decision Timeline**: Vertical numbered sequence showing every greedy action, heuristic score, and state transformation.
- **Divergence Anchor**: Visually distinct marker at Step $i$ where the greedy solver committed to an action absent from the optimal set.
- **Counterfactual Action Card**: Contrasts greedy choice against optimal alternative action with causal consequence narrative.

#### 4. Evidence & Metrics Tab (`/experiments/:id/evidence`)
- **Search Engine Execution**: Candidates evaluated, search strategy used, time elapsed.
- **Coverage Meter**: Exhaustive vs Sampled ratio against estimated search-space size $\hat{S}$.
- **Ground-Truth Oracle Certification**: Method used (e.g. 2D DP Table), confidence level (`EXACT`), and states evaluated.

#### 5. Reproducibility Tab (`/experiments/:id/reproducibility`)
- **Reproducibility Token Box**: Formatted JSON snippet containing normalized parameters.
- **One-Click Action**: **"Reproduce Experiment"** button.
- **Reproduction Audit History**: Displays list of reproduction runs linked to this experiment with verification checkmarks.

---

## 4. Breadcrumb Strategy

Breadcrumbs are displayed consistently below the primary navigation bar to ensure spatial orientation:

- Home > **Create Experiment**
- Home > Experiments > **exp-a1b2c3d4 (Coin Change)**
- Home > Problems > **0/1 Knapsack Problem**

Clicking any ancestor segment returns immediately to that view with search filters preserved in URL query parameters.
