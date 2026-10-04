import React, { useState, useEffect } from 'react';
import type {
  ProblemMetadata,
  ProblemType,
  SearchStrategy,
  SearchMode,
  InvestigationRequest,
  InvestigationResponse,
} from '../types/api';
import { ApiError } from '../types/api';
import { getProblems } from '../api/problems';
import { runInvestigation } from '../api/investigate';
import { MetricCard } from '../components/MetricCard';
import { StatusBadge } from '../components/StatusBadge';
import { CandidateView } from '../components/CandidateView';
import { DivergencePanel } from '../components/DivergencePanel';
import { TraceTable } from '../components/TraceTable';
import { AlertTriangle, Clock, Sliders, CheckCircle2, ChevronDown, ChevronUp } from 'lucide-react';

export const BreakItPage: React.FC = () => {
  const [problems, setProblems] = useState<ProblemMetadata[]>([]);
  const [loadingProblems, setLoadingProblems] = useState<boolean>(true);
  const [problemsError, setProblemsError] = useState<string | null>(null);

  // Form State
  const [selectedProblemId, setSelectedProblemId] = useState<ProblemType>('coin_change');
  const [strategy, setStrategy] = useState<SearchStrategy>('BACKTRACKING');
  const [mode, setMode] = useState<SearchMode>('SMALLEST');
  const [maxCandidates, setMaxCandidates] = useState<number>(5000);
  const [randomSeed, setRandomSeed] = useState<number | undefined>(undefined);
  const [showAdvanced, setShowAdvanced] = useState<boolean>(false);

  // Problem-specific bounds
  // Coin Change
  const [minDenom, setMinDenom] = useState<number>(1);
  const [maxDenom, setMaxDenom] = useState<number>(10);
  const [minCoinCount, setMinCoinCount] = useState<number>(3);
  const [maxCoinCount, setMaxCoinCount] = useState<number>(4);
  const [minTarget, setMinTarget] = useState<number>(1);
  const [maxTarget, setMaxTarget] = useState<number>(12);
  const [requireUnitCoin, setRequireUnitCoin] = useState<boolean>(true);

  // Activity Selection
  const [minActivities, setMinActivities] = useState<number>(3);
  const [maxActivities, setMaxActivities] = useState<number>(6);
  const [minTime, setMinTime] = useState<number>(0);
  const [maxTime, setMaxTime] = useState<number>(15);
  const [minDuration, setMinDuration] = useState<number>(1);

  // Knapsack (0/1 and Fractional)
  const [minItems, setMinItems] = useState<number>(3);
  const [maxItems, setMaxItems] = useState<number>(4);
  const [minWeight, setMinWeight] = useState<number>(1);
  const [maxWeight, setMaxWeight] = useState<number>(10);
  const [minValue, setMinValue] = useState<number>(1);
  const [maxValue, setMaxValue] = useState<number>(20);
  const [minCapacity, setMinCapacity] = useState<number>(5);
  const [maxCapacity, setMaxCapacity] = useState<number>(15);

  // Execution & Result State
  const [isSearching, setIsSearching] = useState<boolean>(false);
  const [elapsedMs, setElapsedMs] = useState<number>(0);
  const [investigationResult, setInvestigationResult] = useState<InvestigationResponse | null>(null);
  const [investigationError, setInvestigationError] = useState<string | null>(null);

  // Load problem metadata from /api/v1/problems
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
      } catch (err: any) {
        if (mounted) {
          setProblemsError(err.message || 'Failed to load problem metadata');
          setLoadingProblems(false);
        }
      }
    };
    load();
    return () => {
      mounted = false;
    };
  }, []);

  // Timer while searching
  useEffect(() => {
    let interval: any = null;
    if (isSearching) {
      const start = Date.now();
      interval = setInterval(() => {
        setElapsedMs(Date.now() - start);
      }, 50);
    } else {
      clearInterval(interval);
    }
    return () => clearInterval(interval);
  }, [isSearching]);

  const activeProblem = problems.find((p) => p.id === selectedProblemId);

  // Quick preset handlers
  const applyPreset = (presetName: string) => {
    switch (presetName) {
      case 'coin-change-flaw':
        setSelectedProblemId('coin_change');
        setStrategy('BACKTRACKING');
        setMode('SMALLEST');
        setMinDenom(1);
        setMaxDenom(10);
        setMinCoinCount(3);
        setMaxCoinCount(4);
        setMinTarget(1);
        setMaxTarget(12);
        setRequireUnitCoin(true);
        setMaxCandidates(5000);
        break;
      case 'knapsack-01-flaw':
        setSelectedProblemId('knapsack_01');
        setStrategy('HEURISTIC');
        setMode('ANY');
        setMinItems(3);
        setMaxItems(4);
        setMinWeight(1);
        setMaxWeight(8);
        setMinValue(1);
        setMaxValue(20);
        setMinCapacity(5);
        setMaxCapacity(15);
        setMaxCandidates(5000);
        break;
      case 'activity-selection-optimal':
        setSelectedProblemId('activity_selection');
        setStrategy('EXHAUSTIVE');
        setMode('ANY');
        setMinActivities(3);
        setMaxActivities(5);
        setMinTime(0);
        setMaxTime(12);
        setMinDuration(1);
        setMaxCandidates(2000);
        break;
      case 'fractional-knapsack-optimal':
        setSelectedProblemId('knapsack_fractional');
        setStrategy('RANDOMIZED');
        setMode('ANY');
        setMinItems(3);
        setMaxItems(5);
        setMinWeight(1);
        setMaxWeight(10);
        setMinValue(2);
        setMaxValue(25);
        setMinCapacity(6);
        setMaxCapacity(15);
        setMaxCandidates(2500);
        break;
    }
  };

  const handleStartInvestigation = async () => {
    setIsSearching(true);
    setInvestigationError(null);
    setInvestigationResult(null);

    const req: InvestigationRequest = {
      problem: selectedProblemId,
      strategy,
      mode,
      max_candidates: maxCandidates,
      random_seed: randomSeed,
    };

    if (selectedProblemId === 'coin_change') {
      req.min_denomination = minDenom;
      req.max_denomination = maxDenom;
      req.min_coin_count = minCoinCount;
      req.max_coin_count = maxCoinCount;
      req.min_target = minTarget;
      req.max_target = maxTarget;
      req.require_unit_coin = requireUnitCoin;
    } else if (selectedProblemId === 'activity_selection') {
      req.min_activities = minActivities;
      req.max_activities = maxActivities;
      req.min_time = minTime;
      req.max_time = maxTime;
      req.min_duration = minDuration;
    } else if (selectedProblemId === 'knapsack_01' || selectedProblemId === 'knapsack_fractional') {
      req.min_items = minItems;
      req.max_items = maxItems;
      req.min_weight = minWeight;
      req.max_weight = maxWeight;
      req.min_value = minValue;
      req.max_value = maxValue;
      req.min_capacity = minCapacity;
      req.max_capacity = maxCapacity;
    }

    try {
      const response = await runInvestigation(req);
      setInvestigationResult(response);
    } catch (err: any) {
      if (err instanceof ApiError) {
        setInvestigationError(`[${err.code}] ${err.message}`);
      } else {
        setInvestigationError(err.message || 'An unexpected error occurred during investigation.');
      }
    } finally {
      setIsSearching(false);
    }
  };

  return (
    <div style={{ display: 'flex', flexDirection: 'column', gap: '2.5rem' }}>
      {/* 1. Page Header */}
      <div>
        <h1 style={{ fontSize: '2rem', marginBottom: '0.4rem' }}>Break It</h1>
        <p style={{ color: 'var(--text-muted)', fontSize: '1.05rem', margin: 0 }}>
          Find an input where the greedy strategy fails against an exact oracle.
        </p>
      </div>

      {/* 2. Investigation Workspace Form */}
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
            Investigation Presets:
          </span>
          <div style={{ display: 'flex', gap: '0.5rem', flexWrap: 'wrap' }}>
            <button
              type="button"
              className="btn btn-secondary"
              style={{ fontSize: '0.775rem', padding: '0.25rem 0.65rem' }}
              onClick={() => applyPreset('coin-change-flaw')}
            >
              Coin Change Suboptimality
            </button>
            <button
              type="button"
              className="btn btn-secondary"
              style={{ fontSize: '0.775rem', padding: '0.25rem 0.65rem' }}
              onClick={() => applyPreset('knapsack-01-flaw')}
            >
              0/1 Knapsack Density Flaw
            </button>
            <button
              type="button"
              className="btn btn-secondary"
              style={{ fontSize: '0.775rem', padding: '0.25rem 0.65rem' }}
              onClick={() => applyPreset('activity-selection-optimal')}
            >
              Activity Selection (Optimal)
            </button>
            <button
              type="button"
              className="btn btn-secondary"
              style={{ fontSize: '0.775rem', padding: '0.25rem 0.65rem' }}
              onClick={() => applyPreset('fractional-knapsack-optimal')}
            >
              Fractional Knapsack (Optimal)
            </button>
          </div>
        </div>

        {loadingProblems ? (
          <div style={{ padding: '2rem', textAlign: 'center', color: 'var(--text-muted)' }}>
            <div className="spinner" style={{ marginRight: '0.5rem' }} /> Loading problem metadata from C++ backend...
          </div>
        ) : problemsError ? (
          <div style={{ color: 'var(--status-failure-text)', padding: '1rem', background: 'var(--status-failure-bg)', borderRadius: 'var(--radius-md)' }}>
            ⚠ {problemsError}
          </div>
        ) : (
          <div>
            {/* Top row: Problem, Strategy, Mode */}
            <div
              style={{
                display: 'grid',
                gridTemplateColumns: 'repeat(auto-fit, minmax(240px, 1fr))',
                gap: '1.25rem',
                marginBottom: '1.5rem',
              }}
            >
              <div className="form-group" style={{ margin: 0 }}>
                <label className="form-label" htmlFor="problem-select">Target Problem</label>
                <select
                  id="problem-select"
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
                {activeProblem && (
                  <div style={{ fontSize: '0.775rem', color: 'var(--text-muted)', marginTop: '0.35rem' }}>
                    {activeProblem.greedy_strategy} vs. {activeProblem.oracle_strategy}
                  </div>
                )}
              </div>

              <div className="form-group" style={{ margin: 0 }}>
                <label className="form-label" htmlFor="strategy-select">Search Strategy</label>
                <select
                  id="strategy-select"
                  className="form-control form-select"
                  value={strategy}
                  onChange={(e) => setStrategy(e.target.value as SearchStrategy)}
                >
                  <option value="BACKTRACKING">Backtracking (Branch pruning)</option>
                  <option value="EXHAUSTIVE">Exhaustive (Full space search)</option>
                  <option value="RANDOMIZED">Randomized (Monte Carlo sampling)</option>
                  <option value="HEURISTIC">Heuristic (Targeted ratio search)</option>
                </select>
                <div style={{ fontSize: '0.775rem', color: 'var(--text-muted)', marginTop: '0.35rem' }}>
                  Determines how candidate inputs are explored.
                </div>
              </div>

              <div className="form-group" style={{ margin: 0 }}>
                <label className="form-label" htmlFor="mode-select">Investigation Mode</label>
                <select
                  id="mode-select"
                  className="form-control form-select"
                  value={mode}
                  onChange={(e) => setMode(e.target.value as SearchMode)}
                >
                  <option value="SMALLEST">Smallest (Minimal candidate size)</option>
                  <option value="ANY">Any (First counterexample discovered)</option>
                  <option value="WORST">Worst (Maximum suboptimality gap)</option>
                </select>
                <div style={{ fontSize: '0.775rem', color: 'var(--text-muted)', marginTop: '0.35rem' }}>
                  Target optimization criterion for search.
                </div>
              </div>
            </div>

            {/* Problem-Specific Search Space Bounds */}
            <div
              style={{
                background: 'var(--bg-subtle)',
                border: '1px solid var(--border-subtle)',
                borderRadius: 'var(--radius-md)',
                padding: '1.25rem',
                marginBottom: '1.5rem',
              }}
            >
              <div style={{ fontSize: '0.825rem', fontWeight: 600, color: 'var(--text-secondary)', marginBottom: '0.75rem', textTransform: 'uppercase' }}>
                Search Space Bounds & Parameters
              </div>

              {selectedProblemId === 'coin_change' && (
                <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))', gap: '1rem' }}>
                  <div className="form-group" style={{ margin: 0 }}>
                    <label className="form-label">Denomination Range</label>
                    <div style={{ display: 'flex', gap: '0.5rem', alignItems: 'center' }}>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={minDenom}
                        onChange={(e) => setMinDenom(parseInt(e.target.value) || 1)}
                        min={1}
                      />
                      <span style={{ color: 'var(--text-muted)' }}>to</span>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={maxDenom}
                        onChange={(e) => setMaxDenom(parseInt(e.target.value) || 10)}
                        min={minDenom}
                      />
                    </div>
                  </div>

                  <div className="form-group" style={{ margin: 0 }}>
                    <label className="form-label">Number of Coins</label>
                    <div style={{ display: 'flex', gap: '0.5rem', alignItems: 'center' }}>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={minCoinCount}
                        onChange={(e) => setMinCoinCount(parseInt(e.target.value) || 2)}
                        min={2}
                      />
                      <span style={{ color: 'var(--text-muted)' }}>to</span>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={maxCoinCount}
                        onChange={(e) => setMaxCoinCount(parseInt(e.target.value) || 4)}
                        min={minCoinCount}
                      />
                    </div>
                  </div>

                  <div className="form-group" style={{ margin: 0 }}>
                    <label className="form-label">Target Amounts</label>
                    <div style={{ display: 'flex', gap: '0.5rem', alignItems: 'center' }}>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={minTarget}
                        onChange={(e) => setMinTarget(parseInt(e.target.value) || 1)}
                        min={1}
                      />
                      <span style={{ color: 'var(--text-muted)' }}>to</span>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={maxTarget}
                        onChange={(e) => setMaxTarget(parseInt(e.target.value) || 12)}
                        min={minTarget}
                      />
                    </div>
                  </div>

                  <div className="form-group" style={{ display: 'flex', alignItems: 'center', margin: 'auto 0 0' }}>
                    <label style={{ display: 'flex', alignItems: 'center', gap: '0.5rem', cursor: 'pointer' }}>
                      <input
                        type="checkbox"
                        checked={requireUnitCoin}
                        onChange={(e) => setRequireUnitCoin(e.target.checked)}
                      />
                      <span style={{ fontSize: '0.85rem', fontWeight: 500 }}>Require 1-cent Unit Coin</span>
                    </label>
                  </div>
                </div>
              )}

              {selectedProblemId === 'activity_selection' && (
                <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))', gap: '1rem' }}>
                  <div className="form-group" style={{ margin: 0 }}>
                    <label className="form-label">Activities Count</label>
                    <div style={{ display: 'flex', gap: '0.5rem', alignItems: 'center' }}>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={minActivities}
                        onChange={(e) => setMinActivities(parseInt(e.target.value) || 2)}
                        min={2}
                      />
                      <span style={{ color: 'var(--text-muted)' }}>to</span>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={maxActivities}
                        onChange={(e) => setMaxActivities(parseInt(e.target.value) || 6)}
                        min={minActivities}
                      />
                    </div>
                  </div>

                  <div className="form-group" style={{ margin: 0 }}>
                    <label className="form-label">Time Window Horizon</label>
                    <div style={{ display: 'flex', gap: '0.5rem', alignItems: 'center' }}>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={minTime}
                        onChange={(e) => setMinTime(parseInt(e.target.value) || 0)}
                        min={0}
                      />
                      <span style={{ color: 'var(--text-muted)' }}>to</span>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={maxTime}
                        onChange={(e) => setMaxTime(parseInt(e.target.value) || 15)}
                        min={minTime + 1}
                      />
                    </div>
                  </div>
                </div>
              )}

              {(selectedProblemId === 'knapsack_01' || selectedProblemId === 'knapsack_fractional') && (
                <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(180px, 1fr))', gap: '1rem' }}>
                  <div className="form-group" style={{ margin: 0 }}>
                    <label className="form-label">Items Count</label>
                    <div style={{ display: 'flex', gap: '0.5rem', alignItems: 'center' }}>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={minItems}
                        onChange={(e) => setMinItems(parseInt(e.target.value) || 2)}
                        min={2}
                      />
                      <span style={{ color: 'var(--text-muted)' }}>to</span>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={maxItems}
                        onChange={(e) => setMaxItems(parseInt(e.target.value) || 4)}
                        min={minItems}
                      />
                    </div>
                  </div>

                  <div className="form-group" style={{ margin: 0 }}>
                    <label className="form-label">Capacity (W)</label>
                    <div style={{ display: 'flex', gap: '0.5rem', alignItems: 'center' }}>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={minCapacity}
                        onChange={(e) => setMinCapacity(parseInt(e.target.value) || 1)}
                        min={1}
                      />
                      <span style={{ color: 'var(--text-muted)' }}>to</span>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={maxCapacity}
                        onChange={(e) => setMaxCapacity(parseInt(e.target.value) || 15)}
                        min={minCapacity}
                      />
                    </div>
                  </div>

                  <div className="form-group" style={{ margin: 0 }}>
                    <label className="form-label">Weights</label>
                    <div style={{ display: 'flex', gap: '0.5rem', alignItems: 'center' }}>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={minWeight}
                        onChange={(e) => setMinWeight(parseInt(e.target.value) || 1)}
                        min={1}
                      />
                      <span style={{ color: 'var(--text-muted)' }}>to</span>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={maxWeight}
                        onChange={(e) => setMaxWeight(parseInt(e.target.value) || 10)}
                        min={minWeight}
                      />
                    </div>
                  </div>

                  <div className="form-group" style={{ margin: 0 }}>
                    <label className="form-label">Values</label>
                    <div style={{ display: 'flex', gap: '0.5rem', alignItems: 'center' }}>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={minValue}
                        onChange={(e) => setMinValue(parseInt(e.target.value) || 1)}
                        min={1}
                      />
                      <span style={{ color: 'var(--text-muted)' }}>to</span>
                      <input
                        type="number"
                        className="form-control font-mono"
                        value={maxValue}
                        onChange={(e) => setMaxValue(parseInt(e.target.value) || 20)}
                        min={minValue}
                      />
                    </div>
                  </div>
                </div>
              )}
            </div>

            {/* Advanced Options Toggle */}
            <div style={{ marginBottom: '1.5rem' }}>
              <button
                type="button"
                className="btn btn-secondary"
                style={{ fontSize: '0.8rem', padding: '0.35rem 0.75rem' }}
                onClick={() => setShowAdvanced(!showAdvanced)}
              >
                <Sliders size={13} />
                <span>{showAdvanced ? 'Hide Safety Limits' : 'Configure Safety Limits & Seed'}</span>
                {showAdvanced ? <ChevronUp size={13} /> : <ChevronDown size={13} />}
              </button>

              {showAdvanced && (
                <div
                  style={{
                    display: 'grid',
                    gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))',
                    gap: '1rem',
                    marginTop: '1rem',
                    padding: '1rem',
                    border: '1px solid var(--border-subtle)',
                    borderRadius: 'var(--radius-sm)',
                    background: '#ffffff',
                  }}
                >
                  <div className="form-group" style={{ margin: 0 }}>
                    <label className="form-label">Maximum Candidates Evaluated</label>
                    <input
                      type="number"
                      className="form-control font-mono"
                      value={maxCandidates}
                      onChange={(e) => setMaxCandidates(parseInt(e.target.value) || 1000)}
                      min={10}
                      max={100000}
                    />
                  </div>

                  <div className="form-group" style={{ margin: 0 }}>
                    <label className="form-label">Random Seed (Optional)</label>
                    <input
                      type="number"
                      className="form-control font-mono"
                      placeholder="Auto (entropy)"
                      value={randomSeed ?? ''}
                      onChange={(e) =>
                        setRandomSeed(e.target.value ? parseInt(e.target.value) : undefined)
                      }
                    />
                  </div>
                </div>
              )}
            </div>

            {/* Main Action Button */}
            <div style={{ display: 'flex', justifyContent: 'flex-start' }}>
              <button
                type="button"
                className="btn btn-break btn-lg"
                disabled={isSearching || loadingProblems}
                onClick={handleStartInvestigation}
              >
                {isSearching ? (
                  <>
                    <span className="spinner" />
                    <span>Searching instance space... ({elapsedMs}ms)</span>
                  </>
                ) : (
                  <span>BREAK IT</span>
                )}
              </button>
            </div>
          </div>
        )}
      </div>

      {/* Investigation Error */}
      {investigationError && (
        <div className="result-banner result-banner-failure">
          <AlertTriangle color="var(--status-failure)" size={22} style={{ flexShrink: 0 }} />
          <div>
            <div style={{ fontWeight: 600, color: 'var(--status-failure-text)', marginBottom: '0.2rem' }}>
              Investigation Engine Error
            </div>
            <div style={{ fontSize: '0.875rem', color: 'var(--text-secondary)' }}>
              {investigationError}
            </div>
          </div>
        </div>
      )}

      {/* 3. Formal Investigation Result Report */}
      {investigationResult && (
        <div style={{ display: 'flex', flexDirection: 'column', gap: '1.75rem' }}>
          <div>
            <div style={{ fontSize: '0.8rem', fontWeight: 700, color: 'var(--accent-primary)', textTransform: 'uppercase', letterSpacing: '0.04em', marginBottom: '0.25rem' }}>
              Investigation Result
            </div>
            <h2 style={{ fontSize: '1.5rem', margin: 0 }}>
              {investigationResult.counterexample_found
                ? 'Counterexample Found'
                : 'No Counterexample Found'}
            </h2>
          </div>

          {/* Status Banner */}
          {investigationResult.counterexample_found ? (
            <div className="result-banner result-banner-failure">
              <AlertTriangle color="var(--status-failure)" size={26} style={{ flexShrink: 0 }} />
              <div style={{ flex: 1 }}>
                <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', flexWrap: 'wrap', gap: '0.5rem' }}>
                  <div style={{ fontSize: '1.1rem', fontWeight: 700, color: 'var(--status-failure-text)' }}>
                    COUNTEREXAMPLE DISCOVERED!
                  </div>
                  <StatusBadge type="counterexample" label="Greedy Suboptimal" />
                </div>
                <div style={{ fontSize: '0.9rem', color: 'var(--text-secondary)', marginTop: '0.35rem' }}>
                  {investigationResult.candidate_summary ||
                    'The greedy strategy made a locally optimal choice that prevented reaching global optimality.'}
                </div>
              </div>
            </div>
          ) : (
            <div className="result-banner result-banner-info">
              <CheckCircle2 color="var(--status-info)" size={26} style={{ flexShrink: 0 }} />
              <div style={{ flex: 1 }}>
                <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', flexWrap: 'wrap', gap: '0.5rem' }}>
                  <div style={{ fontSize: '1.05rem', fontWeight: 700, color: 'var(--status-info-text)' }}>
                    {investigationResult.message || 'No counterexample was found within the configured search space.'}
                  </div>
                  <StatusBadge type="info" label="Search Bounded" />
                </div>
                <div style={{ fontSize: '0.875rem', color: 'var(--text-secondary)', marginTop: '0.35rem', lineHeight: '1.5' }}>
                  <strong>Scientific Notice:</strong> {investigationResult.message}. The bounded search space was explored without discovering any suboptimality. Note that finite search exhaustion does not constitute mathematical proof of global greedy correctness unless supported by structural matroid theory.
                </div>
              </div>
            </div>
          )}

          {/* Candidate & Metrics Comparison (if counterexample found) */}
          {investigationResult.counterexample_found && (
            <>
              <div className="grid-2">
                {/* Candidate Instance */}
                <div className="panel">
                  <div className="panel-header">
                    <div className="panel-title">Forensic Candidate Instance</div>
                    <span className="badge badge-neutral">Minimal Discovery</span>
                  </div>
                  <CandidateView
                    problem={investigationResult.problem}
                    candidate={investigationResult.candidate}
                  />
                </div>

                {/* Optimality Gap */}
                <div className="panel">
                  <div className="panel-header">
                    <div className="panel-title">Optimality Gap & Overhead</div>
                  </div>
                  <div className="metrics-grid">
                    <MetricCard
                      label="Greedy Solution"
                      value={investigationResult.metrics?.greedy_cost ?? 'N/A'}
                      variant="red"
                      sub="Suboptimal cost"
                    />
                    <MetricCard
                      label="Optimal Solution"
                      value={investigationResult.metrics?.optimal_cost ?? 'N/A'}
                      variant="green"
                      sub="Exact DP Oracle"
                    />
                    <MetricCard
                      label="Absolute Gap"
                      value={investigationResult.metrics?.absolute_gap ?? 0}
                      variant="blue"
                      sub="|Greedy - Optimal|"
                    />
                    <MetricCard
                      label="Overhead"
                      value={
                        investigationResult.metrics?.overhead_percentage !== null &&
                        investigationResult.metrics?.overhead_percentage !== undefined
                          ? `${investigationResult.metrics.overhead_percentage.toFixed(1)}%`
                          : 'N/A'
                      }
                      variant="red"
                      sub={
                        investigationResult.metrics?.ratio
                          ? `Ratio: ${investigationResult.metrics.ratio.toFixed(2)}x`
                          : undefined
                      }
                    />
                  </div>
                </div>
              </div>

              {/* First Divergence Rationale */}
              <DivergencePanel divergence={investigationResult.divergence} />

              {/* Dual Execution Traces */}
              {investigationResult.greedy_solution?.trace && (
                <TraceTable
                  greedyTrace={investigationResult.greedy_solution.trace}
                  optimalTrace={investigationResult.optimal_solution?.trace}
                  divergenceStep={investigationResult.divergence?.divergence_step}
                />
              )}
            </>
          )}

          {/* Search Statistics */}
          <div className="panel">
            <div className="panel-header">
              <div className="panel-title">
                <Clock size={16} />
                <span>Search Statistics</span>
              </div>
              <span className="font-mono" style={{ fontSize: '0.8rem', color: 'var(--text-muted)' }}>
                Termination: {investigationResult.search_statistics?.termination_reason || 'COMPLETED'}
              </span>
            </div>
            <div className="metrics-grid">
              <MetricCard
                label="Candidates Evaluated"
                value={investigationResult.search_statistics?.candidates_evaluated?.toLocaleString() ?? 0}
                sub={`Generated: ${investigationResult.search_statistics?.candidates_generated ?? 0}`}
              />
              <MetricCard
                label="Search Duration"
                value={`${investigationResult.search_statistics?.duration_ms?.toFixed(2) ?? 0} ms`}
                sub="C++ Engine Execution"
              />
              <MetricCard
                label="Nodes Explored"
                value={investigationResult.search_statistics?.nodes_explored?.toLocaleString() ?? 0}
                sub={`Pruned: ${investigationResult.search_statistics?.branches_pruned ?? 0}`}
              />
              <MetricCard
                label="Minimality Status"
                value={investigationResult.minimality?.verified ? 'Verified' : 'Unchecked'}
                variant={investigationResult.minimality?.verified ? 'green' : 'default'}
                sub={investigationResult.minimality?.description || 'N/A'}
              />
            </div>
          </div>
        </div>
      )}
    </div>
  );
};
