# Greedy Breaker V2 — UI Design Principles & System Specification

**Document Version**: 2.0.0  
**Status**: APPROVED DESIGN SPECIFICATION  
**Target Implementer**: Frontend Implementation Engineer (Claude Sonnet)  
**Scope**: Visual Language, Design System Tokens, Typography, Layout & Grid, Component Styling Rules, Accessibility Standards, and Responsive Strategies  

---

## 1. Visual Direction & Aesthetic Philosophy

Greedy Breaker is an **algorithm experimentation and verification workbench**. The user interface must project scientific authority, intellectual honesty, and calm focus. 

### 1.1 The Six Visual Directives
1. **Light, Neutral, and Calm**: The interface uses a clean, bright, distraction-free canvas. It avoids eye fatigue during extended algorithm analysis sessions.
2. **Distinctive Without Flashiness**: Visual character is established through crisp geometric borders, disciplined monospace data alignment, and deliberate typography—not glowing borders or flashy graphics.
3. **Restrained Color Economy**: Colors are semantic tools for comprehension, not decorative wallpaper. A single controlled accent color grounds primary actions, while subtle, muted status colors denote algorithmic outcomes.
4. **Data-Dense Yet Breathable**: Algorithmic matrices, interval sets, and decision sequences are formatted with strict columnar discipline and comfortable whitespace.
5. **Meaningful Visualizations Only**: Visual diagrams are deployed solely when they communicate mathematical or structural evidence (e.g. interval overlaps, decision bifurcation, capacity packing).
6. **Strict Anti-Cliché Stance**:
   - ❌ NO dark-mode neon glows
   - ❌ NO heavy purple/cyan gradients
   - ❌ NO frosted glassmorphism overlays
   - ❌ NO AI-chatbot chat bubbles or sparkles
   - ❌ NO giant corporate KPI walls
   - ❌ NO arbitrary spinning dials or fake loading percentages

---

## 2. Color Palette & Semantic Tokens

All colors are defined using high-legibility HSL/Hex coordinates optimized for WCAG 2.1 AA contrast compliance (minimum 4.5:1 for body text, 3:1 for large headings and UI controls).

### 2.1 Neutral Base Palette (Canvas & Structure)
The neutral foundation is built on slate tones that provide warmth and depth without stark, harsh black-and-white contrasts.

| Token Name | Hex Code | Purpose / Usage |
| :--- | :--- | :--- |
| `--color-canvas-bg` | `#F8FAFC` (Slate 50) | Primary page background. Soft, off-white, reduces glare. |
| `--color-surface-bg` | `#FFFFFF` (White) | Cards, panels, modals, and input fields. |
| `--color-surface-subtle` | `#F1F5F9` (Slate 100) | Table headers, secondary toolbars, disabled regions. |
| `--color-border-subtle` | `#E2E8F0` (Slate 200) | Card borders, table dividers, tab underlines. |
| `--color-border-strong` | `#CBD5E1` (Slate 300) | Form input borders, hover states on cards. |
| `--color-text-primary` | `#0F172A` (Slate 900) | Primary headings, table text, numbers. High contrast (14:1). |
| `--color-text-secondary`| `#475569` (Slate 600) | Explanatory subtext, labels, captions, metadata. |
| `--color-text-tertiary` | `#94A3B8` (Slate 400) | Placeholder text, disabled labels, breadcrumb dividers. |

### 2.2 Primary Brand & Interactive Accent
A single, dignified deep slate-blue provides focus for primary actions and active navigation items:

| Token Name | Hex Code | Purpose / Usage |
| :--- | :--- | :--- |
| `--color-primary-base` | `#1E40AF` (Blue 800) | Primary CTA buttons, active tab indicators, selected radio cards. |
| `--color-primary-hover`| `#1D4ED8` (Blue 700) | Hover state for primary buttons and active links. |
| `--color-primary-subtle`| `#EFF6FF` (Blue 50) | Active card backgrounds, selected row highlighting. |
| `--color-primary-border`| `#93C5FD` (Blue 300) | Selected card borders, focused interactive rings. |

### 2.3 Semantic Status Palette (Algorithmic Outcomes)
Outcome colors are purposefully muted to avoid screaming at the user while maintaining immediate recognizability:

| Status Role | Base Hex | Light Surface Hex | Border Hex | Usage |
| :--- | :--- | :--- | :--- | :--- |
| **Counterexample / Suboptimal** | `#B45309` (Amber 700) | `#FFFBEB` (Amber 50) | `#FCD34D` (Amber 300) | Suboptimal gap indicators, critical divergence points. |
| **Feasibility Violation / Error** | `#BE123C` (Rose 700) | `#FFF1F2` (Rose 50) | `#FDA4AF` (Rose 300) | Infeasible greedy solutions, execution failures. |
| **Optimal Ground Truth / Match**| `#047857` (Emerald 700)| `#ECFDF5` (Emerald 50)| `#6EE7B7` (Emerald 300)| Oracle optimal result, deterministic match verification. |
| **Invariance / Neutral Evidence**| `#334155` (Slate 700) | `#F8FAFC` (Slate 50) | `#CBD5E1` (Slate 300) | No counterexample found in bounds, general search telemetry.|

---

## 3. Typography Hierarchy

The typography pairs a clean, modern sans-serif for interface controls and narrative readability with a high-grade monospace face for algorithmic inputs, parameters, and decision traces.

### 3.1 Font Families
- **UI & Reading Face**: `Inter`, `-apple-system`, `BlinkMacSystemFont`, `Segoe UI`, `Roboto`, `sans-serif`.
  - Tuned for clarity at small sizes with legible figures and balanced vertical metrics.
- **Code & Numerical Face**: `JetBrains Mono`, `Consolas`, `Monaco`, `monospace`.
  - Feature-rich tabular figures, distinct zero (`0` vs `O`), and crisp mathematical glyphs (`∈`, `∑`, `≤`, `≥`, `→`).

### 3.2 Type Scale

| Scale Role | Font Size | Line Height | Weight | Font Family | Usage |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Display Heading** | 30px / 1.875rem | 36px | Semi-Bold (600) | Inter | Home hero title, problem family title |
| **Page Heading (H1)** | 24px / 1.5rem | 32px | Semi-Bold (600) | Inter | Result page title, Experiment creator step title |
| **Section Heading (H2)**| 18px / 1.125rem | 26px | Semi-Bold (600) | Inter | Card titles, tab panel titles, table headers |
| **Subheading (H3)** | 15px / 0.9375rem | 22px | Medium (500) | Inter | Fieldset labels, subgroup dividers |
| **Body (Default)** | 14px / 0.875rem | 20px | Regular (400) | Inter | Explanatory copy, descriptions, card body |
| **Body Monospace** | 13px / 0.8125rem | 18px | Regular (400) | JetBrains Mono | Array values, parameter sets, decision options |
| **Metric Value (Hero)**| 28px / 1.75rem | 32px | Bold (700) | JetBrains Mono | Greedy value, Optimal value, Gap numbers |
| **Caption / Badge** | 12px / 0.75rem | 16px | Medium (500) | Inter | Status chips, timestamps, unit labels |

---

## 4. Layout, Spacing, and Elevation

### 4.1 Spacing Scale
The spacing system uses a standard 4px baseline grid:
- `space-1`: `4px` (Tight badge padding, icon gap)
- `space-2`: `8px` (Button inner padding, small component gaps)
- `space-3`: `12px` (Standard form field padding, chip spacing)
- `space-4`: `16px` (Card inner padding, standard container gaps)
- `space-6`: `24px` (Section gaps, major card padding)
- `space-8`: `32px` (Page header margin, grid row gaps)
- `space-12`: `48px` (Major section dividers)

### 4.2 Elevation & Borders
To maintain a crisp, grounded feel, Greedy Breaker relies on **1px structural borders** supplemented by very subtle, diffuse shadows rather than heavy drop shadows:
- **Card Default**: Border `1px solid var(--color-border-subtle)`, Shadow `0 1px 3px 0 rgba(0, 0, 0, 0.05)`.
- **Card Hover (Interactive)**: Border `1px solid var(--color-border-strong)`, Shadow `0 4px 6px -1px rgba(0, 0, 0, 0.08)`.
- **Selected Card**: Border `2px solid var(--color-primary-base)`, Shadow `0 0 0 1px var(--color-primary-base)`.
- **Modal / Flyout**: Border `1px solid var(--color-border-subtle)`, Shadow `0 10px 15px -3px rgba(0, 0, 0, 0.1)`.

---

## 5. Component Styling Guidelines

### 5.1 Buttons & Action Controls
- **Primary Action**: Deep slate-blue fill (`#1E40AF`), white text, 6px border-radius, font-weight 500. Clear active/press feedback (scale 0.98 or darker tint).
- **Secondary Action**: White fill, Slate-200 border, Slate-700 text. Hover transitions to Slate-50 background.
- **Danger Action**: Soft Rose-50 fill, Rose-300 border, Rose-700 text. Destructive actions always require explicit modal confirmation.
- **Keyboard Shortcuts**: Inline `<kbd>` badges styled with Slate-100 fill, Slate-300 border, and 11px monospace font (e.g. `Esc`, `Enter`).

### 5.2 Form Inputs & Parameter Sliders
- Every input has a permanent visible `<label>` and concise helper text.
- Numeric inputs for algorithm bounds (`Min`, `Max`, `Target`) use tabular monospace numbers.
- Sliders feature continuous numeric tooltips and direct typed numeric overrides for power users.
- Validation errors are presented inline directly beneath the field with an accessible alert icon and specific corrective guidance (e.g. *"Target value must be ≥ largest denomination"*).

### 5.3 Metric Quads (Hero Result Cards)
- Arranged in a 4-column horizontal grid on desktop, collapsing to 2x2 on tablet and 1x4 on mobile.
- Each metric card features:
  1. Uppercase 11px label with generous letter-spacing (`0.05em`).
  2. 28px bold monospace value with high-contrast slate-900 color.
  3. Microcopy note beneath the value indicating polarity and formula (e.g. `+1 coin (Greedy - Optimal)`).

### 5.4 Data Tables (Solutions & Evidence)
- Headers (`<th>`): Sticky on scroll, Slate-100 background, Slate-600 text, 12px uppercase medium weight.
- Cells (`<td>`): Monospace right-aligned for numeric data; left-aligned for item labels.
- Borders: Crisp horizontal 1px lines (`#E2E8F0`). Subtle row hover highlighting (`#F8FAFC`).
- No vertical zebra striping—clarity comes from generous cell padding (10px 16px).

---

## 6. Visualization Principles

Visual graphics must directly communicate algorithmic evidence. Decorative illustrations and generic charts are forbidden.

### 6.1 Permitted Algorithmic Visualizations

#### 1. Side-by-Side Solution Bar (Objective Comparison)
- Visualizes the gap between Greedy and Optimal as two horizontal or vertical comparative bars.
- Clear numeric value pinned to the end of each bar.
- Red/Amber bar segment highlighting the wasted capacity or suboptimal excess.

#### 2. Forensic Decision Bifurcation Tree / Timeline
- A vertical node-link line connecting decision points.
- Nodes represent sequential choices ($t=1, 2, 3$).
- At the *Critical Divergence Point*, the line bifurcates into two branches:
  - Branch A (Amber dotted line): Greedy trajectory leading to suboptimal outcome.
  - Branch B (Emerald solid line): Ground-truth optimal trajectory.
- Clear annotation cards at the divergence node explaining the irreversible constraint loss.

#### 3. Delta-Reduction Minimization Progression
- A 3-stage visual reduction card: `Raw Generated Candidate` ──> `Minimizer Delta Passes` ──> `Minimal Instance`.
- Shows strikethrough tokens for eliminated items and a green badge for preserved failure.

#### 4. Interval / Knapsack Packing Diagram
- For **Activity Selection**: Horizontal Gantt-style interval chart showing selected intervals vs. rejected overlapping intervals on a real-time axis.
- For **Knapsack**: Stacked capacity bar showing weight allocation relative to capacity limit $W$.

### 6.2 Prohibited Visual Elements
- ❌ Round donut / pie charts representing arbitrary statistics.
- ❌ Gauge dials with moving needles.
- ❌ Complex multi-line area charts without clear algorithmic meaning.
- ❌ Floating 3D cubes or generic particle animations.

---

## 7. Responsive Design Strategy

Desktop is the primary environment for algorithmic research and parameter configuration. However, the system must degrade gracefully across device form factors.

### 7.1 Breakpoints

| Breakpoint | Viewport Width | Device Target | Layout Adaptations |
| :--- | :--- | :--- | :--- |
| **Wide Desktop (`xl`)** | $\ge 1280\text{ px}$ | High-res monitors | Full 4-column metric quad, side-by-side solution comparison, multi-pane forensic timeline. Max content container $1200\text{ px}$. |
| **Standard Desktop (`lg`)** | $1024\text{ px} - 1279\text{ px}$ | Laptops, MacBooks | 4-column metrics maintained, slightly reduced padding. Form wizard shows side-by-side pre-flight summary card. |
| **Tablet (`md`)** | $768\text{ px} - 1023\text{ px}$ | iPad, Surface | Metric quad collapses to 2x2 grid. Solution comparison switches to stacked cards with sticky comparison toggle. Form wizard stacks summary below inputs. |
| **Mobile (`sm`)** | $< 768\text{ px}$ | Smartphones | Single-column layout. Metric quad collapses to 1-column stack. Tables gain horizontal overflow scroll with sticky first column. Full-width touch-friendly buttons ($44\text{ px}$ minimum tap target). |

---

## 8. Accessibility Standards (WCAG 2.1 AA Compliance)

Greedy Breaker is committed to universal accessibility for all researchers and students.

### 8.1 Keyboard Navigation & Focus Management
- **Full Tab Traversal**: Every interactive control (buttons, links, form inputs, tab headers, collapsible disclosure panels) is navigable via standard `Tab` and `Shift+Tab`.
- **Visible Focus Indicator**: All focused elements feature a high-contrast 2px solid blue focus ring (`#1E40AF`) with a 2px offset (`outline-offset: 2px`). Never suppress `:focus` outlines without replacing them with an accessible ring.
- **Skip Links**: The top of the page includes a `<a href="#main-content" class="sr-only focus:not-sr-only">Skip to main content</a>` element.
- **Modal Dialogs**: Trap focus within the modal when open, restore focus to triggering button on close, and dismiss on `Escape`.

### 8.2 Color Blindness & Non-Color Information Encoding
- Status indicators must **never rely solely on color**. Every status badge pairs color with an explicit textual label and a unique geometric icon:
  - Counterexample Discovered: Amber background + Triangle Warning Icon (`⚠`) + Text `OPTIMALITY COUNTEREXAMPLE`.
  - Invariance / No Breaks: Slate background + Circle Info Icon (`ℹ`) + Text `NO COUNTEREXAMPLE IN BOUNDS`.
  - Failure / Error: Rose background + Octagon Exclamation Icon (`⛔`) + Text `EXECUTION ERROR`.
  - Optimal / Verified: Emerald background + Checkmark Icon (`✓`) + Text `DETERMINISTIC MATCH`.

### 8.3 Screen Reader & Semantic HTML Attributes
- **Semantic Structure**: Proper use of `<header>`, `<nav>`, `<main>`, `<section>`, `<article>`, `<aside>`, and `<footer>`.
- **Heading Hierarchy**: Strict page structure with exactly one `<h1>` per page, followed by logical `<h2>`, `<h3>` nestings without skipping levels.
- **ARIA Annotations**:
  - Tab navigation uses `role="tablist"`, `role="tab"`, `aria-selected="true|false"`, and `role="tabpanel"`.
  - Real-time cardinality estimator uses `aria-live="polite"` so screen reader users hear space size updates without interruption.
  - Contrast tables use `<table aria-label="Greedy versus Optimal Solution Comparison">` with `<th scope="col">` and `<th scope="row">`.

---

## 9. Implementation Handoff Checklist for Claude Sonnet

When proceeding to the frontend implementation phase, the engineer must verify against the following criteria:

- [ ] All colors derive from the defined CSS custom properties (no hardcoded ad-hoc hex values).
- [ ] Typography uses `Inter` for interface copy and `JetBrains Mono` for all numbers, arrays, and parameter sets.
- [ ] No dark mode, neon accents, or glassmorphism effects are implemented.
- [ ] Metric quads display exact numbers, units, and explicit polarity labels.
- [ ] Discovered counterexamples present greedy and optimal solutions in identical tabular rows.
- [ ] Negative results state *"No counterexample was discovered within the configured search space"* (no claims of universal proof).
- [ ] Minimality displays the mandatory phrasing: *"Smallest verified counterexample under the configured ordering."*
- [ ] Reproduction creates a linked experiment with deterministic match verification.
- [ ] Keyboard navigation and visible focus rings operate seamlessly across all interactive elements.
- [ ] Responsive layouts adapt gracefully without breaking tabular data.
