export type ProblemObjective = 'MINIMIZATION' | 'MAXIMIZATION';

export type SearchStrategy = 'EXHAUSTIVE' | 'BACKTRACKING' | 'RANDOMIZED' | 'HEURISTIC';

export type SearchMode = 'ANY' | 'SMALLEST' | 'WORST';

export type ProblemType = 'coin_change' | 'activity_selection' | 'knapsack_01' | 'knapsack_fractional';

export interface SolverComplexity {
  algorithm_name: string;
  time_best: string;
  time_average: string;
  time_worst: string;
  space_worst: string;
  explanation: string;
}

export interface ProblemMetadata {
  id: ProblemType;
  name: string;
  objective: ProblemObjective;
  description: string;
  greedy_strategy: string;
  oracle_strategy: string;
  can_fail: boolean;
  default_config: Record<string, any>;
  solvers: SolverComplexity[];
}

export interface TraceStep {
  step_number: number;
  state_description: string;
  remaining_target: number;
  available_choices: number[];
  selected_choice: number | null;
  outcome_description: string;
  remaining_after: number;
  accumulated_count: number;
  rationale: string;
}

export interface AlgorithmTrace {
  algorithm_name: string;
  strategy_name: string;
  success: boolean;
  steps: TraceStep[];
  final_solution: any[];
  final_cost: number | null;
  execution_notes: string[];
}

export interface ComparisonMetrics {
  is_counterexample: boolean;
  objective: ProblemObjective;
  greedy_solved: boolean;
  optimal_solved: boolean;
  greedy_cost: number | null;
  optimal_cost: number | null;
  absolute_gap: number | null;
  ratio: number | null;
  overhead_percentage: number | null;
  interpretation: string;
}

export interface DivergenceResult {
  has_diverged: boolean;
  divergence_step: number | null;
  state_remaining: number;
  greedy_choice: number | null;
  optimal_choices: number[];
  greedy_projected_cost: number | null;
  optimal_projected_cost: number | null;
  explanation: string;
}

export interface MinimalityResult {
  verified: boolean;
  description: string;
}

export interface SearchStatistics {
  candidates_generated: number;
  candidates_evaluated: number;
  counterexamples_found: number;
  nodes_explored: number;
  branches_pruned: number;
  iterations: number;
  duration_ms: number;
  search_completed: boolean;
  search_limit_reached: boolean;
  termination_reason: string;
}

export interface InvestigationRequest {
  problem: ProblemType;
  strategy: SearchStrategy;
  mode: SearchMode;

  // Coin Change bounds
  min_denomination?: number;
  max_denomination?: number;
  min_coin_count?: number;
  max_coin_count?: number;
  min_target?: number;
  max_target?: number;
  require_unit_coin?: boolean;

  // Activity Selection bounds
  min_activities?: number;
  max_activities?: number;
  min_time?: number;
  max_time?: number;
  min_duration?: number;

  // Knapsack bounds
  min_items?: number;
  max_items?: number;
  min_weight?: number;
  max_weight?: number;
  min_value?: number;
  max_value?: number;
  min_capacity?: number;
  max_capacity?: number;

  // Safety & PRNG
  random_seed?: number;
  max_iterations?: number;
  max_candidates?: number;
}

export interface InvestigationResponse {
  status: 'success' | 'error';
  problem: string;
  message: string;
  counterexample_found: boolean;
  search_config: Record<string, any>;
  search_statistics: SearchStatistics;
  minimality: MinimalityResult;
  candidate_summary?: string;
  candidate: any | null;
  greedy_solution: any | null;
  optimal_solution: any | null;
  metrics: ComparisonMetrics | null;
  divergence: DivergenceResult | null;
  error: { code: string; message: string; details?: any } | null;
}

export interface VerificationRequest {
  problem: ProblemType;
  candidate: Record<string, any>;
}

export interface VerificationResponse {
  status: 'success' | 'error';
  problem: string;
  counterexample_found: boolean;
  candidate: any;
  greedy_solution: any;
  optimal_solution: any;
  metrics: ComparisonMetrics;
  divergence: DivergenceResult;
  error?: { code: string; message: string; details?: any };
}

export interface HealthResponse {
  status: string;
  app_name: string;
  version: string;
  engine: {
    status: string;
    path: string;
    exists: boolean;
  };
}

export class ApiError extends Error {
  code: string;
  statusCode: number;
  details?: any;

  constructor(message: string, code: string = 'API_ERROR', statusCode: number = 500, details?: any) {
    super(message);
    this.name = 'ApiError';
    this.code = code;
    this.statusCode = statusCode;
    this.details = details;
  }
}
