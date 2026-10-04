import { describe, it, expect, vi, beforeEach, afterEach } from 'vitest';
import { apiClient } from '../api/client';
import { ApiError } from '../types/api';

describe('API Client Layer', () => {
  beforeEach(() => {
    vi.stubGlobal('fetch', vi.fn());
  });

  afterEach(() => {
    vi.restoreAllMocks();
  });

  it('successfully returns deserialized JSON data on 200 OK', async () => {
    const mockData = { status: 'ok', version: '1.0.0' };
    (fetch as any).mockResolvedValueOnce({
      ok: true,
      status: 200,
      headers: {
        get: (h: string) => (h === 'content-type' ? 'application/json' : null),
      },
      json: async () => mockData,
    });

    const result = await apiClient<{ status: string; version: string }>('/api/v1/health');
    expect(result).toEqual(mockData);
    expect(fetch).toHaveBeenCalledTimes(1);
  });

  it('throws ApiError with structured error code and message on 400 / 422 error', async () => {
    const errorBody = {
      error: {
        code: 'VALIDATION_ERROR',
        message: 'Invalid bounds specified for search space.',
        details: { field: 'max_target' },
      },
    };

    (fetch as any).mockResolvedValue({
      ok: false,
      status: 422,
      headers: {
        get: (h: string) => (h === 'content-type' ? 'application/json' : null),
      },
      json: async () => errorBody,
    });

    try {
      await apiClient('/api/v1/investigate', { method: 'POST' });
      expect.fail('Expected apiClient to throw');
    } catch (err: any) {
      expect(err).toBeInstanceOf(ApiError);
      expect(err.message).toBe('Invalid bounds specified for search space.');
      expect(err.code).toBe('VALIDATION_ERROR');
      expect(err.statusCode).toBe(422);
      expect(err.details).toEqual({ field: 'max_target' });
    }
  });

  it('handles network disconnection / server unreachable gracefully', async () => {
    (fetch as any).mockRejectedValueOnce(new Error('Failed to fetch'));

    await expect(apiClient('/api/v1/health')).rejects.toThrow(ApiError);

    try {
      await apiClient('/api/v1/health');
    } catch (err: any) {
      expect(err).toBeInstanceOf(ApiError);
      expect(err.code).toBe('NETWORK_ERROR');
      expect(err.statusCode).toBe(0);
    }
  });

  it('handles timeout using AbortController', async () => {
    const abortErr = new Error('The user aborted a request.');
    abortErr.name = 'AbortError';
    (fetch as any).mockRejectedValueOnce(abortErr);

    try {
      await apiClient('/api/v1/investigate', { timeoutMs: 100 });
      expect.fail('Should have thrown timeout error');
    } catch (err: any) {
      expect(err).toBeInstanceOf(ApiError);
      expect(err.code).toBe('TIMEOUT_ERROR');
      expect(err.statusCode).toBe(504);
    }
  });
});
