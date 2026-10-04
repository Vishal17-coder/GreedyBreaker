import React, { useEffect, useState } from 'react';
import type { ProblemMetadata } from '../types/api';
import { getProblems } from '../api/problems';
import { CheckCircle, AlertTriangle } from 'lucide-react';

export const ComplexityCenterPage: React.FC = () => {
  const [problems, setProblems] = useState<ProblemMetadata[]>([]);
  const [loading, setLoading] = useState<boolean>(true);

  useEffect(() => {
    let mounted = true;
    const fetchMeta = async () => {
      try {
        const data = await getProblems();
        if (mounted) {
          setProblems(data);
          setLoading(false);
        }
      } catch (err) {
        if (mounted) setLoading(false);
      }
    };
    fetchMeta();
    return () => {
      mounted = false;
    };
  }, []);

  return (
    <div style={{ display: 'flex', flexDirection: 'column', gap: '2.5rem' }}>
      <div>
        <h1 style={{ fontSize: '2rem', marginBottom: '0.4rem' }}>Complexity Center</h1>
        <p style={{ color: 'var(--text-muted)', fontSize: '1.05rem', margin: 0 }}>
          Formal asymptotic analysis and structural mathematical requirements (Greedy-Choice Property, Optimal Substructure, Matroid Theory) across problem families.
        </p>
      </div>

      {/* DAA Summary Grid */}
      <div className="grid-2">
        <div
          className="panel"
          style={{
            borderLeft: '3px solid var(--status-success)',
            background: 'var(--status-success-bg)',
          }}
        >
          <div className="panel-header" style={{ borderBottomColor: 'var(--status-success-border)' }}>
            <div className="panel-title" style={{ color: 'var(--status-success-text)' }}>
              <CheckCircle size={18} />
              <span>Provably Optimal Greedy Families</span>
            </div>
            <span className="badge badge-success">Matroid Structure</span>
          </div>
          <p style={{ fontSize: '0.9rem', color: 'var(--status-success-text)', marginBottom: '0.5rem' }}>
            <strong>Activity Selection</strong> and <strong>Fractional Knapsack</strong> satisfy the <em>greedy-choice property</em> globally.
          </p>
          <ul style={{ fontSize: '0.85rem', color: 'var(--text-secondary)', paddingLeft: '1.25rem', margin: '0.5rem 0' }}>
            <li style={{ marginBottom: '0.5rem' }}>
              <strong>Activity Selection:</strong> Forms an independent system over interval graphs. Earliest finish time (EFT) guarantees maximum compatible subset (CLRS Theorem 16.1).
            </li>
            <li>
              <strong>Fractional Knapsack:</strong> Admits continuous divisibility; sorting by value-to-weight density ($v_i / w_i$) is optimal via exchange argument.
            </li>
          </ul>
        </div>

        <div
          className="panel"
          style={{
            borderLeft: '3px solid var(--status-failure)',
            background: 'var(--status-failure-bg)',
          }}
        >
          <div className="panel-header" style={{ borderBottomColor: 'var(--status-failure-border)' }}>
            <div className="panel-title" style={{ color: 'var(--status-failure-text)' }}>
              <AlertTriangle size={18} />
              <span>Fragile / Breaker Greedy Families</span>
            </div>
            <span className="badge badge-failure">Requires Exact DP</span>
          </div>
          <p style={{ fontSize: '0.9rem', color: 'var(--status-failure-text)', marginBottom: '0.5rem' }}>
            <strong>Coin Change</strong> and <strong>0/1 Knapsack</strong> exhibit <em>optimal substructure</em>, but fail the <em>greedy-choice property</em>.
          </p>
          <ul style={{ fontSize: '0.85rem', color: 'var(--text-secondary)', paddingLeft: '1.25rem', margin: '0.5rem 0' }}>
            <li style={{ marginBottom: '0.5rem' }}>
              <strong>Coin Change:</strong> Non-canonical systems (e.g. &#123;1, 3, 4&#125; for target 6) trap greedy into picking 4 (yielding 4+1+1=3 coins) instead of optimal 3+3=2 coins.
            </li>
            <li>
              <strong>0/1 Knapsack:</strong> Discrete all-or-nothing constraint means high density items can leave unusable capacity gaps, leading to severe suboptimality.
            </li>
          </ul>
        </div>
      </div>

      {/* Solver Complexity Comparison Tables */}
      <div style={{ display: 'flex', flexDirection: 'column', gap: '1.75rem' }}>
        <h2 style={{ fontSize: '1.4rem' }}>Algorithm Complexity Catalog (Engine Authoritative)</h2>

        {loading ? (
          <div style={{ padding: '2rem', textAlign: 'center', color: 'var(--text-muted)' }}>
            <div className="spinner" style={{ marginRight: '0.5rem' }} /> Loading theoretical complexity specs...
          </div>
        ) : (
          problems.map((problem) => (
            <div key={problem.id} className="panel">
              <div className="panel-header">
                <div>
                  <div className="panel-title">{problem.name}</div>
                  <div className="panel-subtitle">{problem.description}</div>
                </div>
                <div style={{ display: 'flex', gap: '0.5rem', alignItems: 'center' }}>
                  <span className="badge badge-neutral">{problem.objective}</span>
                  {problem.can_fail ? (
                    <span className="badge badge-failure">Greedy Breachable</span>
                  ) : (
                    <span className="badge badge-success">Greedy Optimal</span>
                  )}
                </div>
              </div>

              <div className="table-container">
                <table className="data-table">
                  <thead>
                    <tr>
                      <th>Algorithm</th>
                      <th>Time (Best)</th>
                      <th>Time (Average)</th>
                      <th>Time (Worst)</th>
                      <th>Space (Worst)</th>
                      <th>DAA Theoretical Role</th>
                    </tr>
                  </thead>
                  <tbody>
                    {problem.solvers.map((solver, idx) => (
                      <tr key={idx}>
                        <td style={{ fontWeight: 600 }}>{solver.algorithm_name}</td>
                        <td className="font-mono">{solver.time_best}</td>
                        <td className="font-mono">{solver.time_average}</td>
                        <td className="font-mono" style={{ color: solver.algorithm_name.includes('Exact') ? 'var(--accent-primary)' : 'var(--status-success)', fontWeight: 600 }}>
                          {solver.time_worst}
                        </td>
                        <td className="font-mono">{solver.space_worst}</td>
                        <td style={{ fontSize: '0.85rem' }}>{solver.explanation}</td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            </div>
          ))
        )}
      </div>
    </div>
  );
};
