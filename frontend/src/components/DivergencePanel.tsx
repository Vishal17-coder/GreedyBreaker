import React from 'react';
import type { DivergenceResult } from '../types/api';

interface DivergencePanelProps {
  divergence: DivergenceResult | null;
}

export const DivergencePanel: React.FC<DivergencePanelProps> = ({ divergence }) => {
  if (!divergence || !divergence.has_diverged) {
    return (
      <div
        className="panel"
        style={{
          borderLeft: '3px solid var(--status-success)',
          background: 'var(--status-success-bg)',
          padding: '1.25rem 1.5rem',
        }}
      >
        <div style={{ fontWeight: 600, color: 'var(--status-success-text)', marginBottom: '0.25rem', fontSize: '0.95rem' }}>
          No Algorithmic Divergence
        </div>
        <div style={{ color: 'var(--text-secondary)', fontSize: '0.875rem' }}>
          {divergence?.explanation ||
            'The greedy choice sequence aligned identically with the optimal dynamic programming decisions across all decision points.'}
        </div>
      </div>
    );
  }

  return (
    <div
      className="panel"
      style={{
        borderLeft: '3px solid var(--status-failure)',
        background: '#ffffff',
      }}
    >
      <div className="panel-header" style={{ paddingBottom: '0.65rem', marginBottom: '1rem' }}>
        <div className="panel-title" style={{ color: 'var(--status-failure)' }}>
          <span>First-Divergence Point</span>
        </div>
        <span className="badge badge-failure">Step {divergence.divergence_step ?? 1}</span>
      </div>

      <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))', gap: '1rem', marginBottom: '1.25rem' }}>
        <div style={{ background: 'var(--bg-subtle)', padding: '0.85rem 1rem', borderRadius: 'var(--radius-sm)', border: '1px solid var(--border-subtle)' }}>
          <div style={{ fontSize: '0.75rem', color: 'var(--text-muted)', fontWeight: 600, textTransform: 'uppercase' }}>State at Divergence</div>
          <div className="font-mono" style={{ fontSize: '1.15rem', fontWeight: 700, color: 'var(--text-primary)', marginTop: '0.2rem' }}>
            Remaining: {divergence.state_remaining}
          </div>
        </div>

        <div style={{ background: 'var(--status-failure-bg)', padding: '0.85rem 1rem', borderRadius: 'var(--radius-sm)', border: '1px solid var(--status-failure-border)' }}>
          <div style={{ fontSize: '0.75rem', color: 'var(--status-failure-text)', fontWeight: 600, textTransform: 'uppercase' }}>Greedy Decision</div>
          <div className="font-mono" style={{ fontSize: '1.15rem', fontWeight: 700, color: 'var(--status-failure)', marginTop: '0.2rem' }}>
            Selected: {divergence.greedy_choice ?? 'None'}
          </div>
          {divergence.greedy_projected_cost !== null && (
            <div style={{ fontSize: '0.75rem', color: 'var(--status-failure-text)', marginTop: '0.25rem' }}>
              Projected Total Cost: {divergence.greedy_projected_cost}
            </div>
          )}
        </div>

        <div style={{ background: 'var(--status-success-bg)', padding: '0.85rem 1rem', borderRadius: 'var(--radius-sm)', border: '1px solid var(--status-success-border)' }}>
          <div style={{ fontSize: '0.75rem', color: 'var(--status-success-text)', fontWeight: 600, textTransform: 'uppercase' }}>Optimal Decisions</div>
          <div className="font-mono" style={{ fontSize: '1.15rem', fontWeight: 700, color: 'var(--status-success)', marginTop: '0.2rem' }}>
            Available: [{divergence.optimal_choices?.join(', ') || 'None'}]
          </div>
          {divergence.optimal_projected_cost !== null && (
            <div style={{ fontSize: '0.75rem', color: 'var(--status-success-text)', marginTop: '0.25rem' }}>
              Optimal Total Cost: {divergence.optimal_projected_cost}
            </div>
          )}
        </div>
      </div>

      <div style={{ background: 'var(--bg-subtle)', padding: '1rem 1.25rem', borderRadius: 'var(--radius-sm)', border: '1px solid var(--border-subtle)' }}>
        <div style={{ fontSize: '0.75rem', fontWeight: 700, color: 'var(--text-muted)', textTransform: 'uppercase', marginBottom: '0.35rem', letterSpacing: '0.04em' }}>
          Forensic Explanation
        </div>
        <div style={{ color: 'var(--text-secondary)', fontSize: '0.9rem', lineHeight: '1.6' }}>
          {divergence.explanation}
        </div>
      </div>
    </div>
  );
};
