import React from 'react';

interface StatusBadgeProps {
  type: 'counterexample' | 'optimal' | 'info' | 'warning';
  label?: string;
}

export const StatusBadge: React.FC<StatusBadgeProps> = ({ type, label }) => {
  switch (type) {
    case 'counterexample':
      return <span className="badge badge-failure">⚠ {label || 'COUNTEREXAMPLE'}</span>;
    case 'optimal':
      return <span className="badge badge-success">✓ {label || 'OPTIMAL / MATCH'}</span>;
    case 'warning':
      return <span className="badge badge-warning">⚡ {label || 'NOTICE'}</span>;
    case 'info':
    default:
      return <span className="badge badge-info">ℹ {label || 'INFO'}</span>;
  }
};
