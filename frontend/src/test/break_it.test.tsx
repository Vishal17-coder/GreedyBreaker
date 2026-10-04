import { describe, it, expect, vi, beforeEach, afterEach } from 'vitest';
import { render, screen, waitFor, fireEvent } from '@testing-library/react';
import { BreakItPage } from '../pages/BreakItPage';
import * as problemsApi from '../api/problems';
import * as investigateApi from '../api/investigate';
import type { InvestigationResponse, ProblemMetadata } from '../types/api';

const mockProblems: ProblemMetadata[] = [
  {
    id: 'coin_change',
    name: 'Coin Change Problem',
    objective: 'MINIMIZATION',
    description: 'Find minimum coins needed to make target amount.',
    greedy_strategy: 'Largest denomination first',
    oracle_strategy: 'Exact Dynamic Programming',
    can_fail: true,
    default_config: {},
    solvers: [
      {
        algorithm_name: 'Greedy Change-Maker',
        time_best: 'O(k)',
        time_average: 'O(k)',
        time_worst: 'O(k)',
        space_worst: 'O(1)',
        explanation: 'Iteratively selects largest denomination.',
      },
    ],
  },
  {
    id: 'activity_selection',
    name: 'Activity Selection Problem',
    objective: 'MAXIMIZATION',
    description: 'Select maximum mutually compatible activities.',
    greedy_strategy: 'Earliest finish time (EFT)',
    oracle_strategy: 'Exact interval scheduling DP',
    can_fail: false,
    default_config: {},
    solvers: [],
  },
];

describe('BreakItPage Component', () => {
  beforeEach(() => {
    vi.spyOn(problemsApi, 'getProblems').mockResolvedValue(mockProblems);
  });

  afterEach(() => {
    vi.restoreAllMocks();
  });

  it('renders problem selector and loads problem metadata', async () => {
    render(<BreakItPage />);

    expect(screen.getByRole('heading', { level: 1, name: /Break It/i })).toBeInTheDocument();

    await waitFor(() => {
      expect(screen.getByText(/Coin Change Problem \(MINIMIZATION\)/i)).toBeInTheDocument();
    });

    expect(screen.getByText(/Largest denomination first/i)).toBeInTheDocument();
    expect(screen.getByText(/Exact Dynamic Programming/i)).toBeInTheDocument();
  });

  it('triggers investigation and renders counterexample result with forensic details', async () => {
    const mockInvestigationSuccess: InvestigationResponse = {
      status: 'success',
      problem: 'coin_change',
      message: 'Counterexample found: greedy solution is suboptimal.',
      counterexample_found: true,
      candidate_summary: 'Greedy solution uses 3 coins, but optimal requires only 2 coins.',
      search_config: { strategy: 'BACKTRACKING', mode: 'SMALLEST' },
      search_statistics: {
        candidates_generated: 45,
        candidates_evaluated: 45,
        counterexamples_found: 1,
        nodes_explored: 12,
        branches_pruned: 4,
        iterations: 45,
        duration_ms: 18.5,
        search_completed: true,
        search_limit_reached: false,
        termination_reason: 'FOUND_COUNTEREXAMPLE',
      },
      minimality: {
        verified: true,
        description: 'Verified as minimal counterexample.',
      },
      candidate: {
        denominations: [1, 3, 4],
        target: 6,
      },
      greedy_solution: {
        cost: 3,
        solved: true,
        coins_used: [4, 1, 1],
        trace: {
          algorithm_name: 'Greedy Change-Maker',
          strategy_name: 'Largest-first',
          success: true,
          steps: [
            {
              step_number: 1,
              state_description: 'Remaining target: 6',
              remaining_target: 6,
              available_choices: [4, 3, 1],
              selected_choice: 4,
              outcome_description: 'Chose coin 4',
              remaining_after: 2,
              accumulated_count: 1,
              rationale: 'Largest denomination <= 6 is 4',
            },
          ],
          final_solution: [4, 1, 1],
          final_cost: 3,
          execution_notes: [],
        },
      },
      optimal_solution: {
        cost: 2,
        solved: true,
        coins_used: [3, 3],
        trace: {
          algorithm_name: 'Exact DP Change-Maker',
          strategy_name: 'Dynamic Programming',
          success: true,
          steps: [
            {
              step_number: 1,
              state_description: 'Remaining target: 6',
              remaining_target: 6,
              available_choices: [4, 3, 1],
              selected_choice: 3,
              outcome_description: 'Optimal DP branch chooses 3',
              remaining_after: 3,
              accumulated_count: 1,
              rationale: 'DP table lookup indicates coin 3 yields minimal cost',
            },
          ],
          final_solution: [3, 3],
          final_cost: 2,
          execution_notes: [],
        },
      },
      metrics: {
        is_counterexample: true,
        objective: 'MINIMIZATION',
        greedy_solved: true,
        optimal_solved: true,
        greedy_cost: 3,
        optimal_cost: 2,
        absolute_gap: 1,
        ratio: 1.5,
        overhead_percentage: 50.0,
        interpretation: 'Greedy solution uses 1 extra coin (50.0% overhead).',
      },
      divergence: {
        has_diverged: true,
        divergence_step: 1,
        state_remaining: 6,
        greedy_choice: 4,
        optimal_choices: [3],
        greedy_projected_cost: 3,
        optimal_projected_cost: 2,
        explanation: 'At remaining target 6, greedy picked 4 instead of optimal 3.',
      },
      error: null,
    };

    vi.spyOn(investigateApi, 'runInvestigation').mockResolvedValue(mockInvestigationSuccess);

    render(<BreakItPage />);

    // Wait until problem metadata is loaded and button becomes enabled
    const breakButton = await screen.findByRole('button', { name: /BREAK IT/i });
    await waitFor(() => {
      expect(breakButton).toBeEnabled();
    });

    fireEvent.click(breakButton);

    await waitFor(() => {
      expect(screen.getByText(/COUNTEREXAMPLE DISCOVERED!/i)).toBeInTheDocument();
    });

    // Verify metrics and candidate are rendered
    expect(screen.getByText(/Forensic Candidate Instance/i)).toBeInTheDocument();
    expect(screen.getByText(/50.0%/i)).toBeInTheDocument();
    expect(screen.getByText(/First-Divergence Point/i)).toBeInTheDocument();
    expect(screen.getByText(/At remaining target 6, greedy picked 4 instead of optimal 3./i)).toBeInTheDocument();
    expect(screen.getByText(/Greedy Solver Execution Trace/i)).toBeInTheDocument();
  });

  it('renders scientific bounded-search notice when no counterexample is found', async () => {
    const mockInvestigationNoCounterexample: InvestigationResponse = {
      status: 'success',
      problem: 'activity_selection',
      message: 'No counterexample was found within the configured search space.',
      counterexample_found: false,
      search_config: { strategy: 'EXHAUSTIVE', mode: 'ANY' },
      search_statistics: {
        candidates_generated: 120,
        candidates_evaluated: 120,
        counterexamples_found: 0,
        nodes_explored: 120,
        branches_pruned: 0,
        iterations: 120,
        duration_ms: 12.3,
        search_completed: true,
        search_limit_reached: false,
        termination_reason: 'SPACE_EXHAUSTED',
      },
      minimality: { verified: false, description: 'No counterexample found.' },
      candidate: null,
      greedy_solution: null,
      optimal_solution: null,
      metrics: null,
      divergence: null,
      error: null,
    };

    vi.spyOn(investigateApi, 'runInvestigation').mockResolvedValue(mockInvestigationNoCounterexample);

    render(<BreakItPage />);

    // Wait until problem metadata is loaded and button becomes enabled
    const breakButton = await screen.findByRole('button', { name: /BREAK IT/i });
    await waitFor(() => {
      expect(breakButton).toBeEnabled();
    });

    fireEvent.click(breakButton);

    await waitFor(() => {
      expect(
        screen.getAllByText(/No counterexample was found within the configured search space./i).length
      ).toBeGreaterThan(0);
    });

    // Check the scientific notice requirement: must not claim universal mathematical proof
    expect(
      screen.getByText(/finite search exhaustion does not constitute mathematical proof of global greedy correctness/i)
    ).toBeInTheDocument();

    expect(screen.getByText(/SPACE_EXHAUSTED/i)).toBeInTheDocument();
  });
});
