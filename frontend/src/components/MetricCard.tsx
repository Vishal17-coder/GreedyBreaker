import React from 'react';

interface MetricCardProps {
  label: string;
  value: string | number;
  sub?: string;
  variant?: 'default' | 'red' | 'green' | 'blue';
}

export const MetricCard: React.FC<MetricCardProps> = ({ label, value, sub, variant = 'default' }) => {
  const getHighlightClass = () => {
    switch (variant) {
      case 'red':
        return 'highlight-red';
      case 'green':
        return 'highlight-green';
      case 'blue':
        return 'highlight-blue';
      default:
        return '';
    }
  };

  return (
    <div className="metric-box">
      <div className="metric-label">{label}</div>
      <div className={`metric-value ${getHighlightClass()}`}>{value}</div>
      {sub && <div className="metric-sub">{sub}</div>}
    </div>
  );
};
