// =============================================================================
// Shared utility components
// =============================================================================
import { Link } from 'react-router-dom';
import type { ReactNode } from 'react';

// --- Breadcrumb -----------------------------------------------------------
export interface BreadcrumbItem {
  label: string;
  to?: string;
}

export function Breadcrumb({ items }: { items: BreadcrumbItem[] }) {
  return (
    <nav className="breadcrumb" aria-label="Breadcrumb">
      {items.map((item, i) => (
        <span key={i} className="flex items-center gap-2">
          {i > 0 && <span className="breadcrumb-sep" aria-hidden="true">/</span>}
          {item.to ? (
            <Link to={item.to}>{item.label}</Link>
          ) : (
            <span aria-current="page">{item.label}</span>
          )}
        </span>
      ))}
    </nav>
  );
}

// --- Loading Spinner ------------------------------------------------------
export function Spinner({ size = 'default' }: { size?: 'default' | 'large' }) {
  return (
    <span
      className={`spinner${size === 'large' ? ' spinner-lg' : ''}`}
      role="status"
      aria-label="Loading"
    />
  );
}

export function LoadingPage({ message = 'Loading…' }: { message?: string }) {
  return (
    <div
      style={{
        display: 'flex',
        flexDirection: 'column',
        alignItems: 'center',
        justifyContent: 'center',
        minHeight: '40vh',
        gap: '16px',
        color: 'var(--color-text-secondary)',
      }}
      role="status"
      aria-label={message}
    >
      <Spinner size="large" />
      <span className="text-sm">{message}</span>
    </div>
  );
}

// --- Error State ----------------------------------------------------------
export function ErrorState({
  title = 'Something went wrong',
  message,
  onRetry,
}: {
  title?: string;
  message: string;
  onRetry?: () => void;
}) {
  return (
    <div className="empty-state" role="alert">
      <div className="empty-state-icon" aria-hidden="true">⛔</div>
      <h2>{title}</h2>
      <p>{message}</p>
      {onRetry && (
        <div className="empty-state-actions">
          <button className="btn btn-secondary" onClick={onRetry}>
            Try Again
          </button>
        </div>
      )}
    </div>
  );
}

// --- Empty State ----------------------------------------------------------
export function EmptyState({
  icon,
  title,
  message,
  actions,
}: {
  icon?: string;
  title: string;
  message: string;
  actions?: ReactNode;
}) {
  return (
    <div className="empty-state">
      {icon && <div className="empty-state-icon" aria-hidden="true">{icon}</div>}
      <h2>{title}</h2>
      <p>{message}</p>
      {actions && <div className="empty-state-actions">{actions}</div>}
    </div>
  );
}

// --- Card -----------------------------------------------------------------
export function Card({
  children,
  className = '',
}: {
  children: ReactNode;
  className?: string;
}) {
  return <div className={`card ${className}`}>{children}</div>;
}

// --- Metric Card ----------------------------------------------------------
export function MetricCard({
  label,
  value,
  note,
  valueStyle,
}: {
  label: string;
  value: string | number;
  note?: string;
  valueStyle?: React.CSSProperties;
}) {
  return (
    <div className="metric-card">
      <div className="metric-label">{label}</div>
      <div className="metric-value" style={valueStyle}>
        {value}
      </div>
      {note && <div className="metric-note">{note}</div>}
    </div>
  );
}

// --- Section Divider ------------------------------------------------------
export function SectionTitle({
  children,
  subtitle,
}: {
  children: ReactNode;
  subtitle?: string;
}) {
  return (
    <div style={{ marginBottom: 'var(--space-4)' }}>
      <h2>{children}</h2>
      {subtitle && (
        <p className="text-secondary" style={{ marginTop: 'var(--space-1)' }}>
          {subtitle}
        </p>
      )}
    </div>
  );
}

// --- Inline code token ---------------------------------------------------
export function Token({ children }: { children: ReactNode }) {
  return (
    <code
      style={{
        fontFamily: 'var(--font-mono)',
        fontSize: '13px',
        background: 'var(--color-surface-subtle)',
        border: '1px solid var(--color-border-subtle)',
        borderRadius: 'var(--radius-sm)',
        padding: '1px 6px',
      }}
    >
      {children}
    </code>
  );
}

// --- Objective badge ------------------------------------------------------
export function ObjectiveBadge({ objective }: { objective: 'MINIMIZE' | 'MAXIMIZE' }) {
  return (
    <span
      className={`badge ${objective === 'MINIMIZE' ? 'badge-primary' : 'badge-success'}`}
    >
      {objective}
    </span>
  );
}
