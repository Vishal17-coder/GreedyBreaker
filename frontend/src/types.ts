// =============================================================================
// Greedy Breaker V2 — Domain Types
// Strictly derived from backend DTOs and API contract.
// =============================================================================

// --- Enums (mirror Java enums exactly) ------------------------------------

export type ProblemType =
  | 'coin_change'
  | 'knapsack_01'
  | 'activity_selection'
  | 'fractional_knapsack'
  | 'huffman_coding';

export type ExperimentStatus =
  | 'CREATED'
  | 'VALIDATING'
  | 'VALID'
  | 'INVALID'
  | 'QUEUED'
  | 'RUNNING'
  | 'COMPLETED'
  | 'NO_COUNTEREXAMPLE'
  | 'SEARCH_INCOMPLETE'
  | 'TIMED_OUT'
  | 'FAILED'
  | 'CANCELLED';

export type SearchMode = 'ANY' | 'SMALLEST' | 'WORST' | 'PROFILE';

export type SearchStrategy = 'EXHAUSTIVE' | 'RANDOMIZED' | 'GUIDED' | 'HYBRID';

// --- Problem Registry -----------------------------------------------------

export interface StrategyInfo {
  id: string;
  name: string;
  default: boolean;
}

export interface OracleInfo {
  id: string;
  name: string;
  confidence: string;
  default: boolean;
}

export interface ProblemMetadata {
  id: string;
  name: string;
  description: string;
  objective: 'MINIMIZE' | 'MAXIMIZE';
  strategies: StrategyInfo[];
  oracles: OracleInfo[];
}

// --- Search Space (problem-specific) --------------------------------------
// The search_space is an opaque JSON object at the API level.
// We define specific typed shapes for each problem for the wizard UI.

export interface CoinChangeSearchSpace {
  denomination_count: { min: number; max: number };
  denomination_range: { min: number; max: number };
  target_range: { min: number; max: number };
}

export interface KnapsackSearchSpace {
  item_count: { min: number; max: number };
  weight_range: { min: number; max: number };
  value_range: { min: number; max: number };
  capacity_range: { min: number; max: number };
}

export interface ActivitySelectionSearchSpace {
  interval_count: { min: number; max: number };
  coordinate_range: { min: number; max: number };
}

export interface FractionalKnapsackSearchSpace {
  item_count: { min: number; max: number };
  weight_range: { min: number; max: number };
  value_range: { min: number; max: number };
  capacity_range: { min: number; max: number };
}

export interface HuffmanSearchSpace {
  symbol_count: { min: number; max: number };
  frequency_range: { min: number; max: number };
}

export type SearchSpace =
  | CoinChangeSearchSpace
  | KnapsackSearchSpace
  | ActivitySelectionSearchSpace
  | FractionalKnapsackSearchSpace
  | HuffmanSearchSpace
  | Record<string, unknown>;

// --- Create Experiment Request -------------------------------------------

export interface CreateExperimentRequest {
  name: string;
  description?: string;
  problem_type: ProblemType;
  strategy_id: string;
  oracle_id: string;
  mode: SearchMode;
  strategy: SearchStrategy;
  random_seed: number;
  max_candidates: number;
  max_time_ms: number;
  search_space: SearchSpace;
}

// --- Experiment Summary (list view) ---------------------------------------

export interface ExperimentSummary {
  id: string;
  name: string;
  problem_type: ProblemType;
  status: ExperimentStatus;
  has_counterexample: boolean;
  instances_evaluated: number;
  created_at: string;
}

// --- Experiment Detail (full view) ----------------------------------------

export interface ExperimentConfiguration {
  strategy_id: string;
  oracle_id: string;
  mode: SearchMode;
  strategy: SearchStrategy;
  random_seed: number;
  max_candidates: number;
  max_time_ms: number;
  search_space: SearchSpace;
}

export interface ExperimentSummaryStats {
  has_counterexample: boolean;
  counterexamples_count: number;
  instances_evaluated: number;
  worst_gap: number;
}

export interface ExperimentDetail {
  id: string;
  name: string;
  description?: string;
  problem_type: ProblemType;
  status: ExperimentStatus;
  parent_experiment_id?: string;
  configuration: ExperimentConfiguration;
  summary: ExperimentSummaryStats;
  created_at: string;
  updated_at: string;
}

// --- Execution Status (polling) -------------------------------------------

export interface ExecutionStatus {
  experiment_id: string;
  status: ExperimentStatus;
  instances_evaluated: number;
  counterexamples_found: number;
  elapsed_time_ms: number;
  message: string;
}

// --- Validation Result ----------------------------------------------------

export interface ValidationErrorItem {
  field: string;
  rejected_value: string;
  message: string;
}

export interface ValidationResult {
  experiment_id: string;
  is_valid: boolean;
  status: ExperimentStatus;
  estimated_space_size: number;
  warnings: string[];
  validation_errors: ValidationErrorItem[];
}

// --- Engine Output structures (from EngineOutputDTO) ----------------------

export interface EngineGreedyResult {
  feasible: boolean;
  objective_value: number;
  items_selected: number[];
  solution_description: string;
}

export interface EngineOracleResult {
  confidence: string;
  optimal_value: number;
  items_selected: number[];
  oracle_method: string;
  solution_description: string;
}

export interface EngineVerificationResult {
  status: string;
  is_counterexample: boolean;
  greedy_value: number;
  optimal_value: number;
  absolute_gap: number;
  relative_gap: number;
  failure_percent: number;
}

export interface EngineTraceStep {
  step_index: number;
  action_description: string;
  heuristic_rule: string;
  state_before: string;
  state_after: string;
  cumulative_objective: number;
}

export interface EngineTrace {
  total_steps: number;
  steps: EngineTraceStep[];
}

export interface EngineSearchResult {
  experiment_id: string;
  problem_type: string;
  strategy_id: string;
  oracle_id: string;
  mode: string;
  strategy: string;
  has_counterexample: boolean;
  instances_evaluated: number;
  counterexamples_found: number;
  failure_rate: number;
  worst_gap: number;
  average_gap: number;
  coverage: string;
  coverage_ratio: number;
  elapsed_time_ms: number;
  estimated_space_size: number;
  status_code: number;
  status_message: string;
  best_instance: Record<string, unknown> | null;
  best_greedy: EngineGreedyResult | null;
  best_oracle: EngineOracleResult | null;
  best_verification: EngineVerificationResult | null;
  reproducibility_token: Record<string, unknown> | null;
}

// --- Execution Result (from /result endpoint) -----------------------------

export interface ExecutionResult {
  experiment_id: string;
  status: ExperimentStatus;
  search_result: EngineSearchResult | null;
  reproducibility_token: Record<string, unknown> | null;
}

// --- Counterexample -------------------------------------------------------

export interface CounterexampleDTO {
  experiment_id: string;
  has_counterexample: boolean;
  original_instance: Record<string, unknown> | null;
  minimal_instance: Record<string, unknown> | null;
  reduction_steps: number;
  reduction_percent: number;
  minimality_status: string;
  ordering_criteria: string;
  final_verification: EngineVerificationResult | null;
}

// --- Forensic Report ------------------------------------------------------

export interface ForensicReport {
  experiment_id: string;
  problem_type: string;
  divergence_step_index: number;
  critical_greedy_action: string;
  optimal_alternative_action: string;
  causal_consequence: string;
  evidence_confidence: string;
  minimality_status: string;
  greedy_trace: EngineTrace | null;
}

// --- Reproduction Result --------------------------------------------------

export interface ReproductionResult {
  reproduction_experiment_id: string;
  original_experiment_id: string;
  status: ExperimentStatus;
  is_deterministic_match: boolean;
  match_summary: string;
  created_at: string;
}

// --- Paginated Response ---------------------------------------------------

export interface PagedResponse<T> {
  content: T[];
  page?: number;
  number?: number;  // Spring Data uses "number" for page index
  size: number;
  total_elements?: number;
  totalElements?: number;
  total_pages?: number;
  totalPages?: number;
}

// --- API Error ------------------------------------------------------------

export interface ApiError {
  title?: string;
  status: number;
  detail: string;
  timestamp?: string;
}

// --- UI Helpers -----------------------------------------------------------

/** Whether an experiment status is in a terminal (final) state */
export function isTerminalStatus(status: ExperimentStatus): boolean {
  return [
    'INVALID', 'COMPLETED', 'NO_COUNTEREXAMPLE',
    'SEARCH_INCOMPLETE', 'TIMED_OUT', 'FAILED', 'CANCELLED',
  ].includes(status);
}

/** Whether an experiment has a discovered counterexample */
export function hasCounterexample(status: ExperimentStatus): boolean {
  return status === 'COMPLETED';
}

/** Whether an experiment is actively running */
export function isRunning(status: ExperimentStatus): boolean {
  return ['CREATED', 'VALIDATING', 'VALID', 'QUEUED', 'RUNNING'].includes(status);
}

export const PROBLEM_LABELS: Record<ProblemType, string> = {
  coin_change: 'Coin Change',
  knapsack_01: '0/1 Knapsack',
  activity_selection: 'Activity Selection',
  fractional_knapsack: 'Fractional Knapsack',
  huffman_coding: 'Huffman Coding',
};

export const STATUS_LABELS: Record<ExperimentStatus, string> = {
  CREATED: 'Created',
  VALIDATING: 'Validating',
  VALID: 'Valid',
  INVALID: 'Invalid',
  QUEUED: 'Queued',
  RUNNING: 'Running',
  COMPLETED: 'Counterexample Found',
  NO_COUNTEREXAMPLE: 'No Counterexample',
  SEARCH_INCOMPLETE: 'Search Incomplete',
  TIMED_OUT: 'Timed Out',
  FAILED: 'Failed',
  CANCELLED: 'Cancelled',
};

export const SEARCH_MODE_INFO: Record<SearchMode, { label: string; description: string }> = {
  ANY: {
    label: 'ANY',
    description: 'Find the first verified counterexample discovered. Stops as soon as one is found.',
  },
  SMALLEST: {
    label: 'SMALLEST',
    description: 'Find the smallest verified counterexample under the configured ordering criteria.',
  },
  WORST: {
    label: 'WORST',
    description: 'Search for the largest observed objective gap within the configured search budget.',
  },
  PROFILE: {
    label: 'PROFILE',
    description: 'Measure empirical failure behavior across the configured search space. Does not claim correctness.',
  },
};

export const SEARCH_STRATEGY_INFO: Record<SearchStrategy, { label: string; description: string }> = {
  EXHAUSTIVE: {
    label: 'EXHAUSTIVE',
    description: 'Enumerate all candidate instances within the configured bounds. Coverage is complete for the bounded space.',
  },
  RANDOMIZED: {
    label: 'RANDOMIZED',
    description: 'Sample candidate instances uniformly at random. Coverage is partial; seed controls reproducibility.',
  },
  GUIDED: {
    label: 'GUIDED',
    description: 'Use heuristic guidance to focus candidate generation on promising regions of the search space.',
  },
  HYBRID: {
    label: 'HYBRID',
    description: 'Combine exhaustive enumeration with randomized sampling. Balances coverage and efficiency.',
  },
};
