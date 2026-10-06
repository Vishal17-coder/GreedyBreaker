// =============================================================================
// Experiment Creation Wizard — 4-step guided flow
// =============================================================================
import { useState, useEffect, useCallback, useMemo } from 'react';
import { useNavigate } from 'react-router-dom';
import { experimentsApi, problemsApi, getErrorMessage } from '../api';
import type {
  ProblemMetadata,
  ProblemType,
  SearchMode,
  SearchStrategy,
  CreateExperimentRequest,
} from '../types';
import { SEARCH_MODE_INFO, SEARCH_STRATEGY_INFO } from '../types';
import { Breadcrumb, LoadingPage, ErrorState, MetricCard } from '../components/Shared';
import { formatNumber } from '../utils/format';

type Step = 1 | 2 | 3 | 4;

// --- Default search spaces per problem -----------------------------------
function defaultSearchSpace(problemType: ProblemType): Record<string, unknown> {
  switch (problemType) {
    case 'coin_change':
      return {
        denomination_count: { min: 3, max: 3 },
        denomination_range: { min: 1, max: 10 },
        target_range: { min: 1, max: 20 },
      };
    case 'knapsack_01':
      return {
        item_count: { min: 2, max: 5 },
        weight_range: { min: 1, max: 20 },
        value_range: { min: 1, max: 50 },
        capacity_range: { min: 10, max: 50 },
      };
    case 'activity_selection':
      return {
        activity_count: { min: 3, max: 6 },
        time_range: { min: 0, max: 20 },
        duration_range: { min: 1, max: 10 },
      };
    case 'fractional_knapsack':
      return {
        item_count: { min: 2, max: 5 },
        weight_range: { min: 1, max: 20 },
        value_range: { min: 1, max: 50 },
        capacity_range: { min: 10, max: 50 },
      };
    case 'huffman_coding':
      return {
        symbol_count: { min: 4, max: 8 },
        frequency_range: { min: 1, max: 100 },
      };
    default:
      return {};
  }
}

// --- Search space form per problem ---------------------------------------
function SearchSpaceForm({
  problemType,
  space,
  onChange,
}: {
  problemType: ProblemType;
  space: Record<string, { min: number; max: number }>;
  onChange: (key: string, field: 'min' | 'max', val: number) => void;
}) {
  const fields: Array<{ key: string; label: string; hint: string }> = (() => {
    switch (problemType) {
      case 'coin_change':
        return [
          { key: 'denomination_count', label: 'Denomination Count (K)', hint: 'Number of coin types in each candidate' },
          { key: 'denomination_range', label: 'Denomination Value Range', hint: 'Possible coin values (base coin 1 always included)' },
          { key: 'target_range', label: 'Target Value (T)', hint: 'Amount to make change for' },
        ];
      case 'knapsack_01':
      case 'fractional_knapsack':
        return [
          { key: 'item_count', label: 'Item Count (N)', hint: 'Number of items in each candidate instance' },
          { key: 'weight_range', label: 'Item Weight Range (w)', hint: 'Possible weight values per item' },
          { key: 'value_range', label: 'Item Value Range (v)', hint: 'Possible value values per item' },
          { key: 'capacity_range', label: 'Knapsack Capacity (W)', hint: 'Weight capacity of the knapsack' },
        ];
      case 'activity_selection':
        return [
          { key: 'activity_count', label: 'Activity Count (N)', hint: 'Number of activities in each candidate instance' },
          { key: 'time_range', label: 'Earliest Start Time Range', hint: 'Coordinate range for activity start times' },
          { key: 'duration_range', label: 'Activity Duration Range', hint: 'Possible length/duration bounds for activities' },
        ];
      case 'huffman_coding':
        return [
          { key: 'symbol_count', label: 'Symbol Count', hint: 'Number of distinct symbols per candidate' },
          { key: 'frequency_range', label: 'Frequency Range', hint: 'Possible symbol frequency values' },
        ];
      default:
        return [];
    }
  })();

  return (
    <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(280px, 1fr))', gap: 'var(--space-4)' }}>
      {fields.map(f => (
        <div key={f.key} className="card">
          <div className="card-body" style={{ padding: 'var(--space-4)' }}>
            <label className="form-label" style={{ marginBottom: 'var(--space-1)' }}>
              {f.label}
            </label>
            <p className="form-hint" style={{ marginBottom: 'var(--space-3)' }}>{f.hint}</p>
            <div className="flex gap-3 items-center">
              <div className="form-group" style={{ flex: 1 }}>
                <label className="form-hint" htmlFor={`${f.key}-min`}>Min</label>
                <input
                  id={`${f.key}-min`}
                  type="number"
                  className="input mono"
                  value={space[f.key]?.min ?? 1}
                  min={1}
                  onChange={e => onChange(f.key, 'min', parseInt(e.target.value) || 1)}
                />
              </div>
              <span style={{ color: 'var(--color-text-tertiary)', marginTop: '18px' }}>–</span>
              <div className="form-group" style={{ flex: 1 }}>
                <label className="form-hint" htmlFor={`${f.key}-max`}>Max</label>
                <input
                  id={`${f.key}-max`}
                  type="number"
                  className="input mono"
                  value={space[f.key]?.max ?? 10}
                  min={space[f.key]?.min ?? 1}
                  onChange={e => onChange(f.key, 'max', parseInt(e.target.value) || 1)}
                />
              </div>
            </div>
          </div>
        </div>
      ))}
    </div>
  );
}

// --- Problem Selector Step -----------------------------------------------
function Step1Problem({
  problems,
  selectedProblemId,
  selectedStrategyId,
  selectedOracleId,
  onSelectProblem,
  onSelectStrategy,
  onSelectOracle,
}: {
  problems: ProblemMetadata[];
  selectedProblemId: string;
  selectedStrategyId: string;
  selectedOracleId: string;
  onSelectProblem: (id: ProblemType, defaultStrategy: string, defaultOracle: string) => void;
  onSelectStrategy: (id: string) => void;
  onSelectOracle: (id: string) => void;
}) {
  const selectedProblem = problems.find(p => p.id === selectedProblemId);

  return (
    <div>
      <h2 style={{ marginBottom: 'var(--space-2)' }}>Choose Problem Family</h2>
      <p className="text-secondary" style={{ marginBottom: 'var(--space-6)' }}>
        Select the algorithmic problem to investigate. The available greedy strategies and exact oracles are determined by your choice.
      </p>

      <div className="problem-grid" role="radiogroup" aria-label="Problem family selection">
        {problems.map(p => (
          <div
            key={p.id}
            className={`problem-card${selectedProblemId === p.id ? ' selected' : ''}`}
            role="radio"
            aria-checked={selectedProblemId === p.id}
            tabIndex={0}
            onClick={() => {
              const defStrategy = p.strategies.find(s => s.default)?.id ?? p.strategies[0]?.id ?? 'default';
              const defOracle = p.oracles.find(o => o.default)?.id ?? p.oracles[0]?.id ?? 'default';
              onSelectProblem(p.id as ProblemType, defStrategy, defOracle);
            }}
            onKeyDown={e => {
              if (e.key === 'Enter' || e.key === ' ') {
                e.preventDefault();
                const defStrategy = p.strategies.find(s => s.default)?.id ?? 'default';
                const defOracle = p.oracles.find(o => o.default)?.id ?? 'default';
                onSelectProblem(p.id as ProblemType, defStrategy, defOracle);
              }
            }}
          >
            <div className="flex items-center justify-between mb-3">
              <span className={`badge ${p.objective === 'MINIMIZE' ? 'badge-primary' : 'badge-success'}`}>
                {p.objective}
              </span>
              {selectedProblemId === p.id && (
                <span className="badge badge-primary">Selected ✓</span>
              )}
            </div>
            <h3 style={{ marginBottom: 'var(--space-2)' }}>{p.name}</h3>
            <p className="text-secondary text-sm">{p.description}</p>
          </div>
        ))}
      </div>

      {selectedProblem && (
        <div style={{ marginTop: 'var(--space-6)', display: 'grid', gridTemplateColumns: '1fr 1fr', gap: 'var(--space-4)' }}>
          <div className="form-group">
            <label className="form-label" htmlFor="strategy-select">Greedy Strategy</label>
            <p className="form-hint">The heuristic whose behavior will be investigated.</p>
            <select
              id="strategy-select"
              className="select"
              value={selectedStrategyId}
              onChange={e => onSelectStrategy(e.target.value)}
            >
              {selectedProblem.strategies.map(s => (
                <option key={s.id} value={s.id}>{s.name}</option>
              ))}
            </select>
          </div>

          <div className="form-group">
            <label className="form-label" htmlFor="oracle-select">Ground-Truth Oracle</label>
            <p className="form-hint">The exact algorithm used to verify counterexamples.</p>
            <select
              id="oracle-select"
              className="select"
              value={selectedOracleId}
              onChange={e => onSelectOracle(e.target.value)}
            >
              {selectedProblem.oracles.map(o => (
                <option key={o.id} value={o.id}>
                  {o.name} [{o.confidence}]
                </option>
              ))}
            </select>
          </div>
        </div>
      )}
    </div>
  );
}

// --- Search Space Step ---------------------------------------------------
function Step2SearchSpace({
  problemType,
  searchSpace,
  onUpdate,
}: {
  problemType: ProblemType;
  searchSpace: Record<string, { min: number; max: number }>;
  onUpdate: (key: string, field: 'min' | 'max', val: number) => void;
}) {
  return (
    <div>
      <h2 style={{ marginBottom: 'var(--space-2)' }}>Configure Search Space</h2>
      <p className="text-secondary" style={{ marginBottom: 'var(--space-6)' }}>
        Define the bounds of the candidate search space. Larger spaces increase discovery probability but require more computation.
        The estimated space size is calculated from your input ranges.
      </p>
      <SearchSpaceForm problemType={problemType} space={searchSpace} onChange={onUpdate} />
    </div>
  );
}

// --- Search Mode Step ----------------------------------------------------
function Step3SearchMode({
  mode,
  strategy,
  maxCandidates,
  maxTimeMs,
  randomSeed,
  onChangeMode,
  onChangeStrategy,
  onChangeCandidates,
  onChangeTime,
  onChangeSeed,
}: {
  mode: SearchMode;
  strategy: SearchStrategy;
  maxCandidates: number;
  maxTimeMs: number;
  randomSeed: number;
  onChangeMode: (m: SearchMode) => void;
  onChangeStrategy: (s: SearchStrategy) => void;
  onChangeCandidates: (n: number) => void;
  onChangeTime: (n: number) => void;
  onChangeSeed: (n: number) => void;
}) {
  const modes: SearchMode[] = ['ANY', 'SMALLEST', 'WORST', 'PROFILE'];
  const strategies: SearchStrategy[] = ['EXHAUSTIVE', 'RANDOMIZED', 'GUIDED', 'HYBRID'];

  return (
    <div>
      <h2 style={{ marginBottom: 'var(--space-2)' }}>Search Mode & Strategy</h2>
      <p className="text-secondary" style={{ marginBottom: 'var(--space-6)' }}>
        Select what the search optimizes for and how candidates are generated.
      </p>

      <h3 style={{ marginBottom: 'var(--space-3)' }}>Search Mode</h3>
      <div
        style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(220px, 1fr))', gap: 'var(--space-3)', marginBottom: 'var(--space-6)' }}
        role="radiogroup"
        aria-label="Search mode"
      >
        {modes.map(m => (
          <div
            key={m}
            className={`problem-card${mode === m ? ' selected' : ''}`}
            style={{ cursor: 'pointer' }}
            role="radio"
            aria-checked={mode === m}
            tabIndex={0}
            onClick={() => onChangeMode(m)}
            onKeyDown={e => (e.key === 'Enter' || e.key === ' ') && (e.preventDefault(), onChangeMode(m))}
          >
            <div className="flex items-center justify-between mb-2">
              <code style={{ fontFamily: 'var(--font-mono)', fontSize: '13px', fontWeight: 700 }}>{m}</code>
              {mode === m && <span className="badge badge-primary">✓</span>}
            </div>
            <p className="text-secondary text-sm">{SEARCH_MODE_INFO[m].description}</p>
          </div>
        ))}
      </div>

      <h3 style={{ marginBottom: 'var(--space-3)' }}>Search Strategy</h3>
      <div
        style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(220px, 1fr))', gap: 'var(--space-3)', marginBottom: 'var(--space-6)' }}
        role="radiogroup"
        aria-label="Search strategy"
      >
        {strategies.map(s => (
          <div
            key={s}
            className={`problem-card${strategy === s ? ' selected' : ''}`}
            style={{ cursor: 'pointer' }}
            role="radio"
            aria-checked={strategy === s}
            tabIndex={0}
            onClick={() => onChangeStrategy(s)}
            onKeyDown={e => (e.key === 'Enter' || e.key === ' ') && (e.preventDefault(), onChangeStrategy(s))}
          >
            <div className="flex items-center justify-between mb-2">
              <code style={{ fontFamily: 'var(--font-mono)', fontSize: '13px', fontWeight: 700 }}>{s}</code>
              {strategy === s && <span className="badge badge-primary">✓</span>}
            </div>
            <p className="text-secondary text-sm">{SEARCH_STRATEGY_INFO[s].description}</p>
          </div>
        ))}
      </div>

      <h3 style={{ marginBottom: 'var(--space-3)' }}>Resource Limits</h3>
      <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(200px, 1fr))', gap: 'var(--space-4)' }}>
        <div className="form-group">
          <label className="form-label" htmlFor="max-candidates">Max Candidates</label>
          <p className="form-hint">Upper bound on instances evaluated (1–100,000)</p>
          <input
            id="max-candidates"
            type="number"
            className="input mono"
            value={maxCandidates}
            min={1}
            max={100000}
            onChange={e => onChangeCandidates(parseInt(e.target.value) || 1000)}
          />
        </div>
        <div className="form-group">
          <label className="form-label" htmlFor="max-time">Max Time (ms)</label>
          <p className="form-hint">Execution timeout (100–30,000 ms)</p>
          <input
            id="max-time"
            type="number"
            className="input mono"
            value={maxTimeMs}
            min={100}
            max={30000}
            onChange={e => onChangeTime(parseInt(e.target.value) || 5000)}
          />
        </div>
        <div className="form-group">
          <label className="form-label" htmlFor="random-seed">Random Seed</label>
          <p className="form-hint">Controls reproducibility. Same seed → same results.</p>
          <input
            id="random-seed"
            type="number"
            className="input mono"
            value={randomSeed}
            onChange={e => onChangeSeed(parseInt(e.target.value) || 42)}
          />
        </div>
      </div>
    </div>
  );
}

// --- Pre-flight Review Step ----------------------------------------------
function Step4Review({
  name,
  description,
  problemType,
  problems,
  strategyId,
  oracleId,
  mode,
  strategy,
  searchSpace,
  maxCandidates,
  maxTimeMs,
  randomSeed,
  onChangeName,
  onChangeDescription,
  validationResult,
  submitError,
}: {
  name: string;
  description: string;
  problemType: ProblemType;
  problems: ProblemMetadata[];
  strategyId: string;
  oracleId: string;
  mode: SearchMode;
  strategy: SearchStrategy;
  searchSpace: Record<string, { min: number; max: number }>;
  maxCandidates: number;
  maxTimeMs: number;
  randomSeed: number;
  onChangeName: (n: string) => void;
  onChangeDescription: (d: string) => void;
  validationResult: { estimated_space_size: number; warnings: string[] } | null;
  submitError: string;
}) {
  const problem = problems.find(p => p.id === problemType);
  const strategyName = problem?.strategies.find(s => s.id === strategyId)?.name ?? strategyId;
  const oracleName = problem?.oracles.find(o => o.id === oracleId)?.name ?? oracleId;

  return (
    <div>
      <h2 style={{ marginBottom: 'var(--space-2)' }}>Review & Launch</h2>
      <p className="text-secondary" style={{ marginBottom: 'var(--space-6)' }}>
        Review your configuration before launching the experiment. Verify the estimated search space and resource limits.
      </p>

      <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: 'var(--space-4)', marginBottom: 'var(--space-6)' }}>
        <div className="form-group">
          <label className="form-label" htmlFor="exp-name">Experiment Name <span style={{ color: 'var(--color-danger)' }}>*</span></label>
          <input
            id="exp-name"
            type="text"
            className="input"
            value={name}
            onChange={e => onChangeName(e.target.value)}
            placeholder="e.g. Coin Change Break on 3-Coin System"
            required
          />
        </div>
        <div className="form-group">
          <label className="form-label" htmlFor="exp-desc">Description (optional)</label>
          <input
            id="exp-desc"
            type="text"
            className="input"
            value={description}
            onChange={e => onChangeDescription(e.target.value)}
            placeholder="Brief notes about this experiment"
          />
        </div>
      </div>

      {/* Configuration Summary */}
      <div className="card" style={{ marginBottom: 'var(--space-4)' }}>
        <div className="card-header">
          <h3>Configuration Summary</h3>
        </div>
        <div className="card-body" style={{ padding: 0 }}>
          <table className="data-table" aria-label="Experiment configuration summary">
            <tbody>
              <tr>
                <td className="text-secondary text-sm" style={{ width: '200px' }}>Problem Family</td>
                <td className="font-semibold">{problem?.name ?? problemType}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Objective</td>
                <td>
                  <span className={`badge ${problem?.objective === 'MINIMIZE' ? 'badge-primary' : 'badge-success'}`}>
                    {problem?.objective}
                  </span>
                </td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Greedy Strategy</td>
                <td className="mono">{strategyName}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Ground-Truth Oracle</td>
                <td className="mono">{oracleName}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Search Mode</td>
                <td>
                  <code style={{ fontFamily: 'var(--font-mono)', fontSize: '13px' }}>{mode}</code>
                  <span className="text-secondary text-sm" style={{ marginLeft: '8px' }}>
                    — {SEARCH_MODE_INFO[mode].description}
                  </span>
                </td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Search Strategy</td>
                <td>
                  <code style={{ fontFamily: 'var(--font-mono)', fontSize: '13px' }}>{strategy}</code>
                  <span className="text-secondary text-sm" style={{ marginLeft: '8px' }}>
                    — {SEARCH_STRATEGY_INFO[strategy].description}
                  </span>
                </td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Random Seed</td>
                <td className="mono">{randomSeed}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Max Candidates</td>
                <td className="mono">{formatNumber(maxCandidates)}</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Timeout</td>
                <td className="mono">{(maxTimeMs / 1000).toFixed(1)}s</td>
              </tr>
              <tr>
                <td className="text-secondary text-sm">Search Space</td>
                <td>
                  <pre className="code-block" style={{ fontSize: '12px', margin: 0 }}>
                    {JSON.stringify(searchSpace, null, 2)}
                  </pre>
                </td>
              </tr>
            </tbody>
          </table>
        </div>
      </div>

      {/* Estimated Computational Cost & Complexity */}
      {validationResult && (
        <div className="card" style={{ marginBottom: 'var(--space-6)' }}>
          <div className="card-header">
            <h3>Estimated Computational Cost & Complexity</h3>
          </div>
          <div className="card-body">
            <div className="metric-grid" style={{ gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))', marginBottom: 'var(--space-4)' }}>
              <MetricCard
                label="Estimated Search Space"
                value={formatNumber(validationResult.estimated_space_size)}
                note="Candidate instances within parameter bounds (estimate)"
              />
              <MetricCard
                label="Evaluation Budget"
                value={`${formatNumber(maxCandidates)} candidates`}
                note={`Max timeout ceiling: ${(maxTimeMs / 1000).toFixed(1)}s`}
              />
              <MetricCard
                label="Search Coverage Scope"
                value={
                  strategy === 'EXHAUSTIVE' && maxCandidates >= validationResult.estimated_space_size
                    ? '100% (Exhaustive)'
                    : validationResult.estimated_space_size > 0
                      ? `${Math.min(100, (maxCandidates / validationResult.estimated_space_size) * 100).toFixed(2)}% (Partial)`
                      : 'Bounded Empirical'
                }
                note={
                  strategy === 'EXHAUSTIVE' && maxCandidates >= validationResult.estimated_space_size
                    ? 'Complete space coverage'
                    : 'Stochastic / bounded sampling'
                }
              />
            </div>
            <p className="text-secondary text-sm" style={{ margin: 0 }}>
              <em>Note:</em> Search space cardinality is an analytical estimate based on configured combinatorial ranges. Exact evaluations will be capped by the candidate budget and timeout ceiling.
            </p>
          </div>
        </div>
      )}

      {validationResult?.warnings?.length ? (
        <div className="alert alert-counterexample" style={{ marginBottom: 'var(--space-4)' }} role="alert">
          <span className="alert-icon" aria-hidden="true">⚠</span>
          <div className="alert-content">
            <div className="alert-title">Configuration Warnings</div>
            <ul style={{ marginTop: 'var(--space-2)', paddingLeft: 'var(--space-5)' }}>
              {validationResult.warnings.map((w, i) => <li key={i} className="text-sm">{w}</li>)}
            </ul>
          </div>
        </div>
      ) : null}

      {submitError && (
        <div className="alert alert-danger" style={{ marginBottom: 'var(--space-4)' }} role="alert">
          <span className="alert-icon" aria-hidden="true">⛔</span>
          <div className="alert-content">
            <div className="alert-title">Error</div>
            <p className="text-sm">{submitError}</p>
          </div>
        </div>
      )}

      <div className="alert alert-info" style={{ marginBottom: 'var(--space-6)' }} role="note">
        <span className="alert-icon" aria-hidden="true">ℹ</span>
        <div className="alert-content text-sm">
          Running the experiment will create an immutable, versioned record.
          Seed <code className="mono">{randomSeed}</code> ensures deterministic reproducibility.
          The experiment cannot be modified after launch.
        </div>
      </div>
    </div>
  );
}

// --- Main Wizard ---------------------------------------------------------
export function NewExperimentPage() {
  const navigate = useNavigate();
  const [step, setStep] = useState<Step>(1);
  const [problems, setProblems] = useState<ProblemMetadata[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState('');

  // Step 1
  const [problemType, setProblemType] = useState<ProblemType>('coin_change');
  const [strategyId, setStrategyId] = useState('largest_denomination_first');
  const [oracleId, setOracleId] = useState('dp_exact');

  // Step 2
  const [searchSpace, setSearchSpace] = useState<Record<string, { min: number; max: number }>>(
    defaultSearchSpace('coin_change') as Record<string, { min: number; max: number }>
  );

  // Step 3
  const [mode, setMode] = useState<SearchMode>('ANY');
  const [strategy, setStrategy] = useState<SearchStrategy>('EXHAUSTIVE');
  const [maxCandidates, setMaxCandidates] = useState(1000);
  const [maxTimeMs, setMaxTimeMs] = useState(5000);
  const [randomSeed, setRandomSeed] = useState(42);

  // Step 4
  const [name, setName] = useState('');
  const [description, setDescription] = useState('');
  const validationResult = useMemo(() => {
    let size = 1;
    for (const range of Object.values(searchSpace)) {
      size *= Math.max(1, (range.max - range.min + 1));
    }
    const warnings: string[] = [];
    if (size > 10_000_000 && strategy === 'EXHAUSTIVE') {
      warnings.push('Search space is large for exhaustive search. Consider RANDOMIZED search or reducing bounds.');
    }
    return { estimated_space_size: Math.min(size, 1_000_000_000), warnings };
  }, [searchSpace, strategy]);
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [submitError, setSubmitError] = useState('');

  useEffect(() => {
    problemsApi.list()
      .then(setProblems)
      .catch(e => setError(getErrorMessage(e)))
      .finally(() => setLoading(false));
  }, []);

  const handleSelectProblem = useCallback((id: ProblemType, defStrategy: string, defOracle: string) => {
    setProblemType(id);
    setStrategyId(defStrategy);
    setOracleId(defOracle);
    setSearchSpace(defaultSearchSpace(id) as Record<string, { min: number; max: number }>);
  }, []);

  const handleSpaceUpdate = useCallback((key: string, field: 'min' | 'max', val: number) => {
    setSearchSpace(prev => ({
      ...prev,
      [key]: { ...prev[key], [field]: val },
    }));
  }, []);

  const handleNext = async () => {
    if (step === 3) {
      // Pre-validate by creating a temp experiment
      // We skip that complex flow — just move to review
      // Validation happens on the backend during actual create+validate
      setStep(4);
      return;
    }
    if (step < 4) setStep((step + 1) as Step);
  };

  const handleBack = () => {
    if (step > 1) setStep((step - 1) as Step);
  };

  const handleSubmit = async () => {
    if (!name.trim()) {
      setSubmitError('Please enter an experiment name.');
      return;
    }
    setIsSubmitting(true);
    setSubmitError('');
    try {
      const req: CreateExperimentRequest = {
        name: name.trim(),
        description: description.trim() || undefined,
        problem_type: problemType,
        strategy_id: strategyId,
        oracle_id: oracleId,
        mode,
        strategy,
        random_seed: randomSeed,
        max_candidates: maxCandidates,
        max_time_ms: maxTimeMs,
        search_space: searchSpace,
      };
      const created = await experimentsApi.create(req);
      // Validate, then run
      await experimentsApi.validate(created.id);
      await experimentsApi.run(created.id);
      navigate(`/experiments/${created.id}`);
    } catch (e) {
      setSubmitError(getErrorMessage(e));
      setIsSubmitting(false);
    }
  };

  if (loading) return <LoadingPage message="Loading problem catalogue…" />;
  if (error) return <ErrorState message={error} />;

  const STEP_LABELS: [string, string, string, string] = [
    'Problem & Strategy',
    'Search Space',
    'Search Mode',
    'Review & Launch',
  ];

  const canProceedStep1 = !!problemType;
  const canProceedStep2 = true; // basic field validation happens inline
  const canProceedStep3 = true;

  return (
    <main id="main-content" className="page-content">
      <div className="container">
        <Breadcrumb items={[{ label: 'Experiments', to: '/experiments' }, { label: 'New Experiment' }]} />
        <h1 style={{ marginBottom: 'var(--space-8)' }}>Create Experiment</h1>

        {/* Step indicators */}
        <div className="wizard-steps" role="list" aria-label="Experiment creation steps">
          {STEP_LABELS.map((label, i) => (
            <div
              key={label}
              className={`wizard-step${step === i + 1 ? ' current' : ''}${step > i + 1 ? ' done' : ''}`}
              role="listitem"
              aria-current={step === i + 1 ? 'step' : undefined}
            >
              <span className="wizard-step-number" aria-hidden="true">
                {step > i + 1 ? '✓' : i + 1}
              </span>
              <span className="wizard-step-label">{label}</span>
              {i < 3 && <span style={{ flex: 1, height: 1, background: 'var(--color-border-subtle)' }} aria-hidden="true" />}
            </div>
          ))}
        </div>

        {/* Step content */}
        <div className="card" style={{ marginBottom: 'var(--space-6)' }}>
          <div className="card-body">
            {step === 1 && (
              <Step1Problem
                problems={problems}
                selectedProblemId={problemType}
                selectedStrategyId={strategyId}
                selectedOracleId={oracleId}
                onSelectProblem={handleSelectProblem}
                onSelectStrategy={setStrategyId}
                onSelectOracle={setOracleId}
              />
            )}
            {step === 2 && (
              <Step2SearchSpace
                problemType={problemType}
                searchSpace={searchSpace}
                onUpdate={handleSpaceUpdate}
              />
            )}
            {step === 3 && (
              <Step3SearchMode
                mode={mode}
                strategy={strategy}
                maxCandidates={maxCandidates}
                maxTimeMs={maxTimeMs}
                randomSeed={randomSeed}
                onChangeMode={setMode}
                onChangeStrategy={setStrategy}
                onChangeCandidates={setMaxCandidates}
                onChangeTime={setMaxTimeMs}
                onChangeSeed={setRandomSeed}
              />
            )}
            {step === 4 && (
              <Step4Review
                name={name}
                description={description}
                problemType={problemType}
                problems={problems}
                strategyId={strategyId}
                oracleId={oracleId}
                mode={mode}
                strategy={strategy}
                searchSpace={searchSpace}
                maxCandidates={maxCandidates}
                maxTimeMs={maxTimeMs}
                randomSeed={randomSeed}
                onChangeName={setName}
                onChangeDescription={setDescription}
                validationResult={validationResult}
                submitError={submitError}
              />
            )}
          </div>

          <div className="card-footer">
            <div className="flex justify-between items-center">
              <div>
                {step > 1 && (
                  <button className="btn btn-secondary" onClick={handleBack} disabled={isSubmitting}>
                    ← Back
                  </button>
                )}
              </div>
              <div className="flex gap-3">
                {step < 4 ? (
                  <button
                    className="btn btn-primary"
                    onClick={handleNext}
                    disabled={
                      (step === 1 && !canProceedStep1) ||
                      (step === 2 && !canProceedStep2) ||
                      (step === 3 && !canProceedStep3)
                    }
                  >
                    Next →
                  </button>
                ) : (
                  <button
                    id="submit-experiment"
                    className="btn btn-primary"
                    onClick={handleSubmit}
                    disabled={isSubmitting || !name.trim()}
                  >
                    {isSubmitting ? (
                      <>
                        <span className="spinner" aria-hidden="true" />
                        Launching…
                      </>
                    ) : (
                      '▶ Run Experiment'
                    )}
                  </button>
                )}
              </div>
            </div>
          </div>
        </div>
      </div>
    </main>
  );
}
