// =============================================================================
// Documentation Page — Inline reference for Greedy Breaker concepts
// =============================================================================

export function DocsPage() {
  return (
    <main id="main-content" className="page-content">
      <div className="container">
        <div style={{ maxWidth: '780px' }}>
          <h1 style={{ marginBottom: 'var(--space-2)' }}>Documentation</h1>
          <p className="text-secondary" style={{ marginBottom: 'var(--space-8)' }}>
            Technical reference for Greedy Breaker V2: concepts, workflows, and accuracy caveats.
          </p>

          {DOCS_SECTIONS.map(section => (
            <section
              key={section.id}
              id={section.id}
              aria-labelledby={`doc-${section.id}`}
              style={{ marginBottom: 'var(--space-8)' }}
            >
              <h2 id={`doc-${section.id}`} style={{ marginBottom: 'var(--space-3)' }}>
                {section.title}
              </h2>
              <div
                className="card"
                style={{ overflow: 'hidden' }}
              >
                <div className="card-body">
                  {section.content}
                </div>
              </div>
            </section>
          ))}
        </div>
      </div>
    </main>
  );
}

const P = ({ children }: { children: React.ReactNode }) => (
  <p style={{ marginBottom: 'var(--space-3)', color: 'var(--color-text-secondary)', lineHeight: '22px' }}>
    {children}
  </p>
);

const H3 = ({ children }: { children: React.ReactNode }) => (
  <h3 style={{ marginBottom: 'var(--space-2)', marginTop: 'var(--space-4)' }}>{children}</h3>
);

const DOCS_SECTIONS = [
  {
    id: 'what-is-greedy-breaker',
    title: '1. What is Greedy Breaker?',
    content: (
      <>
        <P>
          Greedy Breaker is an automated counterexample discovery platform for greedy algorithms.
          It automatically constructs and evaluates candidate problem instances, compares the
          greedy heuristic's output against a verified exact oracle, and reports concrete
          counterexamples when the greedy strategy produces suboptimal solutions.
        </P>
        <P>
          The platform is designed for algorithm researchers, students, and educators who need
          rigorous, reproducible evidence about when and why greedy heuristics fail — not
          merely theoretical proofs or cherry-picked examples.
        </P>
      </>
    ),
  },
  {
    id: 'counterexample-discovery',
    title: '2. How Counterexample Discovery Works',
    content: (
      <>
        <P>
          The discovery pipeline follows these stages:
        </P>
        <ol style={{ paddingLeft: 'var(--space-5)', color: 'var(--color-text-secondary)', lineHeight: '24px' }}>
          <li><strong>Candidate generation:</strong> instances are constructed within the configured search space bounds.</li>
          <li><strong>Greedy evaluation:</strong> the chosen heuristic solves each instance and records its selection trace.</li>
          <li><strong>Oracle comparison:</strong> an exact algorithm (DP, exhaustive, or LP) computes the ground-truth optimum.</li>
          <li><strong>Counterexample verification:</strong> if greedy ≠ optimal, the instance is verified as a confirmed failure.</li>
          <li><strong>Delta-reduction:</strong> the verified counterexample is minimized to its smallest form under a lexicographic ordering.</li>
          <li><strong>Forensic analysis:</strong> the decision trace is inspected to identify the exact divergence step.</li>
        </ol>
      </>
    ),
  },
  {
    id: 'greedy-strategy',
    title: '3. Greedy Strategy',
    content: (
      <>
        <P>
          A greedy strategy is a local decision rule applied step-by-step to build a solution.
          At each step, the heuristic makes the choice that appears best under its local objective —
          without considering future consequences.
        </P>
        <P>
          Examples: "Select the largest coin available," "Take the item with highest value density."
          Greedy strategies are efficient but do not always produce globally optimal results.
        </P>
      </>
    ),
  },
  {
    id: 'exact-oracle',
    title: '4. Exact Oracle',
    content: (
      <>
        <P>
          The exact oracle is an authoritative algorithm that provably computes the global optimum
          for any given instance. It may be computationally expensive but is used only for verification,
          not for large-scale enumeration.
        </P>
        <P>
          Examples: Dynamic Programming O(K·T) for Coin Change, DP O(N·W) for 0/1 Knapsack,
          Exhaustive subset enumeration O(2^N) for exact subset problems.
        </P>
        <P>
          <strong>
            The frontend never computes oracle results. All verification is performed by the
            C++20 engine and reported through the backend API.
          </strong>
        </P>
      </>
    ),
  },
  {
    id: 'search-modes',
    title: '5. Search Modes',
    content: (
      <>
        <H3>ANY</H3>
        <P>Find the first verified counterexample and stop. Fastest mode when you just need to confirm a failure exists.</P>
        <H3>SMALLEST</H3>
        <P>Continue searching after the first discovery to find the smallest counterexample under the configured ordering: element count → max parameter → parameter sum → lexicographic order.</P>
        <H3>WORST</H3>
        <P>Search the entire budget for the instance with the largest observed objective gap. Identifies the most severe failure case within bounds.</P>
        <H3>PROFILE</H3>
        <P>Evaluate all or sampled candidates without early termination. Reports failure rate, coverage, and distribution statistics. Does not claim correctness on non-failure.</P>
      </>
    ),
  },
  {
    id: 'search-strategies',
    title: '6. Search Strategies',
    content: (
      <>
        <H3>EXHAUSTIVE</H3>
        <P>Enumerate every candidate instance within the configured parameter bounds. Coverage is complete for the bounded space. Suitable for small, tight search spaces.</P>
        <H3>RANDOMIZED</H3>
        <P>Sample instances uniformly at random from the configured bounds. Coverage is partial; the random seed controls reproducibility. Suitable for large search spaces.</P>
        <H3>GUIDED</H3>
        <P>Use heuristic guidance to direct candidate generation toward regions likely to contain failures. Coverage is focused rather than uniform.</P>
        <H3>HYBRID</H3>
        <P>Combine exhaustive enumeration in core regions with randomized sampling in outer regions. Balances coverage and efficiency.</P>
      </>
    ),
  },
  {
    id: 'minimization',
    title: '7. Counterexample Minimization',
    content: (
      <>
        <P>
          When a counterexample is found, the engine applies delta-reduction to produce the
          smallest verified failure instance under a configured lexicographic ordering:
        </P>
        <ol style={{ paddingLeft: 'var(--space-5)', color: 'var(--color-text-secondary)', lineHeight: '24px' }}>
          <li>Minimize element count (N, K)</li>
          <li>Minimize maximum parameter magnitude</li>
          <li>Minimize parameter sum</li>
          <li>Lexicographic ordering as tiebreaker</li>
        </ol>
        <P>
          <strong>
            Minimality is always stated as: "Smallest verified counterexample under the configured ordering."
            Global unconstrained minimality is not claimed.
          </strong>
        </P>
      </>
    ),
  },
  {
    id: 'forensic-analysis',
    title: '8. Forensic Analysis',
    content: (
      <>
        <P>
          The Forensic Divergence Report identifies the exact decision step where the greedy heuristic
          produced a suboptimal choice that later led to overall suboptimality.
        </P>
        <P>
          The report includes:
        </P>
        <ul style={{ paddingLeft: 'var(--space-5)', color: 'var(--color-text-secondary)', lineHeight: '24px' }}>
          <li>The full greedy decision trace (each step's action, state, and heuristic rule)</li>
          <li>The critical divergence step index</li>
          <li>The greedy choice at divergence</li>
          <li>The optimal alternative at that step</li>
          <li>The causal consequence (from engine output, not fabricated)</li>
        </ul>
        <P>
          <strong>
            If the engine cannot establish a causal explanation, the UI reports "Observed divergence"
            without fabricating a reason.
          </strong>
        </P>
      </>
    ),
  },
  {
    id: 'reproducibility',
    title: '9. Reproducibility',
    content: (
      <>
        <P>
          Every experiment is seeded and immutable. Running a reproduction creates a new experiment
          with <code style={{ fontFamily: 'var(--font-mono)', fontSize: '13px' }}>parent_experiment_id</code> pointing
          to the original. Both runs are preserved side-by-side.
        </P>
        <P>
          Deterministic match verification compares the reproduction's output against the original.
          A confirmed match indicates that the engine and configuration produce identical results
          across independent runs.
        </P>
      </>
    ),
  },
  {
    id: 'coverage-and-limitations',
    title: '10. Coverage & Limitations',
    content: (
      <>
        <P>
          <strong>Non-discovery does not imply correctness.</strong> When an experiment completes
          without finding a counterexample, Greedy Breaker reports:
        </P>
        <blockquote
          style={{
            borderLeft: '3px solid var(--color-border-strong)',
            paddingLeft: 'var(--space-4)',
            margin: 'var(--space-3) 0',
            color: 'var(--color-text-secondary)',
            fontStyle: 'italic',
          }}
        >
          "No counterexample was discovered within the configured search space."
        </blockquote>
        <P>
          This result is an empirical statement about the bounded search, not a universal mathematical
          proof. To strengthen evidence, expand parameter bounds, switch to exhaustive search,
          or evaluate additional heuristic variants.
        </P>
        <P>
          Coverage ratio (candidates evaluated / total bounded space) is always disclosed so
          you can assess confidence in a negative result.
        </P>
      </>
    ),
  },
];
