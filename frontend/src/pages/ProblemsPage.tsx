// =============================================================================
// Problems Page — Research catalogue of supported problem families
// =============================================================================
import { useState, useEffect } from 'react';
import { useParams, Link } from 'react-router-dom';
import { problemsApi, getErrorMessage } from '../api';
import type { ProblemMetadata } from '../types';
import {
  Breadcrumb,
  LoadingPage,
  ErrorState,
  ObjectiveBadge,
} from '../components/Shared';

// --- Problems List --------------------------------------------------------
export function ProblemsPage() {
  const [problems, setProblems] = useState<ProblemMetadata[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState('');

  useEffect(() => {
    problemsApi.list()
      .then(setProblems)
      .catch(e => setError(getErrorMessage(e)))
      .finally(() => setLoading(false));
  }, []);

  if (loading) return <LoadingPage message="Loading problem catalogue…" />;
  if (error) return <ErrorState message={error} />;

  return (
    <main id="main-content" className="page-content">
      <div className="container">
        <Breadcrumb items={[{ label: 'Problems' }]} />
        <div className="page-header">
          <h1>Problem Family Catalogue</h1>
          <p className="text-secondary">
            Supported algorithmic problem families with their greedy strategies and
            ground-truth oracles. Each problem type defines the search space
            parameters and candidate generation rules used by the engine.
          </p>
        </div>

        <div style={{ display: 'flex', flexDirection: 'column', gap: 'var(--space-4)' }}>
          {problems.map(p => (
            <article
              key={p.id}
              className="card"
              aria-labelledby={`problem-${p.id}-heading`}
            >
              <div className="card-header">
                <div>
                  <div className="flex items-center gap-3 mb-1">
                    <ObjectiveBadge objective={p.objective} />
                    <code
                      style={{
                        fontSize: '12px',
                        fontFamily: 'var(--font-mono)',
                        color: 'var(--color-text-tertiary)',
                      }}
                    >
                      {p.id}
                    </code>
                  </div>
                  <h2 id={`problem-${p.id}-heading`}>{p.name}</h2>
                </div>
                <Link
                  to={`/problems/${p.id}`}
                  className="btn btn-secondary btn-sm"
                  aria-label={`View details for ${p.name}`}
                >
                  Details →
                </Link>
              </div>
              <div className="card-body">
                <p className="text-secondary" style={{ marginBottom: 'var(--space-4)' }}>
                  {p.description}
                </p>

                <div
                  style={{
                    display: 'grid',
                    gridTemplateColumns: '1fr 1fr',
                    gap: 'var(--space-4)',
                  }}
                >
                  <div>
                    <h3 style={{ marginBottom: 'var(--space-3)' }}>Greedy Strategies</h3>
                    <div style={{ display: 'flex', flexDirection: 'column', gap: 'var(--space-2)' }}>
                      {p.strategies.map(s => (
                        <div
                          key={s.id}
                          style={{
                            display: 'flex',
                            alignItems: 'center',
                            gap: 'var(--space-2)',
                            padding: 'var(--space-2) var(--space-3)',
                            background: 'var(--color-surface-subtle)',
                            borderRadius: 'var(--radius-md)',
                          }}
                        >
                          <span style={{ fontSize: '13px' }}>{s.name}</span>
                          {s.default && (
                            <span className="badge badge-primary" style={{ marginLeft: 'auto' }}>Default</span>
                          )}
                        </div>
                      ))}
                    </div>
                  </div>

                  <div>
                    <h3 style={{ marginBottom: 'var(--space-3)' }}>Ground-Truth Oracles</h3>
                    <div style={{ display: 'flex', flexDirection: 'column', gap: 'var(--space-2)' }}>
                      {p.oracles.map(o => (
                        <div
                          key={o.id}
                          style={{
                            display: 'flex',
                            alignItems: 'center',
                            gap: 'var(--space-2)',
                            padding: 'var(--space-2) var(--space-3)',
                            background: 'var(--color-surface-subtle)',
                            borderRadius: 'var(--radius-md)',
                          }}
                        >
                          <span style={{ fontSize: '13px', flex: 1 }}>{o.name}</span>
                          <span className="badge badge-success">{o.confidence}</span>
                          {o.default && (
                            <span className="badge badge-primary">Default</span>
                          )}
                        </div>
                      ))}
                    </div>
                  </div>
                </div>
              </div>
              <div className="card-footer">
                <Link
                  to={`/experiments/new`}
                  className="btn btn-primary btn-sm"
                  aria-label={`Create experiment for ${p.name}`}
                >
                  + Run Experiment on This Problem
                </Link>
              </div>
            </article>
          ))}
        </div>
      </div>
    </main>
  );
}

// --- Problem Detail -------------------------------------------------------
export function ProblemDetailPage() {
  const { problemId } = useParams<{ problemId: string }>();
  const [problem, setProblem] = useState<ProblemMetadata | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState('');

  useEffect(() => {
    if (!problemId) return;
    problemsApi.get(problemId)
      .then(setProblem)
      .catch(e => setError(getErrorMessage(e)))
      .finally(() => setLoading(false));
  }, [problemId]);

  if (loading) return <LoadingPage message="Loading problem details…" />;
  if (error) return <ErrorState message={error} />;
  if (!problem) return null;

  return (
    <main id="main-content" className="page-content">
      <div className="container">
        <Breadcrumb
          items={[
            { label: 'Problems', to: '/problems' },
            { label: problem.name },
          ]}
        />

        <div className="flex items-center gap-3 mb-4">
          <ObjectiveBadge objective={problem.objective} />
          <code style={{ fontSize: '12px', fontFamily: 'var(--font-mono)', color: 'var(--color-text-tertiary)' }}>
            {problem.id}
          </code>
        </div>
        <h1 style={{ marginBottom: 'var(--space-3)' }}>{problem.name}</h1>
        <p className="text-secondary" style={{ marginBottom: 'var(--space-6)' }}>
          {problem.description}
        </p>

        <div
          style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: 'var(--space-4)', marginBottom: 'var(--space-6)' }}
        >
          <div className="card">
            <div className="card-header"><h2>Greedy Strategies</h2></div>
            <div className="card-body" style={{ padding: 0 }}>
              <table className="data-table" aria-label="Available greedy strategies">
                <thead>
                  <tr>
                    <th scope="col">Strategy</th>
                    <th scope="col">ID</th>
                    <th scope="col">Default</th>
                  </tr>
                </thead>
                <tbody>
                  {problem.strategies.map(s => (
                    <tr key={s.id}>
                      <td>{s.name}</td>
                      <td className="mono text-sm">{s.id}</td>
                      <td>{s.default ? <span className="badge badge-primary">Default</span> : '—'}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </div>

          <div className="card">
            <div className="card-header"><h2>Ground-Truth Oracles</h2></div>
            <div className="card-body" style={{ padding: 0 }}>
              <table className="data-table" aria-label="Available oracles">
                <thead>
                  <tr>
                    <th scope="col">Oracle</th>
                    <th scope="col">Confidence</th>
                    <th scope="col">Default</th>
                  </tr>
                </thead>
                <tbody>
                  {problem.oracles.map(o => (
                    <tr key={o.id}>
                      <td>{o.name}</td>
                      <td>
                        <span className="badge badge-success">{o.confidence}</span>
                      </td>
                      <td>{o.default ? <span className="badge badge-primary">Default</span> : '—'}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </div>
        </div>

        <Link to="/experiments/new" className="btn btn-primary">
          + Run Experiment on {problem.name}
        </Link>
      </div>
    </main>
  );
}
