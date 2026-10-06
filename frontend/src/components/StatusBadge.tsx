// =============================================================================
// StatusBadge — Maps ExperimentStatus to labelled badge
// =============================================================================
import type { ExperimentStatus } from '../types';
import { STATUS_LABELS } from '../types';

interface StatusBadgeProps {
  status: ExperimentStatus;
  showIcon?: boolean;
}

const STATUS_CLASS: Record<ExperimentStatus, string> = {
  CREATED: 'badge-neutral',
  VALIDATING: 'badge-neutral',
  VALID: 'badge-primary',
  INVALID: 'badge-danger',
  QUEUED: 'badge-neutral',
  RUNNING: 'badge-primary',
  COMPLETED: 'badge-counterexample',
  NO_COUNTEREXAMPLE: 'badge-no-counterexample',
  SEARCH_INCOMPLETE: 'badge-neutral',
  TIMED_OUT: 'badge-neutral',
  FAILED: 'badge-danger',
  CANCELLED: 'badge-neutral',
};

const STATUS_ICON: Record<ExperimentStatus, string> = {
  CREATED: '○',
  VALIDATING: '⋯',
  VALID: '✓',
  INVALID: '⛔',
  QUEUED: '○',
  RUNNING: '▶',
  COMPLETED: '⚠',
  NO_COUNTEREXAMPLE: 'ℹ',
  SEARCH_INCOMPLETE: '~',
  TIMED_OUT: '⏱',
  FAILED: '⛔',
  CANCELLED: '×',
};

export function StatusBadge({ status, showIcon = true }: StatusBadgeProps) {
  return (
    <span
      className={`badge ${STATUS_CLASS[status]}`}
      aria-label={`Status: ${STATUS_LABELS[status]}`}
    >
      {showIcon && (
        <span aria-hidden="true">{STATUS_ICON[status]}</span>
      )}
      {STATUS_LABELS[status]}
    </span>
  );
}
