import React from 'react';
import { FileText, Download } from 'lucide-react';

export const HistoryReportsPage: React.FC = () => {
  return (
    <div style={{ display: 'flex', flexDirection: 'column', gap: '2.5rem' }}>
      <div>
        <h1 style={{ fontSize: '2rem', marginBottom: '0.4rem' }}>History / Forensic Reports</h1>
        <p style={{ color: 'var(--text-muted)', fontSize: '1.05rem', margin: 0 }}>
          Audit trail of completed counterexample investigations, reproducible session parameters, and exportable research reports.
        </p>
      </div>

      <div className="panel" style={{ padding: '3rem 2rem', textAlign: 'center' }}>
        <div style={{ display: 'inline-flex', padding: '1rem', background: 'var(--bg-subtle)', border: '1px solid var(--border-default)', borderRadius: '50%', marginBottom: '1.25rem' }}>
          <FileText size={32} color="var(--text-secondary)" />
        </div>
        <h2 style={{ fontSize: '1.35rem', marginBottom: '0.5rem' }}>Forensic Report Generator Scaffold</h2>
        <p style={{ maxWidth: '650px', margin: '0 auto 2rem', color: 'var(--text-secondary)', fontSize: '0.95rem' }}>
          Every investigation performed in <em>Investigate</em> produces deterministic JSON telemetry from the C++20 engine. The persistence manager and formal LaTeX/Markdown export pipelines are scheduled for Phase 4D.
        </p>

        <div style={{ display: 'flex', justifyContent: 'center', gap: '1rem', flexWrap: 'wrap' }}>
          <button type="button" className="btn btn-secondary" disabled>
            <Download size={14} /> Export Current Session (JSON)
          </button>
          <button type="button" className="btn btn-secondary" disabled>
            <FileText size={14} /> Export DAA Lab Report (Markdown)
          </button>
        </div>
      </div>
    </div>
  );
};
