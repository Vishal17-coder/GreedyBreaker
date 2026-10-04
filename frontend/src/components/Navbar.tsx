import React from 'react';
import type { HealthResponse } from '../types/api';
import { ExternalLink, ArrowRight } from 'lucide-react';

export type NavTab = 'home' | 'investigate' | 'verify' | 'complexity' | 'benchmarks' | 'reports';

interface NavbarProps {
  currentTab: NavTab;
  onSelectTab: (tab: NavTab) => void;
  health: HealthResponse | null;
  healthLoading: boolean;
}

export const Navbar: React.FC<NavbarProps> = ({
  currentTab,
  onSelectTab,
  health,
  healthLoading,
}) => {
  const isEngineOnline =
    health?.engine?.exists && (health?.status === 'ok' || health?.status === 'healthy');

  return (
    <header className="app-header">
      <div className="header-inner">
        {/* Brand */}
        <div
          className="brand-section"
          onClick={() => onSelectTab('home')}
          title="Greedy Breaker - Home"
        >
          <div className="brand-mark">GB</div>
          <div style={{ display: 'flex', flexDirection: 'column' }}>
            <span className="brand-title">GREEDY BREAKER</span>
          </div>
        </div>

        {/* Navigation Links */}
        <nav className="nav-links">
          <button
            type="button"
            className={`nav-item ${currentTab === 'home' ? 'active' : ''}`}
            onClick={() => onSelectTab('home')}
          >
            Home
          </button>

          <button
            type="button"
            className={`nav-item ${currentTab === 'investigate' ? 'active' : ''}`}
            onClick={() => onSelectTab('investigate')}
          >
            Investigate
          </button>

          <button
            type="button"
            className={`nav-item ${currentTab === 'verify' ? 'active' : ''}`}
            onClick={() => onSelectTab('verify')}
          >
            Verify
          </button>

          <button
            type="button"
            className={`nav-item ${currentTab === 'complexity' ? 'active' : ''}`}
            onClick={() => onSelectTab('complexity')}
          >
            Complexity
          </button>

          <button
            type="button"
            className={`nav-item ${currentTab === 'benchmarks' ? 'active' : ''}`}
            onClick={() => onSelectTab('benchmarks')}
          >
            Benchmarks
          </button>

          <button
            type="button"
            className={`nav-item ${currentTab === 'reports' ? 'active' : ''}`}
            onClick={() => onSelectTab('reports')}
          >
            Reports
          </button>
        </nav>

        {/* Right side status & action */}
        <div className="header-actions">
          <div className="status-pill" title={`C++ Engine: ${health?.engine?.path || 'Checking'}`}>
            <span className={`status-dot ${isEngineOnline ? 'online' : 'offline'}`} />
            <span>
              {healthLoading
                ? 'Connecting...'
                : isEngineOnline
                ? 'C++ Engine: Online'
                : 'Engine: Offline'}
            </span>
          </div>

          {currentTab !== 'investigate' ? (
            <button
              type="button"
              className="btn btn-primary"
              style={{ fontSize: '0.8rem', padding: '0.4rem 0.85rem' }}
              onClick={() => onSelectTab('investigate')}
            >
              <span>Investigate</span>
              <ArrowRight size={13} />
            </button>
          ) : (
            <a
              href="http://127.0.0.1:8000/docs"
              target="_blank"
              rel="noreferrer"
              className="btn btn-secondary"
              style={{ fontSize: '0.75rem', padding: '0.35rem 0.65rem' }}
              title="FastAPI Interactive Swagger Docs"
            >
              <span>API Specs</span>
              <ExternalLink size={11} />
            </a>
          )}
        </div>
      </div>
    </header>
  );
};
