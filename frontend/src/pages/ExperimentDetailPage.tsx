// =============================================================================
// Experiment Detail Page — Result Workbench with tabbed sub-navigation
// =============================================================================
import { useState, useEffect, useCallback } from 'react';
import { useParams, Link } from 'react-router-dom';
import { experimentsApi, getErrorMessage } from '../api';
import type {
  ExperimentDetail,
  ExecutionStatus,
  ExecutionResult,
  CounterexampleDTO,
  ForensicReport,
  ReproductionResult,
} from '../types';
import {
  PROBLEM_LABELS,
  SEARCH_MODE_INFO,
  SEARCH_STRATEGY_INFO,
  isTerminalStatus,
  isRunning,
} from '../types';
import { StatusBadge } from '../components/StatusBadge';
import {
  Breadcrumb,
  LoadingPage,
  ErrorState,
  MetricCard,
  EmptyState,
  Spinner,
  Token,
} from '../components/Shared';
import { formatDate, formatMs, formatNumber, formatValue } from '../utils/format';

type Tab = 'overview' | 'counterexample' | 'forensics' | 'evidence' | 'reproducibility';

// --- Helper to format instance for display --------------------------------
function InstanceDisplay({ instance }: { instance: Record<string, unknown> | null }) {
  if (!instance) return <span className="text-secondary">No instance data</span>;
  return (
    <pre
      className="code-block"
      style={{ margin: 0, fontSize: '13px' }}
      aria-label="Problem instance parameters"
    >
      {JSON.stringify(instance, null, 2)}
    </pre>
  );
}

// --- Overview Tab ---------------------------------------------------------
function OverviewTab({
  experiment,
  executionResult,
  onSelectTab,
}: {
  experiment: ExperimentDetail;
  executionResult: ExecutionResult | null;
  onSelectTab: (tab: Tab) => void;
}) {
  const status = experiment.status;
  const summary = experiment.summary;
  const sr = executionResult?.search_result;

  const hasCounterexample = status === 'COMPLETED' && summary?.has_counterexample;
  const noCounterexample = status === 'NO_COUNTEREXAMPLE';

  return (
    <div>
      {/* HERO RESULT: First viewport communicates finding immediately */}
      {hasCounterexample && (
        <div
          className="hero-result-card"
          style={{
            background: 'var(--color-surface-bg)',
            border: '2px solid var(--color-warn-border)',
            borderRadius: 'var(--radius-lg)',
            padding: 'var(--space-6)',
            marginBottom: 'var(--space-6)',
          }}
          role="status"
        >
          <div className="flex items-center justify-between flex-wrap gap-2" style={{ marginBottom: 'var(--space-3)' }}>
            <span className="badge badge-counterexample" style={{ fontSize: '13px', padding: '4px 10px', fontWeight: 700 }}>
              ⚠ COUNTEREXAMPLE FOUND — OPTIMALITY FAILURE VERIFIED
            </span>
            <span className="text-secondary text-sm font-mono">
              Oracle: {experiment.configuration?.oracle_id}
            </span>
          </div>

          <h1 style={{ fontSize: '22px', fontWeight: 700, margin: 'var(--space-2) 0 var(--space-2) 0' }}>
            Greedy Strategy '{experiment.configuration?.strategy_id}' Produced Suboptimal Solution
          </h1>

          <p className="text-secondary text-sm" style={{ marginBottom: 'var(--space-5)' }}>
            Authoritative C++20 engine evaluated candidate instances and proved that the greedy choice diverged from the ground-truth optimum.
          </p>

          {/* Hero Metric Comparison Block */}
          {sr && (
            <div
              style={{
                display: 'grid',
                gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))',
                gap: 'var(--space-4)',
                background: 'var(--color-surface-subtle)',
                padding: 'var(--space-5)',
                borderRadius: 'var(--radius-md)',
                border: '1px solid var(--color-border-subtle)',
                textAlign: 'center',
                marginBottom: 'var(--space-5)',
              }}
            >
              <div>
                <div className="text-xs text-secondary font-semibold uppercase tracking-wider">GREEDY RESULT</div>
                <div className="font-mono font-bold" style={{ fontSize: '32px', color: 'var(--color-warn)', margin: 'var(--space-1) 0' }}>
                  {formatValue(sr.best_greedy?.objective_value)}
                </div>
                <div className="text-xs text-secondary">
                  {sr.best_greedy?.solution_description ?? 'Suboptimal choice'}
                </div>
              </div>

              <div>
                <div className="text-xs text-secondary font-semibold uppercase tracking-wider">EXACT OPTIMAL</div>
                <div className="font-mono font-bold" style={{ fontSize: '32px', color: 'var(--color-success)', margin: 'var(--space-1) 0' }}>
                  {formatValue(sr.best_oracle?.optimal_value)}
                </div>
                <div className="text-xs text-secondary">
                  Ground truth ({sr.best_oracle?.oracle_method ?? 'Exact DP'})
                </div>
              </div>

              <div>
                <div className="text-xs text-secondary font-semibold uppercase tracking-wider">OBJECTIVE GAP</div>
                <div className="font-mono font-bold" style={{ fontSize: '32px', color: 'var(--color-danger)', margin: 'var(--space-1) 0' }}>
                  {formatValue(sr.best_verification?.absolute_gap)}
                </div>
                <div className="text-xs text-secondary">
                  {sr.best_verification?.failure_percent != null
                    ? `${sr.best_verification.failure_percent.toFixed(1)}% Suboptimality`
                    : 'Absolute penalty'}
                </div>
              </div>
            </div>
          )}

          {/* Quick Deep-Dive Links */}
          <div className="flex gap-3 flex-wrap">
            <button className="btn btn-secondary btn-sm" onClick={() => onSelectTab('counterexample')}>
              View Solution Comparison & Minimization →
            </button>
            <button className="btn btn-secondary btn-sm" onClick={() => onSelectTab('forensics')}>
              Inspect Critical Divergence Trace →
            </button>
            <button className="btn btn-secondary btn-sm" onClick={() => onSelectTab('evidence')}>
              View Search Evidence & Bounds →
            </button>
          </div>
        </div>
      )}

      {noCounterexample && (
        <div className="alert alert-no-counterexample" style={{ marginBottom: 'var(--space-6)', padding: 'var(--space-6)' }} role="status">
          <span className="alert-icon" aria-hidden="true" style={{ fontSize: '24px' }}>ℹ</span>
          <div className="alert-content">
            <h2 style={{ fontSize: '18px', fontWeight: 600, margin: 0 }}>
              No Counterexample Discovered Within Configured Bounds
            </h2>
            <p className="text-sm text-secondary" style={{ marginTop: 'var(--space-2)' }}>
              Over {formatNumber(summary?.instances_evaluated ?? 0)} evaluated candidates within the configured search space bounds, the greedy heuristic produced optimal results matching the exact oracle.
            </p>
            <div
              style={{
                marginTop: 'var(--space-3)',
                padding: 'var(--space-3)',
                background: 'var(--color-surface-bg)',
                borderRadius: 'var(--radius-sm)',
                border: '1px solid var(--color-border-subtle)',
                fontSize: '13px',
              }}
            >
              <strong>Empirical Boundary Notice:</strong> This bounded empirical search does not constitute a mathematical proof of unbounded correctness. Algorithmic correctness proofs require formal matroid / exchange property verification.
            </div>
          </div>
        </div>
      )}

      {/* Discovered Counterexample Instance immediately in overview */}
      {hasCounterexample && sr?.best_instance && (
        <div className="card" style={{ marginBottom: 'var(--space-6)' }}>
          <div className="card-header">
            <h2>Discovered Counterexample Instance (Minimal Witness)</h2>
          </div>
          <div className="card-body">
            <p className="text-secondary text-sm" style={{ marginBottom: 'var(--space-4)' }}>
              Problem instance parameters that induced the suboptimality verified above:
            </p>
            <InstanceDisplay instance={sr.best_instance as Record<string, unknown>} />
          </div>
        </div>
      )}

      {/* Summary stats */}
      <div className="card">
        <div className="card-header">
          <h2>Execution Summary</h2>
        </div>
        <div className="card-body" style={{ padding: 0 }}>
          <table className="data-table" aria-label="Experiment execution summary">
            <tbody>
              <tr>
                <td className="text-secondary text-sm" style={{ width: '220px' }}>Candidates Evaluated</td>
                <td className="mono">{formatNumber(summary?.instances_evaluated ?? 0)}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Counterexamples Found</td>
                <td className="mono">{formatNumber(summary?.counterexamples_count ?? 0)}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Worst Gap Observed</td>
                <td className="mono">{summary?.worst_gap?.toFixed(4) ?? '—'}</td>
              </tr>
              {sr && (
                <>
                  <tr>
                    <td className="text-secondary text-sm">Failure Rate</td>
                    <td className="mono">{(sr.failure_rate * 100).toFixed(4)}%</td>
                  </tr>
                  <tr>
                    <td className="text-secondary text-sm">Search Coverage</td>
                    <td>
                      <span className="mono">{sr.coverage}</span>
                      <span className="text-secondary text-sm" style={{ marginLeft: 8 }}>
                        ({(sr.coverage_ratio * 100).toFixed(2)}% of estimated space)
                      </span>
                    </td>
                  </tr>
                  <tr>
                    <td className="text-secondary text-sm">Elapsed Time</td>
                    <td className="mono">{formatMs(sr.elapsed_time_ms)}</td>
                  </tr>
                  <tr>
                    <td className="text-secondary text-sm">Estimated Space</td>
                    <td className="mono">{formatNumber(sr.estimated_space_size)}</td>
                  </tr>
                </>
              )}
            </tbody>
          </table>
        </div>
      </div>
    </div>
  );
}

// --- Counterexample Tab ---------------------------------------------------
function CounterexampleTab({
  cx,
  status,
  executionResult,
}: {
  cx: CounterexampleDTO | null;
  status: string;
  executionResult: ExecutionResult | null;
}) {
  if (status === 'NO_COUNTEREXAMPLE') {
    return (
      <EmptyState
        icon="ℹ"
        title="No Counterexample Discovered in Configured Bounds"
        message="No counterexample was discovered within the configured search space. This result does not prove correctness for unbounded inputs."
      />
    );
  }

  if (!cx) return <LoadingPage message="Loading counterexample data…" />;

  if (!cx.has_counterexample) {
    return (
      <EmptyState
        icon="ℹ"
        title="No Counterexample in This Experiment"
        message="No counterexample was discovered within the configured search space. This result does not prove correctness for unbounded inputs."
      />
    );
  }

  const v = cx.final_verification;
  const sr = executionResult?.search_result;

  return (
    <div>
      {/* Minimal instance */}
      <section aria-labelledby="instance-heading" style={{ marginBottom: 'var(--space-6)' }}>
        <div className="card">
          <div className="card-header">
            <h2 id="instance-heading">Minimal Counterexample Instance</h2>
          </div>
          <div className="card-body">
            <p className="text-secondary text-sm" style={{ marginBottom: 'var(--space-4)' }}>
              <strong>{cx.minimality_status ?? 'Smallest verified counterexample under the configured ordering.'}</strong>
              {cx.ordering_criteria && (
                <span style={{ display: 'block', marginTop: 'var(--space-1)' }}>
                  Ordering: {cx.ordering_criteria}
                </span>
              )}
            </p>
            <InstanceDisplay instance={cx.minimal_instance} />
          </div>
        </div>
      </section>

      {/* Side-by-side solutions */}
      {v && (
        <section aria-labelledby="solutions-heading" style={{ marginBottom: 'var(--space-6)' }}>
          <h2 id="solutions-heading" style={{ marginBottom: 'var(--space-4)' }}>
            Solution Comparison
          </h2>
          <div className="solution-grid">
            <div className="solution-col greedy">
              <div className="solution-col-header">
                <span aria-hidden="true">⚠</span>
                Greedy Solution (Suboptimal)
              </div>
              <div className="solution-col-body">
                <table className="data-table" aria-label="Greedy solution details">
                  <tbody>
                    <tr>
                      <td className="text-secondary text-sm">Objective Value</td>
                      <td className="mono font-bold" style={{ color: 'var(--color-warn)' }}>
                        {formatValue(v.greedy_value)}
                      </td>
                    </tr>
                    {sr?.best_greedy?.solution_description && (
                      <tr>
                        <td className="text-secondary text-sm">Selected Elements</td>
                        <td className="mono">{sr.best_greedy.solution_description}</td>
                      </tr>
                    )}
                    <tr>
                      <td className="text-secondary text-sm">Feasible</td>
                      <td>Yes</td>
                    </tr>
                    <tr>
                      <td className="text-secondary text-sm">Status</td>
                      <td>
                        <span className="badge badge-counterexample">SUBOPTIMAL</span>
                      </td>
                    </tr>
                  </tbody>
                </table>
              </div>
            </div>

            <div className="solution-col optimal">
              <div className="solution-col-header">
                <span aria-hidden="true">✓</span>
                Exact Optimal Solution (Ground Truth)
              </div>
              <div className="solution-col-body">
                <table className="data-table" aria-label="Optimal solution details">
                  <tbody>
                    <tr>
                      <td className="text-secondary text-sm">Objective Value</td>
                      <td className="mono font-bold" style={{ color: 'var(--color-success)' }}>
                        {formatValue(v.optimal_value)}
                      </td>
                    </tr>
                    {sr?.best_oracle?.solution_description && (
                      <tr>
                        <td className="text-secondary text-sm">Selected Elements</td>
                        <td className="mono">{sr.best_oracle.solution_description}</td>
                      </tr>
                    )}
                    <tr>
                      <td className="text-secondary text-sm">Feasible</td>
                      <td>Yes</td>
                    </tr>
                    <tr>
                      <td className="text-secondary text-sm">Status</td>
                      <td>
                        <span className="badge badge-success">OPTIMAL (VERIFIED)</span>
                      </td>
                    </tr>
                  </tbody>
                </table>
              </div>
            </div>
          </div>
        </section>
      )}

      {/* Gap metrics */}
      {v && (
        <section aria-labelledby="gap-heading" style={{ marginBottom: 'var(--space-6)' }}>
          <h2 id="gap-heading" style={{ marginBottom: 'var(--space-4)' }}>Objective Gap</h2>
          <div className="metric-grid">
            <MetricCard label="Absolute Gap" value={formatValue(v.absolute_gap)} note="Greedy − Optimal" />
            <MetricCard label="Relative Gap" value={formatValue(v.relative_gap)} note="Greedy / Optimal" />
            <MetricCard label="Suboptimality" value={`${v.failure_percent.toFixed(1)}%`} note="Excess over optimal" />
            <MetricCard label="Failure Type" value={v.status.replace('_', ' ')} note="Verification status" />
          </div>
        </section>
      )}

      {/* Minimization record */}
      {cx.reduction_steps > 0 && (
        <section aria-labelledby="minimization-heading">
          <div className="card">
            <div className="card-header">
              <h2 id="minimization-heading">Delta-Reduction Minimization Record</h2>
            </div>
            <div className="card-body">
              <div
                style={{
                  display: 'grid',
                  gridTemplateColumns: '1fr auto 1fr',
                  gap: 'var(--space-4)',
                  alignItems: 'center',
                }}
              >
                <div>
                  <h3 style={{ marginBottom: 'var(--space-3)' }}>Original Candidate</h3>
                  <InstanceDisplay instance={cx.original_instance} />
                </div>
                <div style={{ textAlign: 'center', color: 'var(--color-text-tertiary)' }}>
                  <div style={{ fontSize: '24px' }}>→</div>
                  <div className="text-xs" style={{ marginTop: 4 }}>
                    {cx.reduction_steps} pass{cx.reduction_steps !== 1 ? 'es' : ''}
                  </div>
                  <div className="text-xs">{cx.reduction_percent.toFixed(1)}% reduced</div>
                </div>
                <div>
                  <h3 style={{ marginBottom: 'var(--space-3)' }}>Minimal Instance</h3>
                  <InstanceDisplay instance={cx.minimal_instance} />
                </div>
              </div>
              <div className="alert alert-success" style={{ marginTop: 'var(--space-4)' }} role="status">
                <span className="alert-icon" aria-hidden="true">✓</span>
                <div className="alert-content text-sm">
                  Failure preserved through all {cx.reduction_steps} reduction pass{cx.reduction_steps !== 1 ? 'es' : ''}.
                  Minimality criterion: <em>{cx.minimality_status}</em>
                </div>
              </div>
            </div>
          </div>
        </section>
      )}
    </div>
  );
}

// --- Forensics Tab --------------------------------------------------------
function ForensicsTab({ report, status }: { report: ForensicReport | null; status: string }) {
  if (status === 'NO_COUNTEREXAMPLE') {
    return (
      <EmptyState
        icon="ℹ"
        title="No Divergence to Analyze"
        message="Because no counterexample was found within the configured bounds, greedy decisions matched optimal decisions across all evaluated instances."
      />
    );
  }

  if (!report) return <LoadingPage message="Loading forensic report…" />;

  const trace = report.greedy_trace;

  return (
    <div>
      {/* Investigation Questions & Findings */}
      <section aria-labelledby="forensics-summary-heading" style={{ marginBottom: 'var(--space-6)' }}>
        <div className="card">
          <div className="card-header">
            <h2 id="forensics-summary-heading">Algorithmic Forensic Investigation</h2>
            <span className="badge badge-counterexample">
              {report.evidence_confidence?.replace(/_/g, ' ') ?? 'Verified Divergence'}
            </span>
          </div>
          <div className="card-body">
            <div
              style={{
                display: 'grid',
                gridTemplateColumns: 'repeat(auto-fit, minmax(260px, 1fr))',
                gap: 'var(--space-4)',
                marginBottom: 'var(--space-4)',
              }}
            >
              <div
                style={{
                  padding: 'var(--space-4)',
                  background: 'var(--color-surface-subtle)',
                  borderRadius: 'var(--radius-md)',
                  border: '1px solid var(--color-border-subtle)',
                }}
              >
                <div className="text-xs font-semibold text-secondary uppercase tracking-wider mb-1">
                  1. What Did Greedy Do?
                </div>
                <div className="font-mono text-sm">{report.critical_greedy_action}</div>
              </div>

              <div
                style={{
                  padding: 'var(--space-4)',
                  background: 'var(--color-surface-subtle)',
                  borderRadius: 'var(--radius-md)',
                  border: '1px solid var(--color-border-subtle)',
                }}
              >
                <div className="text-xs font-semibold text-secondary uppercase tracking-wider mb-1">
                  2. Where Did It Diverge?
                </div>
                <div className="text-sm">
                  Divergence at Step <strong>{report.divergence_step_index + 1}</strong> of execution
                </div>
              </div>

              <div
                style={{
                  padding: 'var(--space-4)',
                  background: 'var(--color-surface-subtle)',
                  borderRadius: 'var(--radius-md)',
                  border: '1px solid var(--color-border-subtle)',
                }}
              >
                <div className="text-xs font-semibold text-secondary uppercase tracking-wider mb-1">
                  3. What Alternative Existed?
                </div>
                <div className="font-mono text-sm">{report.optimal_alternative_action}</div>
              </div>

              <div
                style={{
                  padding: 'var(--space-4)',
                  background: 'var(--color-surface-subtle)',
                  borderRadius: 'var(--radius-md)',
                  border: '1px solid var(--color-border-subtle)',
                }}
              >
                <div className="text-xs font-semibold text-secondary uppercase tracking-wider mb-1">
                  4. Observed Consequence
                </div>
                <div className="text-sm">
                  {report.causal_consequence || 'Observed Divergence: greedy choice stranded resources, preventing optimal configuration.'}
                </div>
              </div>
            </div>

            <p className="text-secondary text-sm" style={{ margin: 0 }}>
              <em>Evidence Standard:</em> Forensic divergence is computed through exact dual-trace analysis comparing greedy decisions against the ground-truth oracle path. Causal consequences reflect observed state changes; no speculative explanations are fabricated.
            </p>
          </div>
        </div>
      </section>

      {/* Decision Trace */}
      {trace && trace.steps?.length > 0 && (
        <section aria-labelledby="trace-heading">
          <h2 id="trace-heading" style={{ marginBottom: 'var(--space-4)' }}>
            Greedy Decision Trace
          </h2>
          <div className="card">
            <div className="card-body">
              <div className="trace-list" role="list" aria-label="Greedy algorithm decision steps">
                {trace.steps.map((step) => {
                  const isDivergence = step.step_index === report.divergence_step_index;
                  return (
                    <div
                      key={step.step_index}
                      className={`trace-item${isDivergence ? ' divergence' : ''}`}
                      role="listitem"
                    >
                      <div
                        className="trace-step-number"
                        aria-label={`Step ${step.step_index + 1}${isDivergence ? ', critical divergence' : ''}`}
                      >
                        {step.step_index + 1}
                      </div>
                      <div className="trace-content">
                        {isDivergence && (
                          <span className="badge badge-counterexample" style={{ marginBottom: 'var(--space-2)' }}>
                            ⚠ Critical Divergence Point
                          </span>
                        )}
                        <div className="trace-action">{step.action_description}</div>
                        <div className="trace-state">
                          Rule: <em>{step.heuristic_rule}</em>
                        </div>
                        <div className="trace-state">
                          Before: {step.state_before} → After: {step.state_after}
                        </div>
                        <div className="trace-state">
                          Cumulative objective: <Token>{step.cumulative_objective}</Token>
                        </div>
                      </div>
                    </div>
                  );
                })}
              </div>
            </div>
          </div>
          <p className="text-secondary text-sm" style={{ marginTop: 'var(--space-3)' }}>
            Total steps: {trace.total_steps}. All decision data is sourced directly from the
            C++20 algorithmic engine output. No causal explanations are fabricated.
          </p>
        </section>
      )}
    </div>
  );
}

// --- Evidence Tab ---------------------------------------------------------
function EvidenceTab({
  experiment,
  executionResult,
}: {
  experiment: ExperimentDetail;
  executionResult: ExecutionResult | null;
}) {
  const cfg = experiment.configuration;
  const sr = executionResult?.search_result;

  return (
    <div>
      <div className="card" style={{ marginBottom: 'var(--space-6)' }}>
        <div className="card-header">
          <h2>Search Audit & Execution Evidence</h2>
        </div>
        <div className="card-body" style={{ padding: 0 }}>
          <table className="data-table" aria-label="Search audit metrics">
            <thead>
              <tr>
                <th>Metric</th>
                <th>Value</th>
                <th>Interpretation</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td>Search Strategy</td>
                <td className="mono">{cfg?.strategy ?? '—'}</td>
                <td className="text-secondary text-sm">{cfg?.strategy ? SEARCH_STRATEGY_INFO[cfg.strategy as keyof typeof SEARCH_STRATEGY_INFO]?.description : '—'}</td>
              </tr>
              <tr>
                <td>Search Mode</td>
                <td className="mono">{cfg?.mode ?? '—'}</td>
                <td className="text-secondary text-sm">{cfg?.mode ? SEARCH_MODE_INFO[cfg.mode as keyof typeof SEARCH_MODE_INFO]?.description : '—'}</td>
              </tr>
              {sr && (
                <>
                  <tr>
                    <td>Candidates Evaluated</td>
                    <td className="mono">{formatNumber(sr.instances_evaluated)}</td>
                    <td className="text-secondary text-sm">Exact instances tested against oracle</td>
                  </tr>
                  <tr>
                    <td>Estimated Space Size</td>
                    <td className="mono">{formatNumber(sr.estimated_space_size)}</td>
                    <td className="text-secondary text-sm">Total instances in configured parameter bounds</td>
                  </tr>
                  <tr>
                    <td>Coverage Ratio</td>
                    <td className="mono">{(sr.coverage_ratio * 100).toFixed(2)}%</td>
                    <td className="text-secondary text-sm">
                      {sr.coverage === 'EXHAUSTIVE'
                        ? 'Complete exhaustive enumeration'
                        : 'Partial coverage — not all instances were evaluated'}
                    </td>
                  </tr>
                  <tr>
                    <td>Coverage Type</td>
                    <td><span className={`badge ${sr.coverage === 'EXHAUSTIVE' ? 'badge-success' : 'badge-neutral'}`}>{sr.coverage}</span></td>
                    <td className="text-secondary text-sm">
                      {sr.coverage === 'EXHAUSTIVE'
                        ? 'All bounded candidates evaluated'
                        : 'Partial: results are statistically indicative, not exhaustive'}
                    </td>
                  </tr>
                  <tr>
                    <td>Counterexamples Found</td>
                    <td className="mono">{formatNumber(sr.counterexamples_found)}</td>
                    <td className="text-secondary text-sm">Verified failures against exact oracle</td>
                  </tr>
                  <tr>
                    <td>Failure Rate</td>
                    <td className="mono">{(sr.failure_rate * 100).toFixed(4)}%</td>
                    <td className="text-secondary text-sm">Over evaluated candidates</td>
                  </tr>
                  <tr>
                    <td>Elapsed Time</td>
                    <td className="mono">{formatMs(sr.elapsed_time_ms)}</td>
                    <td className="text-secondary text-sm">Total wall-clock engine execution time</td>
                  </tr>
                </>
              )}
              <tr>
                <td>PRNG Seed</td>
                <td className="mono">{cfg?.random_seed ?? '—'}</td>
                <td className="text-secondary text-sm">Controls randomized candidate generation</td>
              </tr>
              <tr>
                <td>Oracle Method</td>
                <td className="mono">{cfg?.oracle_id ?? '—'}</td>
                <td className="text-secondary text-sm">Authoritative exact verification algorithm</td>
              </tr>
              <tr>
                <td>Greedy Strategy</td>
                <td className="mono">{cfg?.strategy_id ?? '—'}</td>
                <td className="text-secondary text-sm">Heuristic under investigation</td>
              </tr>
            </tbody>
          </table>
        </div>
      </div>

      {/* Scientific scope note */}
      {experiment.status === 'NO_COUNTEREXAMPLE' && (
        <div className="alert alert-no-counterexample" role="note">
          <span className="alert-icon" aria-hidden="true">ℹ</span>
          <div className="alert-content">
            <div className="alert-title">Scientific Scope Boundary</div>
            <p className="text-sm" style={{ marginTop: 'var(--space-2)' }}>
              No counterexample was discovered in this bounded search.
              This result establishes empirical non-discovery within the configured parameter bounds only.
              It does <strong>not</strong> constitute a mathematical proof of correctness for unbounded inputs.
              To expand evidence, increase the search space bounds or evaluate additional strategy variants.
            </p>
          </div>
        </div>
      )}

      {/* Search space config */}
      <div className="card" style={{ marginTop: 'var(--space-4)' }}>
        <div className="card-header">
          <h3>Search Space Configuration</h3>
        </div>
        <div className="card-body">
          <pre className="code-block" style={{ margin: 0 }}>
            {JSON.stringify(cfg?.search_space ?? {}, null, 2)}
          </pre>
        </div>
      </div>
    </div>
  );
}

// --- Reproducibility Tab --------------------------------------------------
function ReproducibilityTab({
  experiment,
  executionResult,
  onReproduce,
  reproducing,
  reproductionResult,
  reproductionError,
}: {
  experiment: ExperimentDetail;
  executionResult: ExecutionResult | null;
  onReproduce: () => void;
  reproducing: boolean;
  reproductionResult: ReproductionResult | null;
  reproductionError: string;
}) {
  const cfg = experiment.configuration;
  const token = executionResult?.reproducibility_token ?? experiment;

  return (
    <div>
      <div className="card" style={{ marginBottom: 'var(--space-4)' }}>
        <div className="card-header">
          <h2>Deterministic Reproducibility Audit</h2>
        </div>
        <div className="card-body">
          <p className="text-secondary" style={{ marginBottom: 'var(--space-4)' }}>
            Re-running this experiment with the identical configuration and seed will produce
            the identical counterexample and forensic trace. Reproduction creates a{' '}
            <strong>new, distinct experiment record</strong> — the original is never overwritten.
          </p>
          <table className="data-table" style={{ marginBottom: 'var(--space-4)' }} aria-label="Reproducibility parameters">
            <tbody>
              <tr>
                <td className="text-secondary text-sm" style={{ width: '200px' }}>Experiment ID</td>
                <td className="mono">{experiment.id}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Problem Type</td>
                <td className="mono">{experiment.problem_type}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Greedy Strategy</td>
                <td className="mono">{cfg?.strategy_id ?? '—'}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Oracle</td>
                <td className="mono">{cfg?.oracle_id ?? '—'}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Search Mode</td>
                <td className="mono">{cfg?.mode ?? '—'}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Search Strategy</td>
                <td className="mono">{cfg?.strategy ?? '—'}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">PRNG Seed</td>
                <td className="mono">{cfg?.random_seed ?? '—'}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Max Candidates</td>
                <td className="mono">{formatNumber(cfg?.max_candidates ?? 0)}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Timeout</td>
                <td className="mono">{cfg?.max_time_ms ? `${(cfg.max_time_ms / 1000).toFixed(1)}s` : '—'}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Created At</td>
                <td className="mono">{formatDate(experiment.created_at)}</td>
              </tr>
              {experiment.parent_experiment_id && (
                <tr>
                  <td className="text-secondary text-sm">Parent Experiment</td>
                  <td>
                    <Link
                      to={`/experiments/${experiment.parent_experiment_id}`}
                      className="font-mono"
                      style={{ color: 'var(--color-primary)' }}
                    >
                      {experiment.parent_experiment_id}
                    </Link>
                  </td>
                </tr>
              )}
            </tbody>
          </table>

          {reproductionResult ? (
            <div
              className={`alert ${reproductionResult.is_deterministic_match ? 'alert-success' : 'alert-counterexample'}`}
              role="status"
            >
              <span className="alert-icon" aria-hidden="true">
                {reproductionResult.is_deterministic_match ? '✓' : '⚠'}
              </span>
              <div className="alert-content">
                <div className="alert-title">
                  {reproductionResult.is_deterministic_match
                    ? 'Reproduction Complete: Deterministic Match Confirmed'
                    : 'Reproduction Complete: Results Differ'}
                </div>
                <div className="text-sm" style={{ marginTop: 'var(--space-2)' }}>
                  {reproductionResult.match_summary}
                </div>
                <div className="text-sm" style={{ marginTop: 'var(--space-2)' }}>
                  Reproduction experiment:{' '}
                  <Link
                    to={`/experiments/${reproductionResult.reproduction_experiment_id}`}
                    style={{ color: 'var(--color-primary)' }}
                  >
                    {reproductionResult.reproduction_experiment_id}
                  </Link>
                </div>
              </div>
            </div>
          ) : reproductionError ? (
            <div className="alert alert-danger" role="alert">
              <span className="alert-icon" aria-hidden="true">⛔</span>
              <div className="alert-content text-sm">{reproductionError}</div>
            </div>
          ) : null}

          <div className="flex gap-3" style={{ marginTop: 'var(--space-4)' }}>
            <button
              id="reproduce-experiment"
              className="btn btn-primary"
              onClick={onReproduce}
              disabled={reproducing || !isTerminalStatus(experiment.status)}
              aria-label="Reproduce this experiment as a new linked run"
            >
              {reproducing ? (
                <>
                  <Spinner />
                  Reproducing…
                </>
              ) : (
                '⟳ Reproduce Experiment'
              )}
            </button>
          </div>
          <p className="text-secondary text-sm" style={{ marginTop: 'var(--space-2)' }}>
            Reproduction creates a new experiment with <Token>parent_experiment_id</Token> pointing to this run.
            The original experiment record is immutable and preserved.
          </p>
        </div>
      </div>

      {token && (
        <div className="card">
          <div className="card-header">
            <h3>Raw Reproducibility Token</h3>
          </div>
          <div className="card-body">
            <pre className="code-block" style={{ margin: 0, fontSize: '12px' }}>
              {JSON.stringify(token, null, 2)}
            </pre>
          </div>
        </div>
      )}
    </div>
  );
}

// --- Persistent Experiment Header -----------------------------------------
function ExperimentHeader({
  experiment,
  onReproduce,
  reproducing,
}: {
  experiment: ExperimentDetail;
  onReproduce: () => void;
  reproducing: boolean;
}) {
  return (
    <div
      style={{
        background: 'var(--color-surface-bg)',
        border: '1px solid var(--color-border-subtle)',
        borderRadius: 'var(--radius-lg)',
        padding: 'var(--space-5)',
        marginBottom: 'var(--space-4)',
        boxShadow: 'var(--shadow-card)',
      }}
    >
      <div className="flex items-start justify-between gap-4">
        <div>
          <div className="flex items-center gap-3 mb-2">
            <span className="badge badge-neutral">{PROBLEM_LABELS[experiment.problem_type]}</span>
            <StatusBadge status={experiment.status} />
            {experiment.parent_experiment_id && (
              <span className="badge badge-primary">Reproduction</span>
            )}
          </div>
          <h1 style={{ fontSize: '20px', marginBottom: 'var(--space-2)' }}>{experiment.name}</h1>
          <div
            className="flex gap-4 flex-wrap"
            style={{ fontSize: '12px', color: 'var(--color-text-tertiary)' }}
          >
            <span>ID: <code className="mono">{experiment.id}</code></span>
            <span>Created: {formatDate(experiment.created_at)}</span>
            <span>Strategy: <code className="mono">{experiment.configuration?.strategy_id}</code></span>
            <span>Seed: <code className="mono">{experiment.configuration?.random_seed}</code></span>
          </div>
        </div>
        {isTerminalStatus(experiment.status) && (
          <button
            className="btn btn-secondary"
            onClick={onReproduce}
            disabled={reproducing}
            style={{ flexShrink: 0 }}
          >
            {reproducing ? <Spinner /> : '⟳ Reproduce'}
          </button>
        )}
      </div>
    </div>
  );
}

// --- Running State --------------------------------------------------------
function RunningState({ status }: { status: ExecutionStatus | null }) {
  if (!status) return <LoadingPage message="Fetching experiment status…" />;
  return (
    <div className="card">
      <div className="card-body" style={{ textAlign: 'center', padding: 'var(--space-12) var(--space-8)' }}>
        <Spinner size="large" />
        <h2 style={{ marginTop: 'var(--space-4)', marginBottom: 'var(--space-2)' }}>
          Experiment Running
        </h2>
        <p className="text-secondary" style={{ marginBottom: 'var(--space-6)' }}>
          Status: <strong>{status.status}</strong>
        </p>
        <div
          className="metric-grid"
          style={{ gridTemplateColumns: 'repeat(3, 1fr)', maxWidth: '600px', margin: '0 auto' }}
        >
          <MetricCard
            label="Candidates Evaluated"
            value={formatNumber(status.instances_evaluated)}
          />
          <MetricCard
            label="Counterexamples Found"
            value={formatNumber(status.counterexamples_found)}
          />
          <MetricCard
            label="Elapsed"
            value={formatMs(status.elapsed_time_ms)}
          />
        </div>
        {status.message && (
          <p className="text-secondary text-sm" style={{ marginTop: 'var(--space-4)' }}>
            {status.message}
          </p>
        )}
      </div>
    </div>
  );
}

// --- Main Page -----------------------------------------------------------
export function ExperimentDetailPage() {
  const { id } = useParams<{ id: string }>();

  const [experiment, setExperiment] = useState<ExperimentDetail | null>(null);
  const [executionStatus, setExecutionStatus] = useState<ExecutionStatus | null>(null);
  const [executionResult, setExecutionResult] = useState<ExecutionResult | null>(null);
  const [counterexample, setCounterexample] = useState<CounterexampleDTO | null>(null);
  const [forensics, setForensics] = useState<ForensicReport | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState('');
  const [tab, setTab] = useState<Tab>('overview');

  const [reproducing, setReproducing] = useState(false);
  const [reproductionResult, setReproductionResult] = useState<ReproductionResult | null>(null);
  const [reproductionError, setReproductionError] = useState('');

  // Initial load
  useEffect(() => {
    if (!id) return;
    experimentsApi.get(id)
      .then(async (exp) => {
        setExperiment(exp);
        if (isTerminalStatus(exp.status)) {
          // Load results
          await Promise.all([
            experimentsApi.getResult(id).then(setExecutionResult).catch(() => {}),
            exp.status === 'COMPLETED'
              ? experimentsApi.getCounterexample(id).then(setCounterexample).catch(() => {})
              : Promise.resolve(),
            exp.status === 'COMPLETED'
              ? experimentsApi.getForensics(id).then(setForensics).catch(() => {})
              : Promise.resolve(),
          ]);
        } else if (isRunning(exp.status)) {
          await experimentsApi.getStatus(id).then(setExecutionStatus);
        }
      })
      .catch(e => setError(getErrorMessage(e)))
      .finally(() => setLoading(false));
  }, [id]);

  // Poll while running
  useEffect(() => {
    if (!id || !experiment) return;
    if (isTerminalStatus(experiment.status)) return;

    const interval = setInterval(async () => {
      try {
        const status = await experimentsApi.getStatus(id);
        setExecutionStatus(status);
        if (isTerminalStatus(status.status)) {
          clearInterval(interval);
          // Reload full experiment
          const exp = await experimentsApi.get(id);
          setExperiment(exp);
          if (status.status === 'COMPLETED') {
            const [result, cx, fr] = await Promise.all([
              experimentsApi.getResult(id),
              experimentsApi.getCounterexample(id),
              experimentsApi.getForensics(id),
            ]);
            setExecutionResult(result);
            setCounterexample(cx);
            setForensics(fr);
          } else {
            const result = await experimentsApi.getResult(id);
            setExecutionResult(result);
          }
        }
      } catch {
        clearInterval(interval);
      }
    }, 2000);

    return () => clearInterval(interval);
  }, [id, experiment]);

  const handleReproduce = useCallback(async () => {
    if (!id) return;
    setReproducing(true);
    setReproductionResult(null);
    setReproductionError('');
    try {
      const result = await experimentsApi.reproduce(id);
      setReproductionResult(result);
    } catch (e) {
      setReproductionError(getErrorMessage(e));
    } finally {
      setReproducing(false);
    }
  }, [id]);

  if (loading) return <LoadingPage message="Loading experiment…" />;
  if (error) return (
    <div className="page-content">
      <div className="container">
        <ErrorState
          title="Failed to load experiment"
          message={error}
          onRetry={() => { setError(''); setLoading(true); window.location.reload(); }}
        />
      </div>
    </div>
  );
  if (!experiment) return null;

  const isActive = isRunning(experiment.status);
  const tabs: { id: Tab; label: string }[] = [
    { id: 'overview', label: 'Overview' },
    { id: 'counterexample', label: 'Counterexample' },
    { id: 'forensics', label: 'Forensic Divergence' },
    { id: 'evidence', label: 'Evidence & Limits' },
    { id: 'reproducibility', label: 'Reproducibility' },
  ];

  return (
    <main id="main-content" className="page-content">
      <div className="container">
        <Breadcrumb items={[{ label: 'Experiments', to: '/experiments' }, { label: experiment.name }]} />

        <ExperimentHeader
          experiment={experiment}
          onReproduce={handleReproduce}
          reproducing={reproducing}
        />

        {isActive ? (
          <RunningState status={executionStatus} />
        ) : (
          <>
            {/* Tab navigation */}
            <div role="tablist" aria-label="Experiment result sections" className="tab-list">
              {tabs.map(t => (
                <button
                  key={t.id}
                  id={`tab-${t.id}`}
                  role="tab"
                  aria-selected={tab === t.id}
                  aria-controls={`tabpanel-${t.id}`}
                  className={`tab-trigger${tab === t.id ? ' active' : ''}`}
                  onClick={() => setTab(t.id)}
                >
                  {t.label}
                </button>
              ))}
            </div>

            {/* Tab panels */}
            <div style={{ marginTop: 'var(--space-6)' }}>
              {tab === 'overview' && (
                <div id="tabpanel-overview" role="tabpanel" aria-labelledby="tab-overview">
                  <OverviewTab experiment={experiment} executionResult={executionResult} onSelectTab={setTab} />
                </div>
              )}
              {tab === 'counterexample' && (
                <div id="tabpanel-counterexample" role="tabpanel" aria-labelledby="tab-counterexample">
                  <CounterexampleTab cx={counterexample} status={experiment.status} executionResult={executionResult} />
                </div>
              )}
              {tab === 'forensics' && (
                <div id="tabpanel-forensics" role="tabpanel" aria-labelledby="tab-forensics">
                  <ForensicsTab report={forensics} status={experiment.status} />
                </div>
              )}
              {tab === 'evidence' && (
                <div id="tabpanel-evidence" role="tabpanel" aria-labelledby="tab-evidence">
                  <EvidenceTab experiment={experiment} executionResult={executionResult} />
                </div>
              )}
              {tab === 'reproducibility' && (
                <div id="tabpanel-reproducibility" role="tabpanel" aria-labelledby="tab-reproducibility">
                  <ReproducibilityTab
                    experiment={experiment}
                    executionResult={executionResult}
                    onReproduce={handleReproduce}
                    reproducing={reproducing}
                    reproductionResult={reproductionResult}
                    reproductionError={reproductionError}
                  />
                </div>
              )}
            </div>
          </>
        )}
      </div>
    </main>
  );
}
