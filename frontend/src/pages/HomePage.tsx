import { ArrowRight, Cpu } from 'lucide-react';

interface HomePageProps {
  onStartInvestigation: () => void;
  onOpenVerify: () => void;
}

export const HomePage: React.FC<HomePageProps> = ({
  onStartInvestigation,
  onOpenVerify,
}) => {
  const scrollToHowItWorks = () => {
    document.getElementById('how-it-works')?.scrollIntoView({ behavior: 'smooth' });
  };

  return (
    <div>
      {/* 1. Hero Section */}
      <section className="hero-section">
        <div className="hero-pill">
          <span>Automated Counterexample Discovery Engine</span>
        </div>

        <h1 className="hero-title">
          Automatically discover the inputs that break greedy algorithms.
        </h1>

        <p className="hero-subtitle">
          Greedy Breaker systematically searches bounded input spaces, compares a greedy
          strategy against an exact solver, verifies counterexamples, and analyzes where the
          greedy decision first diverges from the optimal solution.
        </p>

        <div className="hero-actions">
          <button
            type="button"
            className="btn btn-break btn-lg"
            onClick={onStartInvestigation}
          >
            <span>Start an Investigation</span>
            <ArrowRight size={16} />
          </button>
          <button
            type="button"
            className="btn btn-secondary btn-lg"
            onClick={scrollToHowItWorks}
          >
            Explore How It Works
          </button>
        </div>
      </section>

      {/* 2. Architectural Flow Visualization */}
      <section className="flow-diagram-container">
        <div className="flow-diagram-title">System Architecture & Forensic Evaluation Pipeline</div>

        <div className="flow-grid">
          {/* Candidate Input */}
          <div className="flow-node">
            <div className="flow-node-title">Candidate Input</div>
            <div className="flow-node-sub font-mono">Instance (S, w, n)</div>
          </div>

          <div className="flow-arrow">
            <ArrowRight size={20} />
          </div>

          {/* Dual Solvers Column */}
          <div style={{ display: 'flex', flexDirection: 'column', gap: '1rem' }}>
            <div
              className="flow-node"
              style={{ borderLeft: '3px solid #dc2626', background: '#fef2f2' }}
            >
              <div className="flow-node-title" style={{ color: '#991b1b' }}>
                Greedy Strategy
              </div>
              <div className="flow-node-sub">Locally Optimal Heuristic</div>
            </div>

            <div
              className="flow-node"
              style={{ borderLeft: '3px solid #2563eb', background: '#eff6ff' }}
            >
              <div className="flow-node-title" style={{ color: '#1e40af' }}>
                Exact Oracle
              </div>
              <div className="flow-node-sub">Dynamic Programming Benchmark</div>
            </div>
          </div>

          <div className="flow-arrow">
            <ArrowRight size={20} />
          </div>

          {/* Comparison & Divergence */}
          <div
            className="flow-node"
            style={{ borderLeft: '3px solid #0f172a', background: '#f8fafc' }}
          >
            <div className="flow-node-title">Dual Verification</div>
            <div className="flow-node-sub font-mono">
              Compare Cost &rarr; Divergence Step
            </div>
          </div>
        </div>
      </section>

      {/* 3. Real Example Section: See a greedy algorithm break */}
      <section className="product-section">
        <div className="section-header">
          <div className="section-eyebrow">Concrete Demonstration</div>
          <h2 className="section-title">See a greedy algorithm break.</h2>
          <p className="section-desc">
            Greedy heuristics make locally optimal choices at each decision point. In non-canonical
            problem instances, this shortsightedness traps the algorithm into suboptimal outcomes.
          </p>
        </div>

        <div
          style={{
            background: '#ffffff',
            border: '1px solid var(--border-subtle)',
            borderRadius: 'var(--radius-lg)',
            padding: '2rem',
            boxShadow: 'var(--shadow-subtle)',
          }}
        >
          {/* Instance Header */}
          <div
            style={{
              display: 'flex',
              justifyContent: 'space-between',
              alignItems: 'center',
              flexWrap: 'wrap',
              gap: '1rem',
              paddingBottom: '1.25rem',
              borderBottom: '1px solid var(--border-subtle)',
              marginBottom: '1.5rem',
            }}
          >
            <div>
              <span style={{ fontSize: '0.8rem', fontWeight: 600, color: 'var(--text-muted)', textTransform: 'uppercase' }}>
                Problem Family:
              </span>{' '}
              <strong>Coin Change Problem</strong>
            </div>

            <div style={{ display: 'flex', gap: '1.5rem', alignItems: 'center' }}>
              <div>
                <span style={{ fontSize: '0.8rem', color: 'var(--text-muted)' }}>Denominations: </span>
                <span className="font-mono" style={{ fontWeight: 600 }}>&#123;1, 3, 4&#125;</span>
              </div>
              <div>
                <span style={{ fontSize: '0.8rem', color: 'var(--text-muted)' }}>Target: </span>
                <span className="font-mono" style={{ fontWeight: 700, color: 'var(--accent-primary)' }}>6</span>
              </div>
            </div>
          </div>

          {/* Comparison Columns */}
          <div className="grid-2" style={{ marginBottom: '1.5rem' }}>
            {/* Greedy Column */}
            <div
              style={{
                border: '1px solid var(--status-failure-border)',
                background: 'var(--status-failure-bg)',
                borderRadius: 'var(--radius-md)',
                padding: '1.5rem',
              }}
            >
              <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '0.75rem' }}>
                <span style={{ fontWeight: 700, color: '#991b1b', fontSize: '1rem' }}>
                  Greedy Choice (Largest-First)
                </span>
                <span className="badge badge-failure">Suboptimal</span>
              </div>

              <div style={{ fontSize: '0.9rem', color: 'var(--text-secondary)', marginBottom: '1rem' }}>
                Greedy immediately selects coin <strong>4</strong> (the largest denomination &le; 6).
                Remaining target is 2, requiring two 1-cent coins.
              </div>

              <div
                className="font-mono"
                style={{
                  background: '#ffffff',
                  border: '1px solid var(--status-failure-border)',
                  borderRadius: 'var(--radius-sm)',
                  padding: '0.75rem 1rem',
                  fontSize: '1.1rem',
                  fontWeight: 700,
                  color: '#991b1b',
                  display: 'flex',
                  justifyContent: 'space-between',
                }}
              >
                <span>4 + 1 + 1</span>
                <span>= 3 coins</span>
              </div>
            </div>

            {/* Optimal Column */}
            <div
              style={{
                border: '1px solid var(--status-success-border)',
                background: 'var(--status-success-bg)',
                borderRadius: 'var(--radius-md)',
                padding: '1.5rem',
              }}
            >
              <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '0.75rem' }}>
                <span style={{ fontWeight: 700, color: '#166534', fontSize: '1rem' }}>
                  Exact Solver (Dynamic Programming)
                </span>
                <span className="badge badge-success">Optimal</span>
              </div>

              <div style={{ fontSize: '0.9rem', color: 'var(--text-secondary)', marginBottom: '1rem' }}>
                Exact DP oracle looks ahead and recognizes that two <strong>3-cent</strong> coins
                satisfy target 6 with strictly fewer coins.
              </div>

              <div
                className="font-mono"
                style={{
                  background: '#ffffff',
                  border: '1px solid var(--status-success-border)',
                  borderRadius: 'var(--radius-sm)',
                  padding: '0.75rem 1rem',
                  fontSize: '1.1rem',
                  fontWeight: 700,
                  color: '#166534',
                  display: 'flex',
                  justifyContent: 'space-between',
                }}
              >
                <span>3 + 3</span>
                <span>= 2 coins</span>
              </div>
            </div>
          </div>

          {/* Forensic Takeaway */}
          <div
            style={{
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'space-between',
              flexWrap: 'wrap',
              gap: '1rem',
              background: 'var(--bg-subtle)',
              padding: '1rem 1.25rem',
              borderRadius: 'var(--radius-md)',
            }}
          >
            <div style={{ fontSize: '0.9rem', color: 'var(--text-secondary)' }}>
              <strong>Result:</strong> Absolute Gap = <strong>1 coin</strong> | Overhead ={' '}
              <strong>50.0%</strong> | First Divergence at <strong>Step 1</strong> (Selected 4 vs
              Optimal 3).
            </div>
            <button
              type="button"
              className="btn btn-outline-primary"
              style={{ fontSize: '0.8rem', padding: '0.35rem 0.75rem' }}
              onClick={onOpenVerify}
            >
              Verify this instance live &rarr;
            </button>
          </div>
        </div>
      </section>

      {/* 4. Supported Algorithms Section */}
      <section className="product-section">
        <div className="section-header">
          <div className="section-eyebrow">Problem Taxonomy</div>
          <h2 className="section-title">Four algorithm families. One investigation framework.</h2>
          <p className="section-desc">
            Greedy Breaker tests both vulnerable heuristics and mathematically proven optimal
            algorithms within a unified C++20 search architecture.
          </p>
        </div>

        <div
          style={{
            display: 'grid',
            gridTemplateColumns: 'repeat(auto-fit, minmax(260px, 1fr))',
            gap: '1.5rem',
          }}
        >
          {/* 1. Coin Change */}
          <div
            style={{
              background: '#ffffff',
              border: '1px solid var(--border-subtle)',
              borderRadius: 'var(--radius-md)',
              padding: '1.5rem',
            }}
          >
            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '0.5rem' }}>
              <h3 style={{ fontSize: '1.05rem', margin: 0 }}>Coin Change</h3>
              <span className="badge badge-failure">Breachable</span>
            </div>
            <div style={{ fontSize: '0.85rem', color: 'var(--text-muted)', marginBottom: '1rem' }}>
              Minimization Objective
            </div>
            <div style={{ fontSize: '0.85rem', lineHeight: '1.5' }}>
              <div style={{ marginBottom: '0.4rem' }}>
                <span style={{ color: 'var(--text-muted)' }}>Greedy: </span>
                <strong>Largest denomination first</strong>
              </div>
              <div>
                <span style={{ color: 'var(--text-muted)' }}>Oracle: </span>
                <strong>Exact Dynamic Programming</strong>
              </div>
            </div>
          </div>

          {/* 2. Activity Selection */}
          <div
            style={{
              background: '#ffffff',
              border: '1px solid var(--border-subtle)',
              borderRadius: 'var(--radius-md)',
              padding: '1.5rem',
            }}
          >
            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '0.5rem' }}>
              <h3 style={{ fontSize: '1.05rem', margin: 0 }}>Activity Selection</h3>
              <span className="badge badge-success">Optimal</span>
            </div>
            <div style={{ fontSize: '0.85rem', color: 'var(--text-muted)', marginBottom: '1rem' }}>
              Maximization Objective
            </div>
            <div style={{ fontSize: '0.85rem', lineHeight: '1.5' }}>
              <div style={{ marginBottom: '0.4rem' }}>
                <span style={{ color: 'var(--text-muted)' }}>Greedy: </span>
                <strong>Earliest finish time (EFT)</strong>
              </div>
              <div>
                <span style={{ color: 'var(--text-muted)' }}>Oracle: </span>
                <strong>Exact Interval Scheduling DP</strong>
              </div>
            </div>
          </div>

          {/* 3. 0/1 Knapsack */}
          <div
            style={{
              background: '#ffffff',
              border: '1px solid var(--border-subtle)',
              borderRadius: 'var(--radius-md)',
              padding: '1.5rem',
            }}
          >
            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '0.5rem' }}>
              <h3 style={{ fontSize: '1.05rem', margin: 0 }}>0/1 Knapsack</h3>
              <span className="badge badge-failure">Breachable</span>
            </div>
            <div style={{ fontSize: '0.85rem', color: 'var(--text-muted)', marginBottom: '1rem' }}>
              Maximization Objective
            </div>
            <div style={{ fontSize: '0.85rem', lineHeight: '1.5' }}>
              <div style={{ marginBottom: '0.4rem' }}>
                <span style={{ color: 'var(--text-muted)' }}>Greedy: </span>
                <strong>Value-to-weight density</strong>
              </div>
              <div>
                <span style={{ color: 'var(--text-muted)' }}>Oracle: </span>
                <strong>Exact 2D Dynamic Programming</strong>
              </div>
            </div>
          </div>

          {/* 4. Fractional Knapsack */}
          <div
            style={{
              background: '#ffffff',
              border: '1px solid var(--border-subtle)',
              borderRadius: 'var(--radius-md)',
              padding: '1.5rem',
            }}
          >
            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '0.5rem' }}>
              <h3 style={{ fontSize: '1.05rem', margin: 0 }}>Fractional Knapsack</h3>
              <span className="badge badge-success">Optimal</span>
            </div>
            <div style={{ fontSize: '0.85rem', color: 'var(--text-muted)', marginBottom: '1rem' }}>
              Maximization Objective
            </div>
            <div style={{ fontSize: '0.85rem', lineHeight: '1.5' }}>
              <div style={{ marginBottom: '0.4rem' }}>
                <span style={{ color: 'var(--text-muted)' }}>Greedy: </span>
                <strong>Value-to-weight density</strong>
              </div>
              <div>
                <span style={{ color: 'var(--text-muted)' }}>Oracle: </span>
                <strong>Exact continuous greedy solution</strong>
              </div>
            </div>
          </div>
        </div>
      </section>

      {/* 5. How It Works Section */}
      <section id="how-it-works" className="product-section">
        <div className="section-header">
          <div className="section-eyebrow">Methodology</div>
          <h2 className="section-title">How it works.</h2>
          <p className="section-desc">
            A four-stage systematic pipeline exploring the boundary where greedy heuristics
            fail to achieve global optimality.
          </p>
        </div>

        <div
          style={{
            display: 'grid',
            gridTemplateColumns: 'repeat(auto-fit, minmax(240px, 1fr))',
            gap: '1.5rem',
          }}
        >
          <div style={{ padding: '1.5rem', borderLeft: '2px solid var(--border-strong)' }}>
            <div className="font-mono" style={{ fontSize: '0.85rem', color: 'var(--accent-primary)', fontWeight: 700, marginBottom: '0.5rem' }}>
              01
            </div>
            <h3 style={{ fontSize: '1.05rem', marginBottom: '0.5rem' }}>Generate Candidates</h3>
            <p style={{ fontSize: '0.875rem', color: 'var(--text-muted)' }}>
              Configurable search generators (exhaustive, backtracking with pruning, randomized, or heuristic)
              systematically instantiate valid problem configurations within bounded parameters.
            </p>
          </div>

          <div style={{ padding: '1.5rem', borderLeft: '2px solid var(--border-strong)' }}>
            <div className="font-mono" style={{ fontSize: '0.85rem', color: 'var(--accent-primary)', fontWeight: 700, marginBottom: '0.5rem' }}>
              02
            </div>
            <h3 style={{ fontSize: '1.05rem', marginBottom: '0.5rem' }}>Run Greedy Strategy</h3>
            <p style={{ fontSize: '0.875rem', color: 'var(--text-muted)' }}>
              The canonical greedy solver processes the candidate instance, generating an execution trace of
              step-by-step choices and recording the resulting heuristic solution cost.
            </p>
          </div>

          <div style={{ padding: '1.5rem', borderLeft: '2px solid var(--border-strong)' }}>
            <div className="font-mono" style={{ fontSize: '0.85rem', color: 'var(--accent-primary)', fontWeight: 700, marginBottom: '0.5rem' }}>
              03
            </div>
            <h3 style={{ fontSize: '1.05rem', marginBottom: '0.5rem' }}>Compare Against Exact Solver</h3>
            <p style={{ fontSize: '0.875rem', color: 'var(--text-muted)' }}>
              An exact dynamic programming oracle executes independently to find the true mathematical optimum.
              A counterexample is flagged whenever the greedy solution differs from optimal cost.
            </p>
          </div>

          <div style={{ padding: '1.5rem', borderLeft: '2px solid var(--border-strong)' }}>
            <div className="font-mono" style={{ fontSize: '0.85rem', color: 'var(--accent-primary)', fontWeight: 700, marginBottom: '0.5rem' }}>
              04
            </div>
            <h3 style={{ fontSize: '1.05rem', marginBottom: '0.5rem' }}>Analyze the Failure</h3>
            <p style={{ fontSize: '0.875rem', color: 'var(--text-muted)' }}>
              Forensic divergence analysis pinpoints the exact decision point where greedy choices branched away
              from optimal choices, computing failure overhead and minimality verification.
            </p>
          </div>
        </div>
      </section>

      {/* 6. Why the Exact Solver Matters Section */}
      <section className="product-section">
        <div
          style={{
            background: '#ffffff',
            border: '1px solid var(--border-subtle)',
            borderRadius: 'var(--radius-lg)',
            padding: '2.5rem 2rem',
            display: 'flex',
            alignItems: 'flex-start',
            gap: '1.5rem',
            boxShadow: 'var(--shadow-subtle)',
          }}
        >
          <div
            style={{
              width: '42px',
              height: '42px',
              borderRadius: 'var(--radius-md)',
              background: 'var(--accent-subtle)',
              border: '1px solid var(--accent-border)',
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'center',
              color: 'var(--accent-primary)',
              flexShrink: 0,
            }}
          >
            <Cpu size={22} />
          </div>

          <div style={{ flex: 1 }}>
            <h2 style={{ fontSize: '1.4rem', fontWeight: 700, marginBottom: '0.75rem' }}>
              Greedy Breaker does not assume the greedy answer is correct.
            </h2>
            <p style={{ fontSize: '0.95rem', color: 'var(--text-secondary)', lineHeight: '1.6', marginBottom: '1rem' }}>
              In algorithm analysis, verifying a counterexample requires an authoritative oracle.
              Greedy Breaker never uses heuristic approximations as a reference point.
              Every discovered candidate is evaluated against an exact solver (typically Dynamic
              Programming with memoization or optimal substructure reconstruction).
            </p>
            <p style={{ fontSize: '0.95rem', color: 'var(--text-secondary)', lineHeight: '1.6' }}>
              The exact solver is the ultimate judge: it guarantees that any discrepancy reported
              as a counterexample is mathematically genuine and reproducible.
            </p>
          </div>
        </div>
      </section>
    </div>
  );
};
