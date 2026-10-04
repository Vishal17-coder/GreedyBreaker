import { describe, it, expect, vi, beforeEach, afterEach } from 'vitest';
import { getHealth, getProblems } from '../api/problems';
import type { ProblemMetadata } from '../types/api';

describe('Problems & Health API functions', () => {
  beforeEach(() => {
    vi.stubGlobal('fetch', vi.fn());
  });

  afterEach(() => {
    vi.restoreAllMocks();
  });

  it('fetches health check with engine status', async () => {
    const mockHealth = {
      status: 'ok',
      app_name: 'GreedyBreaker API',
      version: '1.0.0',
      engine: {
        status: 'ready',
        path: 'engine/build/greedybreaker_cli.exe',
        exists: true,
      },
    };

    (fetch as any).mockResolvedValueOnce({
      ok: true,
      headers: { get: () => 'application/json' },
      json: async () => mockHealth,
    });

    const res = await getHealth();
    expect(res.status).toBe('ok');
    expect(res.engine.exists).toBe(true);
  });

  it('fetches the 4 problem families metadata', async () => {
    const mockProblems: ProblemMetadata[] = [
      {
        id: 'coin_change',
        name: 'Coin Change Problem',
        objective: 'MINIMIZATION',
        description: 'Standard change-making problem.',
        greedy_strategy: 'Largest denomination first',
        oracle_strategy: 'Exact Dynamic Programming',
        can_fail: true,
        default_config: {},
        solvers: [],
      },
      {
        id: 'activity_selection',
        name: 'Activity Selection Problem',
        objective: 'MAXIMIZATION',
        description: 'Interval scheduling.',
        greedy_strategy: 'Earliest finish time',
        oracle_strategy: 'Exact DP',
        can_fail: false,
        default_config: {},
        solvers: [],
      },
      {
        id: 'knapsack_01',
        name: '0/1 Knapsack Problem',
        objective: 'MAXIMIZATION',
        description: 'Binary knapsack.',
        greedy_strategy: 'Value-to-weight density',
        oracle_strategy: 'Exact 2D DP',
        can_fail: true,
        default_config: {},
        solvers: [],
      },
      {
        id: 'knapsack_fractional',
        name: 'Fractional Knapsack Problem',
        objective: 'MAXIMIZATION',
        description: 'Continuous knapsack.',
        greedy_strategy: 'Value-to-weight density',
        oracle_strategy: 'Exact continuous greedy',
        can_fail: false,
        default_config: {},
        solvers: [],
      },
    ];

    (fetch as any).mockResolvedValueOnce({
      ok: true,
      headers: { get: () => 'application/json' },
      json: async () => mockProblems,
    });

    const res = await getProblems();
    expect(res).toHaveLength(4);
    expect(res.map((p) => p.id)).toEqual([
      'coin_change',
      'activity_selection',
      'knapsack_01',
      'knapsack_fractional',
    ]);
  });
});
