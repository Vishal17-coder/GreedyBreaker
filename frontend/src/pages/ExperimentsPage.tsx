// =============================================================================
// Experiment History Page — Filterable catalog of all experiments
// =============================================================================
import { useState, useEffect, useCallback } from 'react';
import { Link } from 'react-router-dom';
import { experimentsApi, getErrorMessage } from '../api';
import type { ExperimentSummary, ExperimentStatus, ProblemType } from '../types';
import { PROBLEM_LABELS, STATUS_LABELS } from '../types';
import { StatusBadge } from '../components/StatusBadge';
import {
  Breadcrumb,
  LoadingPage,
  ErrorState,
  EmptyState,
} from '../components/Shared';
import { formatDate, formatNumber } from '../utils/format';

const ALL_STATUSES: ExperimentStatus[] = [
  'COMPLETED', 'NO_COUNTEREXAMPLE', 'SEARCH_INCOMPLETE',
  'TIMED_OUT', 'FAILED', 'CANCELLED', 'RUNNING', 'QUEUED',
  'CREATED', 'VALID', 'INVALID',
];

const PROBLEM_TYPES: ProblemType[] = [
  'coin_change', 'knapsack_01', 'activity_selection',
  'fractional_knapsack', 'huffman_coding',
];

export function ExperimentsPage() {
  const [experiments, setExperiments] = useState<ExperimentSummary[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState('');
  const [filterStatus, setFilterStatus] = useState<ExperimentStatus | ''>('');
  const [filterProblem, setFilterProblem] = useState<ProblemType | ''>('');
  const [page, setPage] = useState(0);
  const [totalPages, setTotalPages] = useState(0);
  const [totalElements, setTotalElements] = useState(0);

  const load = useCallback((showLoading = false) => {
    if (showLoading) setLoading(true);
    experimentsApi.list({
      status: filterStatus || undefined,
      problemType: filterProblem || undefined,
      page,
      size: 20,
      direction: 'desc',
    })
      .then(resp => {
        setExperiments(resp.content ?? []);
        setTotalPages(resp.total_pages ?? resp.totalPages ?? 0);
        setTotalElements(resp.total_elements ?? resp.totalElements ?? 0);
      })
      .catch(e => setError(getErrorMessage(e)))
      .finally(() => setLoading(false));
  }, [filterStatus, filterProblem, page]);

  useEffect(() => {
    let cancelled = false;
    experimentsApi.list({
      status: filterStatus || undefined,
      problemType: filterProblem || undefined,
      page,
      size: 20,
      direction: 'desc',
    })
      .then(resp => {
        if (!cancelled) {
          setExperiments(resp.content ?? []);
          setTotalPages(resp.total_pages ?? resp.totalPages ?? 0);
          setTotalElements(resp.total_elements ?? resp.totalElements ?? 0);
          setLoading(false);
        }
      })
      .catch(e => {
        if (!cancelled) {
          setError(getErrorMessage(e));
          setLoading(false);
        }
      });
    return () => { cancelled = true; };
  }, [filterStatus, filterProblem, page]);

  if (error) return <ErrorState message={error} onRetry={() => { setError(''); load(true); }} />;

  return (
    <main id="main-content" className="page-content">
      <div className="container">
        <Breadcrumb items={[{ label: 'Experiments' }]} />
        <div className="flex items-center justify-between mb-6">
          <div>
            <h1 style={{ marginBottom: 'var(--space-1)' }}>Experiments</h1>
            <p className="text-secondary text-sm">
              {totalElements > 0 ? `${formatNumber(totalElements)} experiment${totalElements !== 1 ? 's' : ''} recorded` : 'No experiments yet'}
            </p>
          </div>
          <Link to="/experiments/new" className="btn btn-primary" id="experiments-new-btn">
            + New Experiment
          </Link>
        </div>

        {/* Filters */}
        <div
          className="card"
          style={{ marginBottom: 'var(--space-4)', padding: 'var(--space-4) var(--space-5)' }}
        >
          <div className="flex gap-4 items-end flex-wrap">
            <div className="form-group" style={{ minWidth: 160 }}>
              <label className="form-label" htmlFor="filter-status">Status</label>
              <select
                id="filter-status"
                className="select"
                value={filterStatus}
                onChange={e => { setFilterStatus(e.target.value as ExperimentStatus | ''); setPage(0); }}
              >
                <option value="">All statuses</option>
                {ALL_STATUSES.map(s => (
                  <option key={s} value={s}>{STATUS_LABELS[s]}</option>
                ))}
              </select>
            </div>
            <div className="form-group" style={{ minWidth: 180 }}>
              <label className="form-label" htmlFor="filter-problem">Problem</label>
              <select
                id="filter-problem"
                className="select"
                value={filterProblem}
                onChange={e => { setFilterProblem(e.target.value as ProblemType | ''); setPage(0); }}
              >
                <option value="">All problems</option>
                {PROBLEM_TYPES.map(p => (
                  <option key={p} value={p}>{PROBLEM_LABELS[p]}</option>
                ))}
              </select>
            </div>
          </div>
        </div>

        {/* Table */}
        <div className="card">
          {loading ? (
            <div style={{ padding: 'var(--space-8)' }}>
              <LoadingPage message="Loading experiments…" />
            </div>
          ) : experiments.length === 0 ? (
            <EmptyState
              icon="🔍"
              title="No Experiments Found"
              message="No experiments match the current filters. Create a new experiment to begin automated counterexample discovery."
              actions={
                <>
                  <Link to="/experiments/new" className="btn btn-primary">Create First Experiment</Link>
                  {(filterStatus || filterProblem) && (
                    <button
                      className="btn btn-secondary"
                      onClick={() => { setFilterStatus(''); setFilterProblem(''); setPage(0); }}
                    >
                      Clear Filters
                    </button>
                  )}
                </>
              }
            />
          ) : (
            <div className="overflow-x-auto">
              <table
                className="data-table"
                aria-label="Experiment history"
                style={{ minWidth: 700 }}
              >
                <thead>
                  <tr>
                    <th scope="col">Experiment</th>
                    <th scope="col">Problem</th>
                    <th scope="col">Status</th>
                    <th scope="col" className="numeric">Candidates</th>
                    <th scope="col">Created</th>
                    <th scope="col" aria-label="Actions"></th>
                  </tr>
                </thead>
                <tbody>
                  {experiments.map(exp => (
                    <tr key={exp.id}>
                      <td>
                        <div style={{ fontWeight: 500, marginBottom: 2 }}>{exp.name}</div>
                        <div className="mono text-xs text-secondary">{exp.id}</div>
                      </td>
                      <td>
                        <span className="badge badge-neutral">
                          {PROBLEM_LABELS[exp.problem_type] ?? exp.problem_type}
                        </span>
                      </td>
                      <td>
                        <StatusBadge status={exp.status} />
                        {exp.status === 'COMPLETED' && exp.has_counterexample && (
                          <span
                            className="badge badge-counterexample"
                            style={{ marginLeft: 6 }}
                          >
                            ⚠ Gap Found
                          </span>
                        )}
                      </td>
                      <td className="numeric mono">
                        {formatNumber(exp.instances_evaluated)}
                      </td>
                      <td className="text-secondary text-sm">
                        {formatDate(exp.created_at)}
                      </td>
                      <td>
                        <Link
                          to={`/experiments/${exp.id}`}
                          className="btn btn-ghost btn-sm"
                          aria-label={`View details for experiment ${exp.name}`}
                        >
                          View →
                        </Link>
                      </td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          )}

          {/* Pagination */}
          {totalPages > 1 && (
            <div
              className="flex items-center justify-between"
              style={{
                padding: 'var(--space-3) var(--space-5)',
                borderTop: '1px solid var(--color-border-subtle)',
              }}
            >
              <span className="text-secondary text-sm">
                Page {page + 1} of {totalPages}
              </span>
              <div className="flex gap-2">
                <button
                  className="btn btn-secondary btn-sm"
                  disabled={page === 0}
                  onClick={() => setPage(p => p - 1)}
                  aria-label="Previous page"
                >
                  ← Prev
                </button>
                <button
                  className="btn btn-secondary btn-sm"
                  disabled={page >= totalPages - 1}
                  onClick={() => setPage(p => p + 1)}
                  aria-label="Next page"
                >
                  Next →
                </button>
              </div>
            </div>
          )}
        </div>
      </div>
    </main>
  );
}
