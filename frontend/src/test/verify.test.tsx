import { describe, it, expect, vi, beforeEach, afterEach } from 'vitest';
import { render, screen, waitFor, fireEvent } from '@testing-library/react';
import { VerifyPage } from '../pages/VerifyPage';
import * as problemsApi from '../api/problems';
import * as verifyApi from '../api/verify';
import type { VerificationResponse, ProblemMetadata } from '../types/api';

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
    solvers: [],
  },
];

describe('VerifyPage Component', () => {
  beforeEach(() => {
    vi.spyOn(problemsApi, 'getProblems').mockResolvedValue(mockProblems);
  });

  afterEach(() => {
    vi.restoreAllMocks();
  });

  it('renders verify form and presets', async () => {
    render(<VerifyPage />);

    expect(screen.getByRole('heading', { level: 1, name: /Verify a Candidate/i })).toBeInTheDocument();
    expect(
      screen.getByText(/Coin Change: {1,3,4}, Target 6 \(Counterexample\)/i)
    ).toBeInTheDocument();
  });

  it('verifies candidate instance {1, 3, 4} target=6 and displays counterexample results', async () => {
    const mockVerifyResponse: VerificationResponse = {
      status: 'success',
      problem: 'coin_change',
      counterexample_found: true,
      candidate: { denominations: [1, 3, 4], target: 6 },
      greedy_solution: {
        cost: 3,
        solved: true,
        trace: {
          algorithm_name: 'Greedy Change-Maker',
          strategy_name: 'Largest-first',
          success: true,
          steps: [
            {
              step_number: 1,
              state_description: 'Remaining: 6',
              remaining_target: 6,
              available_choices: [4, 3, 1],
              selected_choice: 4,
              outcome_description: 'Chose coin 4',
              remaining_after: 2,
              accumulated_count: 1,
              rationale: 'Greedy picks 4',
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
        trace: {
          algorithm_name: 'Exact DP',
          strategy_name: 'DP Table',
          success: true,
          steps: [
            {
              step_number: 1,
              state_description: 'Remaining: 6',
              remaining_target: 6,
              available_choices: [4, 3, 1],
              selected_choice: 3,
              outcome_description: 'Optimal picks 3',
              remaining_after: 3,
              accumulated_count: 1,
              rationale: 'Optimal picks 3',
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
    };

    vi.spyOn(verifyApi, 'verifyCandidate').mockResolvedValue(mockVerifyResponse);

    render(<VerifyPage />);

    const runButton = screen.getByRole('button', { name: /RUN DUAL VERIFICATION/i });
    fireEvent.click(runButton);

    await waitFor(() => {
      expect(
        screen.getByText(/VALIDATED COUNTEREXAMPLE — GREEDY FAILED/i)
      ).toBeInTheDocument();
    });

    expect(screen.getByText(/Greedy solution uses 1 extra coin/i)).toBeInTheDocument();
    expect(screen.getByText(/First-Divergence Point/i)).toBeInTheDocument();
    expect(screen.getByText(/Step 1/i)).toBeInTheDocument();
  });
});
