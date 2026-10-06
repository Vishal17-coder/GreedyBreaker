// =============================================================================
// Navigation component
// =============================================================================
import { Link, useLocation } from 'react-router-dom';

export function Navigation() {
  const loc = useLocation();

  const isActive = (path: string) => {
    if (path === '/') return loc.pathname === '/';
    return loc.pathname.startsWith(path);
  };

  return (
    <nav className="nav" aria-label="Main navigation">
      <div className="nav-inner">
        <Link to="/" className="nav-brand" aria-label="Greedy Breaker home">
          <span className="nav-brand-icon" aria-hidden="true">GB</span>
          Greedy Breaker
        </Link>

        <div className="nav-links" role="list">
          <Link
            to="/"
            className={`nav-link${isActive('/') ? ' active' : ''}`}
            role="listitem"
            aria-current={isActive('/') ? 'page' : undefined}
          >
            Home
          </Link>
          <Link
            to="/experiments"
            className={`nav-link${isActive('/experiments') ? ' active' : ''}`}
            role="listitem"
            aria-current={isActive('/experiments') ? 'page' : undefined}
          >
            Experiments
          </Link>
          <Link
            to="/problems"
            className={`nav-link${isActive('/problems') ? ' active' : ''}`}
            role="listitem"
            aria-current={isActive('/problems') ? 'page' : undefined}
          >
            Problems
          </Link>
          <Link
            to="/docs"
            className={`nav-link${isActive('/docs') ? ' active' : ''}`}
            role="listitem"
            aria-current={isActive('/docs') ? 'page' : undefined}
          >
            Documentation
          </Link>
        </div>

        <div className="nav-actions">
          <Link to="/experiments/new" className="btn btn-primary btn-sm" id="nav-new-experiment">
            + New Experiment
          </Link>
        </div>
      </div>
    </nav>
  );
}
