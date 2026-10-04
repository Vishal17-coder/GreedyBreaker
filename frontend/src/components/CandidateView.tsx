import React from 'react';

interface CandidateViewProps {
  problem: string;
  candidate: any;
}

export const CandidateView: React.FC<CandidateViewProps> = ({ problem, candidate }) => {
  if (!candidate) {
    return <span style={{ color: 'var(--text-muted)' }}>No candidate instance data</span>;
  }

  // Coin Change format: { denominations: [1, 3, 4], target: 6 }
  if (problem === 'coin_change' || ('denominations' in candidate && 'target' in candidate)) {
    const denoms = candidate.denominations || [];
    const target = candidate.target;
    return (
      <div style={{ display: 'flex', flexDirection: 'column', gap: '0.65rem' }}>
        <div style={{ display: 'flex', alignItems: 'center', gap: '0.75rem', flexWrap: 'wrap' }}>
          <span style={{ color: 'var(--text-muted)', fontSize: '0.825rem', fontWeight: 600, textTransform: 'uppercase' }}>
            Coin System:
          </span>
          <span className="font-mono" style={{ fontWeight: 700, color: 'var(--text-primary)', fontSize: '1rem' }}>
            &#123; {denoms.join(', ')} &#125;
          </span>
        </div>
        <div style={{ display: 'flex', alignItems: 'center', gap: '0.75rem' }}>
          <span style={{ color: 'var(--text-muted)', fontSize: '0.825rem', fontWeight: 600, textTransform: 'uppercase' }}>
            Target Amount:
          </span>
          <span className="font-mono" style={{ fontWeight: 700, color: 'var(--accent-primary)', fontSize: '1.15rem' }}>
            {target}
          </span>
        </div>
      </div>
    );
  }

  // 0/1 or Fractional Knapsack: { capacity: 5, items: [{weight: 2, value: 10}, ...] }
  if (
    problem === 'knapsack_01' ||
    problem === 'knapsack_fractional' ||
    ('capacity' in candidate && 'items' in candidate)
  ) {
    const capacity = candidate.capacity;
    const items = candidate.items || [];
    return (
      <div style={{ display: 'flex', flexDirection: 'column', gap: '0.75rem' }}>
        <div style={{ display: 'flex', alignItems: 'center', gap: '0.75rem' }}>
          <span style={{ color: 'var(--text-muted)', fontSize: '0.825rem', fontWeight: 600, textTransform: 'uppercase' }}>
            Knapsack Capacity:
          </span>
          <span className="font-mono" style={{ fontWeight: 700, color: 'var(--accent-primary)', fontSize: '1.05rem' }}>
            W = {capacity}
          </span>
          <span style={{ color: 'var(--text-muted)', fontSize: '0.825rem' }}>({items.length} items)</span>
        </div>
        <div className="table-container" style={{ maxHeight: '200px' }}>
          <table className="data-table">
            <thead>
              <tr>
                <th>Item #</th>
                <th>Weight (w)</th>
                <th>Value (v)</th>
                <th>Density (v/w)</th>
              </tr>
            </thead>
            <tbody>
              {items.map((item: any, idx: number) => {
                const ratio = item.weight > 0 ? (item.value / item.weight).toFixed(2) : '∞';
                return (
                  <tr key={idx}>
                    <td className="font-mono">Item {idx + 1}</td>
                    <td className="font-mono">{item.weight}</td>
                    <td className="font-mono">{item.value}</td>
                    <td className="font-mono" style={{ color: 'var(--accent-primary)', fontWeight: 600 }}>
                      {ratio}
                    </td>
                  </tr>
                );
              })}
            </tbody>
          </table>
        </div>
      </div>
    );
  }

  // Activity Selection: { activities: [{start: 1, finish: 4}, ...] }
  if (problem === 'activity_selection' || 'activities' in candidate) {
    const activities = candidate.activities || [];
    return (
      <div style={{ display: 'flex', flexDirection: 'column', gap: '0.75rem' }}>
        <div style={{ display: 'flex', alignItems: 'center', gap: '0.75rem' }}>
          <span style={{ color: 'var(--text-muted)', fontSize: '0.825rem', fontWeight: 600, textTransform: 'uppercase' }}>
            Total Activities:
          </span>
          <span className="font-mono" style={{ fontWeight: 700, color: 'var(--accent-primary)', fontSize: '1.05rem' }}>
            |S| = {activities.length}
          </span>
        </div>
        <div className="table-container" style={{ maxHeight: '200px' }}>
          <table className="data-table">
            <thead>
              <tr>
                <th>Activity</th>
                <th>Start Time (s)</th>
                <th>Finish Time (f)</th>
                <th>Duration (f - s)</th>
              </tr>
            </thead>
            <tbody>
              {activities.map((act: any, idx: number) => (
                <tr key={idx}>
                  <td className="font-mono">a_{idx + 1}</td>
                  <td className="font-mono">{act.start}</td>
                  <td className="font-mono">{act.finish}</td>
                  <td className="font-mono">{act.finish - act.start}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </div>
    );
  }

  // Fallback generic JSON view
  return (
    <pre
      className="font-mono"
      style={{
        background: 'var(--bg-subtle)',
        border: '1px solid var(--border-subtle)',
        padding: '0.75rem',
        borderRadius: 'var(--radius-sm)',
        fontSize: '0.8rem',
        overflowX: 'auto',
      }}
    >
      {JSON.stringify(candidate, null, 2)}
    </pre>
  );
};
