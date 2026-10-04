import React, { useState, useEffect } from 'react';
import type {
  ProblemMetadata,
  ProblemType,
  VerificationRequest,
  VerificationResponse,
} from '../types/api';
import { ApiError } from '../types/api';
import { getProblems } from '../api/problems';
import { verifyCandidate } from '../api/verify';
import { MetricCard } from '../components/MetricCard';
import { StatusBadge } from '../components/StatusBadge';
import { DivergencePanel } from '../components/DivergencePanel';
import { TraceTable } from '../components/TraceTable';
import { CheckCircle2, AlertTriangle, Play, Code2 } from 'lucide-react';

export const VerifyPage: React.FC = () => {
  const [problems, setProblems] = useState<ProblemMetadata[]>([]);
  const [, setLoadingProblems] = useState<boolean>(true);
  const [selectedProblemId, setSelectedProblemId] = useState<ProblemType>('coin_change');

  // Input modes: 'structured' | 'json'
  const [inputMode, setInputMode] = useState<'structured' | 'json'>('structured');

  // Coin Change state
  const [coinDenoms, setCoinDenoms] = useState<string>('1, 3, 4');
  const [coinTarget, setCoinTarget] = useState<number>(6);

  // Knapsack state
  const [knapsackCapacity, setKnapsackCapacity] = useState<number>(5);
  const [knapsackItemsText, setKnapsackItemsText] = useState<string>(
    '2:10\n3:15\n4:25' // weight:value format
  );

  // Activity Selection state
  const [activitiesText, setActivitiesText] = useState<string>(
    '1:4\n3:5\n0:6\n5:7\n3:9\n5:9\n6:10\n8:11\n8:12\n2:14\n12:16' // start:finish format
  );

  // Raw JSON state
  const [rawJson, setRawJson] = useState<string>(
    JSON.stringify({ denominations: [1, 3, 4], target: 6 }, null, 2)
  );

  // Verification execution state
  const [isVerifying, setIsVerifying] = useState<boolean>(false);
  const [result, setResult] = useState<VerificationResponse | null>(null);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    let mounted = true;
    const load = async () => {
      try {
        setLoadingProblems(true);
        const data = await getProblems();
        if (mounted) {
          setProblems(data);
          setLoadingProblems(false);
        }
      } catch (err) {
        if (mounted) setLoadingProblems(false);
      }
    };
    load();
    return () => {
      mounted = false;
    };
  }, []);

  const buildCandidateFromStructured = (): any => {
    if (selectedProblemId === 'coin_change') {
      const denoms = coinDenoms
        .split(',')
        .map((s) => parseInt(s.trim()))
        .filter((n) => !isNaN(n) && n > 0);
      return { denominations: denoms, target: coinTarget };
    }

    if (selectedProblemId === 'knapsack_01' || selectedProblemId === 'knapsack_fractional') {
      const lines = knapsackItemsText.split('\n');
      const items: { weight: number; value: number }[] = [];
      for (const line of lines) {
        const parts = line.split(':');
        if (parts.length === 2) {
          const w = parseInt(parts[0].trim());
          const v = parseInt(parts[1].trim());
          if (!isNaN(w) && !isNaN(v) && w > 0 && v >= 0) {
            items.push({ weight: w, value: v });
          }
        }
      }
      return { capacity: knapsackCapacity, items };
    }

    if (selectedProblemId === 'activity_selection') {
      const lines = activitiesText.split('\n');
      const activities: { start: number; finish: number }[] = [];
      for (const line of lines) {
        const parts = line.split(':');
        if (parts.length === 2) {
          const s = parseInt(parts[0].trim());
          const f = parseInt(parts[1].trim());
          if (!isNaN(s) && !isNaN(f) && s >= 0 && f > s) {
            activities.push({ start: s, finish: f });
          }
        }
      }
      return { activities };
    }

    return {};
  };

  const applyPreset = (preset: string) => {
    switch (preset) {
      case 'coin-change-classic':
        setSelectedProblemId('coin_change');
        setCoinDenoms('1, 3, 4');
        setCoinTarget(6);
        setRawJson(JSON.stringify({ denominations: [1, 3, 4], target: 6 }, null, 2));
        break;
      case 'coin-change-us':
        setSelectedProblemId('coin_change');
        setCoinDenoms('1, 5, 10, 25');
        setCoinTarget(30);
        setRawJson(JSON.stringify({ denominations: [1, 5, 10, 25], target: 30 }, null, 2));
        break;
      case 'knapsack-classic':
        setSelectedProblemId('knapsack_01');
        setKnapsackCapacity(5);
        setKnapsackItemsText('2:10\n3:15\n4:25');
        setRawJson(
          JSON.stringify(
            {
              capacity: 5,
              items: [
                { weight: 2, value: 10 },
                { weight: 3, value: 15 },
                { weight: 4, value: 25 },
              ],
            },
            null,
            2
          )
        );
        break;
      case 'activity-clrs':
        setSelectedProblemId('activity_selection');
        setActivitiesText('1:4\n3:5\n0:6\n5:7\n3:9\n5:9\n6:10\n8:11\n8:12\n2:14\n12:16');
        setRawJson(
          JSON.stringify(
            {
              activities: [
                { start: 1, finish: 4 },
                { start: 3, finish: 5 },
                { start: 0, finish: 6 },
                { start: 5, finish: 7 },
                { start: 3, finish: 9 },
                { start: 5, finish: 9 },
                { start: 6, finish: 10 },
                { start: 8, finish: 11 },
                { start: 8, finish: 12 },
                { start: 2, finish: 14 },
                { start: 12, finish: 16 },
              ],
            },
            null,
            2
          )
        );
        break;
    }
  };

  const handleVerify = async () => {
    setIsVerifying(true);
    setError(null);
    setResult(null);

    let candidateObj: any;
    if (inputMode === 'json') {
      try {
        candidateObj = JSON.parse(rawJson);
      } catch (e: any) {
        setError(`Malformed JSON input: ${e.message}`);
        setIsVerifying(false);
        return;
      }
    } else {
      candidateObj = buildCandidateFromStructured();
    }

    const req: VerificationRequest = {
      problem: selectedProblemId,
      candidate: candidateObj,
    };

    try {
      const res = await verifyCandidate(req);
      setResult(res);
    } catch (err: any) {
      if (err instanceof ApiError) {
        setError(`[${err.code}] ${err.message}`);
      } else {
        setError(err.message || 'Verification failed unexpectedly.');
      }
    } finally {
      setIsVerifying(false);
    }
  };

  const activeProblem = problems.find((p) => p.id === selectedProblemId);

  return (
    <div style={{ display: 'flex', flexDirection: 'column', gap: '2.5rem' }}>
      {/* 1. Header */}
      <div>
        <h1 style={{ fontSize: '2rem', marginBottom: '0.4rem' }}>Verify a Candidate</h1>
        <p style={{ color: 'var(--text-muted)', fontSize: '1.05rem', margin: 0 }}>
          Execute dual greedy and exact DP evaluation on specific problem instances to verify solution optimality and trace divergences.
        </p>
      </div>

      {/* 2. Workspace Panel */}
      <div className="panel">
        {/* Presets Header */}
        <div
          style={{
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'space-between',
            flexWrap: 'wrap',
            gap: '0.75rem',
            paddingBottom: '1rem',
            borderBottom: '1px solid var(--border-subtle)',
            marginBottom: '1.5rem',
          }}
        >
          <span style={{ fontSize: '0.8rem', fontWeight: 600, color: 'var(--text-muted)', textTransform: 'uppercase' }}>
            Verification Presets:
          </span>
          <div style={{ display: 'flex', gap: '0.5rem', flexWrap: 'wrap' }}>
            <button
              type="button"
              className="btn btn-secondary"
              style={{ fontSize: '0.775rem', padding: '0.25rem 0.65rem' }}
              onClick={() => applyPreset('coin-change-classic')}
            >
              Coin Change: &#123;1,3,4&#125;, Target 6 (Counterexample)
            </button>
            <button
              type="button"
              className="btn btn-secondary"
              style={{ fontSize: '0.775rem', padding: '0.25rem 0.65rem' }}
              onClick={() => applyPreset('coin-change-us')}
            >
              US Coins: &#123;1,5,10,25&#125;, Target 30 (Optimal)
            </button>
            <button
              type="button"
              className="btn btn-secondary"
              style={{ fontSize: '0.775rem', padding: '0.25rem 0.65rem' }}
              onClick={() => applyPreset('knapsack-classic')}
            >
              0/1 Knapsack: 3 Items, W=5 (Counterexample)
            </button>
            <button
              type="button"
              className="btn btn-secondary"
              style={{ fontSize: '0.775rem', padding: '0.25rem 0.65rem' }}
              onClick={() => applyPreset('activity-clrs')}
            >
              Activity Selection: 11 Activities (Optimal)
            </button>
          </div>
        </div>

        {/* Configuration Columns */}
        <div className="grid-2">
          {/* Left Column: Problem Selection */}
          <div>
            <div className="form-group">
              <label className="form-label" htmlFor="verify-problem-select">Target Problem Family</label>
              <select
                id="verify-problem-select"
                className="form-control form-select"
                value={selectedProblemId}
                onChange={(e) => setSelectedProblemId(e.target.value as ProblemType)}
              >
                {problems.map((p) => (
                  <option key={p.id} value={p.id}>
                    {p.name} ({p.objective})
                  </option>
                ))}
              </select>
            </div>

            {activeProblem && (
              <div
                style={{
                  background: 'var(--bg-subtle)',
                  padding: '1rem',
                  borderRadius: 'var(--radius-sm)',
                  border: '1px solid var(--border-subtle)',
                }}
              >
                <div style={{ fontSize: '0.85rem', color: 'var(--text-secondary)', marginBottom: '0.5rem' }}>
                  {activeProblem.description}
                </div>
                <div style={{ fontSize: '0.8rem', display: 'flex', flexDirection: 'column', gap: '0.25rem' }}>
                  <div>
                    <span style={{ color: 'var(--text-muted)' }}>Greedy Strategy: </span>
                    <strong>{activeProblem.greedy_strategy}</strong>
                  </div>
                  <div>
                    <span style={{ color: 'var(--text-muted)' }}>Oracle Strategy: </span>
                    <strong style={{ color: 'var(--accent-primary)' }}>{activeProblem.oracle_strategy}</strong>
                  </div>
                </div>
              </div>
            )}
          </div>

          {/* Right Column: Candidate Parameters */}
          <div>
            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '0.5rem' }}>
              <label className="form-label" style={{ margin: 0 }}>Candidate Instance Input</label>
              <div style={{ display: 'flex', gap: '0.25rem' }}>
                <button
                  type="button"
                  className={`btn btn-secondary ${inputMode === 'structured' ? 'active' : ''}`}
                  style={{ fontSize: '0.725rem', padding: '0.2rem 0.5rem' }}
                  onClick={() => setInputMode('structured')}
                >
                  Form
                </button>
                <button
                  type="button"
                  className={`btn btn-secondary ${inputMode === 'json' ? 'active' : ''}`}
                  style={{ fontSize: '0.725rem', padding: '0.2rem 0.5rem' }}
                  onClick={() => setInputMode('json')}
                >
                  <Code2 size={11} />
                  JSON
                </button>
              </div>
            </div>

            {inputMode === 'structured' ? (
              <>
                {selectedProblemId === 'coin_change' && (
                  <div style={{ display: 'flex', flexDirection: 'column', gap: '0.85rem' }}>
                    <div className="form-group" style={{ margin: 0 }}>
                      <label className="form-label">Denominations (comma-separated)</label>
                      <input
                        type="text"
                        className="form-control font-mono"
                        value={coinDenoms}
                        onChange={(e) => setCoinDenoms(e.target.value)}
                        placeholder="1, 3, 4"
                      />
                      <div className="form-help">Positive integers, strictly increasing or unique.</div>
                    </div>
                    <div className="form-group" style={{ margin: 0 }}>
                      <label className="form-label">Target Amount (n)</label>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={coinTarget}
                        onChange={(e) => setCoinTarget(parseInt(e.target.value) || 1)}
                        min={1}
                      />
                    </div>
                  </div>
                )}

                {(selectedProblemId === 'knapsack_01' || selectedProblemId === 'knapsack_fractional') && (
                  <div style={{ display: 'flex', flexDirection: 'column', gap: '0.85rem' }}>
                    <div className="form-group" style={{ margin: 0 }}>
                      <label className="form-label">Capacity (W)</label>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={knapsackCapacity}
                        onChange={(e) => setKnapsackCapacity(parseInt(e.target.value) || 1)}
                        min={1}
                      />
                    </div>
                    <div className="form-group" style={{ margin: 0 }}>
                      <label className="form-label">Items List (weight:value per line)</label>
                      <textarea
                        className="form-control font-mono"
                        rows={3}
                        value={knapsackItemsText}
                        onChange={(e) => setKnapsackItemsText(e.target.value)}
                        placeholder="2:10&#10;3:15&#10;4:25"
                      />
                    </div>
                  </div>
                )}

                {selectedProblemId === 'activity_selection' && (
                  <div className="form-group" style={{ margin: 0 }}>
                    <label className="form-label">Activities (start:finish per line)</label>
                    <textarea
                      className="form-control font-mono"
                      rows={4}
                      value={activitiesText}
                      onChange={(e) => setActivitiesText(e.target.value)}
                      placeholder="1:4&#10;3:5&#10;0:6"
                    />
                    <div className="form-help">Start time must be strictly less than finish time.</div>
                  </div>
                )}
              </>
            ) : (
              <div className="form-group" style={{ margin: 0 }}>
                <textarea
                  className="form-control font-mono"
                  rows={5}
                  value={rawJson}
                  onChange={(e) => setRawJson(e.target.value)}
                />
              </div>
            )}
          </div>
        </div>

        {/* Action Button */}
        <div style={{ marginTop: '1.5rem', display: 'flex', justifyContent: 'flex-start' }}>
          <button
            type="button"
            className="btn btn-break btn-lg"
            disabled={isVerifying}
            onClick={handleVerify}
          >
            {isVerifying ? (
              <>
                <span className="spinner" />
                <span>Executing dual solvers...</span>
              </>
            ) : (
              <>
                <Play size={16} />
                <span>RUN DUAL VERIFICATION</span>
              </>
            )}
          </button>
        </div>
      </div>

      {/* Error Banner */}
      {error && (
        <div className="result-banner result-banner-failure">
          <AlertTriangle color="var(--status-failure)" size={22} style={{ flexShrink: 0 }} />
          <div>
            <div style={{ fontWeight: 600, color: 'var(--status-failure-text)', marginBottom: '0.2rem' }}>
              Verification Rejected
            </div>
            <div style={{ fontSize: '0.875rem', color: 'var(--text-secondary)' }}>{error}</div>
          </div>
        </div>
      )}

      {/* Verification Result Display */}
      {result && (
        <div style={{ display: 'flex', flexDirection: 'column', gap: '1.75rem' }}>
          <div>
            <div style={{ fontSize: '0.8rem', fontWeight: 700, color: 'var(--accent-primary)', textTransform: 'uppercase', letterSpacing: '0.04em', marginBottom: '0.25rem' }}>
              Verification Report
            </div>
            <h2 style={{ fontSize: '1.5rem', margin: 0 }}>
              {result.counterexample_found
                ? 'Counterexample Validated — Greedy Suboptimal'
                : 'Instance Verified — Greedy Matched Optimal'}
            </h2>
          </div>

          {/* Banner */}
          {result.counterexample_found ? (
            <div className="result-banner result-banner-failure">
              <AlertTriangle color="var(--status-failure)" size={26} style={{ flexShrink: 0 }} />
              <div style={{ flex: 1 }}>
                <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', flexWrap: 'wrap', gap: '0.5rem' }}>
                  <div style={{ fontSize: '1.1rem', fontWeight: 700, color: 'var(--status-failure-text)' }}>
                    VALIDATED COUNTEREXAMPLE — GREEDY FAILED
                  </div>
                  <StatusBadge type="counterexample" label="Suboptimal" />
                </div>
                <div style={{ fontSize: '0.9rem', color: 'var(--text-secondary)', marginTop: '0.35rem' }}>
                  {result.metrics?.interpretation ||
                    'On this instance, greedy heuristic yielded a suboptimal result compared to the exact dynamic programming oracle.'}
                </div>
              </div>
            </div>
          ) : (
            <div className="result-banner result-banner-success">
              <CheckCircle2 color="var(--status-success)" size={26} style={{ flexShrink: 0 }} />
              <div style={{ flex: 1 }}>
                <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', flexWrap: 'wrap', gap: '0.5rem' }}>
                  <div style={{ fontSize: '1.1rem', fontWeight: 700, color: 'var(--status-success-text)' }}>
                    VALIDATED INSTANCE — GREEDY MATCHED OPTIMAL
                  </div>
                  <StatusBadge type="optimal" label="Optimal Match" />
                </div>
                <div style={{ fontSize: '0.9rem', color: 'var(--text-secondary)', marginTop: '0.35rem' }}>
                  {result.metrics?.interpretation ||
                    'On this specific instance, the greedy algorithm achieved the exact optimal cost.'}
                </div>
              </div>
            </div>
          )}

          {/* Metrics Panel */}
          <div className="panel">
            <div className="panel-header">
              <div className="panel-title">Verification Metrics & Comparison</div>
              <span className="badge badge-neutral">{result.metrics?.objective}</span>
            </div>
            <div className="metrics-grid">
              <MetricCard
                label="Greedy Solution"
                value={result.metrics?.greedy_cost ?? 'Infeasible'}
                variant={result.counterexample_found ? 'red' : 'green'}
                sub={result.metrics?.greedy_solved ? 'Solver finished' : 'Solver failed'}
              />
              <MetricCard
                label="Optimal Oracle"
                value={result.metrics?.optimal_cost ?? 'Infeasible'}
                variant="green"
                sub="Exact DP benchmark"
              />
              <MetricCard
                label="Absolute Gap"
                value={result.metrics?.absolute_gap ?? 0}
                variant="blue"
                sub="|Greedy - Optimal|"
              />
              <MetricCard
                label="Suboptimality Ratio"
                value={
                  result.metrics?.ratio !== null && result.metrics?.ratio !== undefined
                    ? `${result.metrics.ratio.toFixed(2)}x`
                    : '1.00x'
                }
                variant={result.counterexample_found ? 'red' : 'green'}
                sub={
                  result.metrics?.overhead_percentage !== null &&
                  result.metrics?.overhead_percentage !== undefined
                    ? `Overhead: ${result.metrics.overhead_percentage.toFixed(1)}%`
                    : 'Overhead: 0.0%'
                }
              />
            </div>
          </div>

          {/* Divergence Panel */}
          <DivergencePanel divergence={result.divergence} />

          {/* Traces Table */}
          {result.greedy_solution?.trace && (
            <TraceTable
              greedyTrace={result.greedy_solution.trace}
              optimalTrace={result.optimal_solution?.trace}
              divergenceStep={result.divergence?.divergence_step}
            />
          )}
        </div>
      )}
    </div>
  );
};
