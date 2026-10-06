// =============================================================================
// Home Page — Launchpad
// =============================================================================
import { Link } from 'react-router-dom';

export function HomePage() {
  return (
    <main id="main-content" className="page-content">
      <div className="container">
        {/* Hero */}
        <section aria-labelledby="hero-heading" style={{ marginBottom: 'var(--space-12)' }}>
          <div style={{ maxWidth: '680px' }}>
            <span
              className="badge badge-primary"
              style={{ marginBottom: 'var(--space-4)', display: 'inline-flex' }}
            >
              Algorithm Research Workbench
            </span>
            <h1 id="hero-heading" className="text-display" style={{ marginBottom: 'var(--space-4)' }}>
              Automated Counterexample Discovery for Greedy Algorithms
            </h1>
            <p
              className="text-secondary"
              style={{ fontSize: '16px', lineHeight: '24px', marginBottom: 'var(--space-6)' }}
            >
              Configure an experiment, let Greedy Breaker automatically investigate a greedy
              strategy against an exact oracle, and understand whether, when, and why it can
              be broken.
            </p>
            <div className="flex gap-3 flex-wrap">
              <Link to="/experiments/new" className="btn btn-primary btn-lg" id="home-create-experiment">
                Create Experiment
              </Link>
              <Link to="/experiments" className="btn btn-secondary btn-lg">
                View Experiments
              </Link>
              <Link to="/problems" className="btn btn-ghost btn-lg">
                Explore Problems
              </Link>
            </div>
          </div>
        </section>

        {/* Pipeline Diagram */}
        <section aria-labelledby="pipeline-heading" style={{ marginBottom: 'var(--space-12)' }}>
          <h2
            id="pipeline-heading"
            style={{ marginBottom: 'var(--space-4)', fontSize: '13px', color: 'var(--color-text-tertiary)', fontWeight: 600, textTransform: 'uppercase', letterSpacing: '0.05em' }}
          >
            Discovery Pipeline
          </h2>
          <div className="pipeline" role="list" aria-label="Counterexample discovery pipeline stages">
            {[
              { label: 'Problem Family', desc: 'Coin Change, Knapsack, …' },
              { label: 'Greedy Strategy', desc: 'Largest First, Density…' },
              { label: 'Candidate Search', desc: 'Exhaustive / Randomized' },
              { label: 'Exact Oracle', desc: 'DP / Exhaustive Verify' },
              { label: 'Counterexample', desc: 'Verified & Minimized' },
              { label: 'Forensic Analysis', desc: 'Divergence Trace' },
            ].map((step, i, arr) => (
              <span key={step.label} className="flex items-center gap-2" role="listitem">
                <span
                  className="pipeline-step"
                  title={step.desc}
                  style={{ display: 'flex', flexDirection: 'column', gap: '2px', alignItems: 'center' }}
                >
                  <span style={{ fontWeight: 600, fontSize: '13px' }}>{step.label}</span>
                  <span style={{ fontSize: '11px', color: 'var(--color-text-tertiary)' }}>{step.desc}</span>
                </span>
                {i < arr.length - 1 && (
                  <span className="pipeline-arrow" aria-hidden="true">→</span>
                )}
              </span>
            ))}
          </div>
        </section>

        {/* Problem Family Overview */}
        <section aria-labelledby="problems-heading" style={{ marginBottom: 'var(--space-12)' }}>
          <div className="flex items-center justify-between mb-4">
            <h2 id="problems-heading">Supported Problem Families</h2>
            <Link to="/problems" className="btn btn-ghost btn-sm">View All →</Link>
          </div>
          <div className="problem-grid" role="list">
            {PROBLEM_CARDS.map(p => (
              <article
                key={p.id}
                className="problem-card"
                role="listitem"
                aria-label={p.name}
                style={{ cursor: 'default' }}
              >
                <div className="flex items-center justify-between mb-3">
                  <span style={{ fontSize: '20px' }} aria-hidden="true">{p.icon}</span>
                  <span className={`badge ${p.objective === 'MINIMIZE' ? 'badge-primary' : 'badge-success'}`}>
                    {p.objective}
                  </span>
                </div>
                <h3 style={{ marginBottom: 'var(--space-2)' }}>{p.name}</h3>
                <p className="text-secondary text-sm" style={{ marginBottom: 'var(--space-3)' }}>
                  {p.description}
                </p>
                <div className="flex items-center gap-2 flex-wrap">
                  <span className="badge badge-neutral">{p.strategy}</span>
                  {p.breakable ? (
                    <span className="badge badge-counterexample">⚠ Breakable</span>
                  ) : (
                    <span className="badge badge-no-counterexample">ℹ Optimal</span>
                  )}
                </div>
              </article>
            ))}
          </div>
        </section>

        {/* Core Concepts */}
        <section aria-labelledby="concepts-heading">
          <h2 id="concepts-heading" style={{ marginBottom: 'var(--space-6)' }}>
            How Greedy Breaker Works
          </h2>
          <div
            style={{
              display: 'grid',
              gridTemplateColumns: 'repeat(auto-fill, minmax(220px, 1fr))',
              gap: 'var(--space-4)',
            }}
          >
            {CONCEPTS.map(c => (
              <div key={c.title} className="card">
                <div className="card-body">
                  <div style={{ fontSize: '22px', marginBottom: 'var(--space-3)' }} aria-hidden="true">
                    {c.icon}
                  </div>
                  <h3 style={{ marginBottom: 'var(--space-2)' }}>{c.title}</h3>
                  <p className="text-secondary text-sm">{c.description}</p>
                </div>
              </div>
            ))}
          </div>
        </section>

        {/* Important Disclaimer */}
        <div className="alert alert-info mt-8" role="note">
          <span className="alert-icon" aria-hidden="true">ℹ</span>
          <div className="alert-content">
            <div className="alert-title">Scientific Integrity</div>
            <p className="text-sm" style={{ marginTop: 0 }}>
              Greedy Breaker reports only empirical evidence from finite search spaces.
              The absence of a discovered counterexample within configured bounds does{' '}
              <strong>not</strong> constitute a mathematical proof of correctness.
              Coverage, search strategy, and bounds are always disclosed.
            </p>
          </div>
        </div>
      </div>
    </main>
  );
}

const PROBLEM_CARDS = [
  {
    id: 'coin_change',
    name: 'Coin Change (Fewest Coins)',
    description: 'Find the minimum number of coins to make change for a target value. Largest-first greedy fails on non-canonical coin systems.',
    objective: 'MINIMIZE' as const,
    strategy: 'Largest Denomination First',
    breakable: true,
    icon: '🪙',
  },
  {
    id: 'knapsack_01',
    name: '0/1 Knapsack Problem',
    description: 'Maximize total value of discrete items subject to a weight capacity. Value-density greedy fails for indivisible items.',
    objective: 'MAXIMIZE' as const,
    strategy: 'Value Density Descending',
    breakable: true,
    icon: '🎒',
  },
  {
    id: 'activity_selection',
    name: 'Activity Selection',
    description: 'Maximize non-overlapping activities in a single room. Earliest Finish Time is theoretically optimal for compatible activities.',
    objective: 'MAXIMIZE' as const,
    strategy: 'Earliest Finish Time',
    breakable: false,
    icon: '📅',
  },
  {
    id: 'fractional_knapsack',
    name: 'Fractional Knapsack',
    description: 'Maximizes value when arbitrary fractions of items may be selected. Value-density greedy is provably optimal for continuous fractions.',
    objective: 'MAXIMIZE' as const,
    strategy: 'Value Density Descending',
    breakable: false,
    icon: '⚖️',
  },
  {
    id: 'huffman_coding',
    name: 'Huffman Coding',
    description: 'Construct an optimal prefix-free code minimizing expected code length. Shannon-Fano split is a weaker, breakable alternative.',
    objective: 'MINIMIZE' as const,
    strategy: 'Standard Huffman Merge',
    breakable: false,
    icon: '🌲',
  },
];

const CONCEPTS = [
  {
    icon: '🔍',
    title: 'Counterexample Search',
    description:
      'Automatically enumerate or sample candidate problem instances and test each against a configurable greedy heuristic.',
  },
  {
    icon: '✓',
    title: 'Exact Oracle Verification',
    description:
      'Every candidate is compared against a deterministic exact algorithm (DP or exhaustive enumeration) to certify suboptimality.',
  },
  {
    icon: '⬇',
    title: 'Minimal Instance Reduction',
    description:
      'Discovered counterexamples are delta-reduced to the smallest verified failure under the configured ordering criteria.',
  },
  {
    icon: '🔬',
    title: 'Forensic Divergence Trace',
    description:
      'The exact decision step where the greedy heuristic diverged from optimal is identified, with causal context from actual engine output.',
  },
  {
    icon: '⟳',
    title: 'Deterministic Reproducibility',
    description:
      'Every experiment is seeded and versioned. Reproduction creates a new, linked experiment that verifies identical output.',
  },
  {
    icon: '📋',
    title: 'Scholarly Honesty',
    description:
      'Non-discovery within bounds is reported as empirical evidence, never as a proof of correctness.',
  },
];
