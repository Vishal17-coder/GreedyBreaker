import React from 'react';
import type { AlgorithmTrace } from '../types/api';

interface TraceTableProps {
  greedyTrace?: AlgorithmTrace | null;
  optimalTrace?: AlgorithmTrace | null;
  divergenceStep?: number | null;
}

export const TraceTable: React.FC<TraceTableProps> = ({
  greedyTrace,
  optimalTrace,
  divergenceStep,
}) => {
  if (!greedyTrace && !optimalTrace) {
    return null;
  }

  return (
    <div style={{ display: 'flex', flexDirection: 'column', gap: '1.5rem' }}>
      {greedyTrace && (
        <div className="panel">
          <div className="panel-header">
            <div className="panel-title">
              <span style={{ color: 'var(--status-failure)' }}>●</span>
              <span>Greedy Solver Execution Trace</span>
              <span className="badge badge-neutral" style={{ marginLeft: '0.5rem' }}>
                {greedyTrace.strategy_name}
              </span>
            </div>
            <div style={{ fontSize: '0.85rem', color: 'var(--text-muted)' }}>
              Final Cost: <span className="font-mono" style={{ fontWeight: 700, color: 'var(--text-primary)' }}>{greedyTrace.final_cost ?? 'N/A'}</span>
            </div>
          </div>

          <div className="table-container">
            <table className="data-table">
              <thead>
                <tr>
                  <th>Step</th>
                  <th>State Description</th>
                  <th>Remaining</th>
                  <th>Choices Evaluated</th>
                  <th>Choice Made</th>
                  <th>Outcome / Rationale</th>
                </tr>
              </thead>
              <tbody>
                {greedyTrace.steps.map((step) => {
                  const isDivergence = divergenceStep === step.step_number;
                  return (
                    <tr key={step.step_number} className={isDivergence ? 'trace-diverged' : ''}>
                      <td className="font-mono">
                        {step.step_number} {isDivergence && <span title="First divergence point">⚠</span>}
                      </td>
                      <td>{step.state_description}</td>
                      <td className="font-mono">{step.remaining_target}</td>
                      <td className="font-mono" style={{ fontSize: '0.8rem' }}>
                        [{step.available_choices.join(', ')}]
                      </td>
                      <td className="font-mono" style={{ fontWeight: 600 }}>
                        {step.selected_choice !== null ? step.selected_choice : '—'}
                      </td>
                      <td>
                        <span style={{ fontSize: '0.825rem', color: 'var(--text-secondary)' }}>
                          {step.rationale || step.outcome_description}
                        </span>
                      </td>
                    </tr>
                  );
                })}
              </tbody>
            </table>
          </div>
        </div>
      )}

      {optimalTrace && (
        <div className="panel">
          <div className="panel-header">
            <div className="panel-title">
              <span style={{ color: 'var(--accent-primary)' }}>●</span>
              <span>Optimal Oracle Execution Trace</span>
              <span className="badge badge-info" style={{ marginLeft: '0.5rem' }}>
                {optimalTrace.strategy_name}
              </span>
            </div>
            <div style={{ fontSize: '0.85rem', color: 'var(--text-muted)' }}>
              Final Cost: <span className="font-mono" style={{ fontWeight: 700, color: 'var(--text-primary)' }}>{optimalTrace.final_cost ?? 'N/A'}</span>
            </div>
          </div>

          <div className="table-container">
            <table className="data-table">
              <thead>
                <tr>
                  <th>Step</th>
                  <th>State Description</th>
                  <th>Remaining</th>
                  <th>Available Choices</th>
                  <th>Choice Made</th>
                  <th>Outcome / Rationale</th>
                </tr>
              </thead>
              <tbody>
                {optimalTrace.steps.map((step) => (
                  <tr key={step.step_number}>
                    <td className="font-mono">{step.step_number}</td>
                    <td>{step.state_description}</td>
                    <td className="font-mono">{step.remaining_target}</td>
                    <td className="font-mono" style={{ fontSize: '0.8rem' }}>
                      [{step.available_choices.join(', ')}]
                    </td>
                    <td className="font-mono" style={{ fontWeight: 600, color: 'var(--accent-primary)' }}>
                      {step.selected_choice !== null ? step.selected_choice : '—'}
                    </td>
                    <td>
                      <span style={{ fontSize: '0.825rem', color: 'var(--text-secondary)' }}>
                        {step.rationale || step.outcome_description}
                      </span>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </div>
      )}
    </div>
  );
};
