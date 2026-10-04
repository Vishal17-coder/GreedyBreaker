import React, { useEffect, useState } from 'react';
import { Navbar } from '../components/Navbar';
import type { NavTab } from '../components/Navbar';
import type { HealthResponse } from '../types/api';
import { getHealth } from '../api/problems';

interface DashboardLayoutProps {
  currentTab: NavTab;
  onSelectTab: (tab: NavTab) => void;
  children: React.ReactNode;
}

export const DashboardLayout: React.FC<DashboardLayoutProps> = ({
  currentTab,
  onSelectTab,
  children,
}) => {
  const [health, setHealth] = useState<HealthResponse | null>(null);
  const [healthLoading, setHealthLoading] = useState<boolean>(true);

  useEffect(() => {
    let mounted = true;
    const fetchHealth = async () => {
      try {
        const res = await getHealth();
        if (mounted) {
          setHealth(res);
          setHealthLoading(false);
        }
      } catch (err) {
        if (mounted) {
          setHealth(null);
          setHealthLoading(false);
        }
      }
    };

    fetchHealth();
    const interval = setInterval(fetchHealth, 15000);
    return () => {
      mounted = false;
      clearInterval(interval);
    };
  }, []);

  return (
    <div className="app-container">
      <Navbar
        currentTab={currentTab}
        onSelectTab={onSelectTab}
        health={health}
        healthLoading={healthLoading}
      />
      <main className="main-content">{children}</main>
      <footer
        style={{
          borderTop: '1px solid var(--border-subtle)',
          padding: '1.75rem 1.5rem',
          background: '#ffffff',
          color: 'var(--text-muted)',
          fontSize: '0.8rem',
        }}
      >
        <div
          style={{
            maxWidth: 1240,
            margin: '0 auto',
            display: 'flex',
            justifyContent: 'space-between',
            alignItems: 'center',
            flexWrap: 'wrap',
            gap: '0.75rem',
          }}
        >
          <div>
            <strong>Greedy Breaker</strong> — Automated Counterexample Discovery Framework for Greedy Algorithms.
          </div>
          <div className="font-mono" style={{ fontSize: '0.75rem' }}>
            FastAPI Subprocess IPC • C++20 Core • {health?.version || 'v1.0.0'}
          </div>
        </div>
      </footer>
    </div>
  );
};
