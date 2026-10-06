// =============================================================================
// Greedy Breaker V2 — API Client
// All communication with the Spring Boot backend.
// Base URL: /api/v1 (proxied to http://localhost:8080 by Vite)
// =============================================================================

import type {
  CreateExperimentRequest,
  ExperimentSummary,
  ExperimentDetail,
  ExecutionStatus,
  ExecutionResult,
  ValidationResult,
  CounterexampleDTO,
  ForensicReport,
  ReproductionResult,
  ProblemMetadata,
  PagedResponse,
  ApiError,
  ExperimentStatus,
  ProblemType,
} from './types';

const BASE_URL = '/api/v1';

// --- HTTP helpers ---------------------------------------------------------

async function handleResponse<T>(res: Response): Promise<T> {
  if (!res.ok) {
    let error: ApiError;
    try {
      error = await res.json();
    } catch {
      error = { status: res.status, detail: res.statusText };
    }
    throw error;
  }
  // 201 / 204 may have no body
  if (res.status === 204) return undefined as unknown as T;
  return res.json() as Promise<T>;
}

async function get<T>(path: string): Promise<T> {
  const res = await fetch(`${BASE_URL}${path}`, {
    headers: { Accept: 'application/json' },
  });
  return handleResponse<T>(res);
}

async function post<T>(path: string, body?: unknown): Promise<T> {
  const res = await fetch(`${BASE_URL}${path}`, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      Accept: 'application/json',
    },
    body: body !== undefined ? JSON.stringify(body) : undefined,
  });
  return handleResponse<T>(res);
}

// --- Problems API ---------------------------------------------------------

export const problemsApi = {
  /** GET /api/v1/problems */
  list: (): Promise<ProblemMetadata[]> => get('/problems'),

  /** GET /api/v1/problems/:id */
  get: (id: string): Promise<ProblemMetadata> => get(`/problems/${id}`),
};

// --- Experiments API ------------------------------------------------------

export const experimentsApi = {
  /** POST /api/v1/experiments — Create a new experiment */
  create: (req: CreateExperimentRequest): Promise<ExperimentSummary> =>
    post('/experiments', req),

  /** POST /api/v1/experiments/:id/validate */
  validate: (id: string): Promise<ValidationResult> =>
    post(`/experiments/${id}/validate`),

  /** POST /api/v1/experiments/:id/run */
  run: (id: string): Promise<ExecutionResult> =>
    post(`/experiments/${id}/run`),

  /** GET /api/v1/experiments/:id */
  get: (id: string): Promise<ExperimentDetail> => get(`/experiments/${id}`),

  /** GET /api/v1/experiments/:id/status */
  getStatus: (id: string): Promise<ExecutionStatus> =>
    get(`/experiments/${id}/status`),

  /** GET /api/v1/experiments/:id/result */
  getResult: (id: string): Promise<ExecutionResult> =>
    get(`/experiments/${id}/result`),

  /** GET /api/v1/experiments/:id/counterexamples */
  getCounterexample: (id: string): Promise<CounterexampleDTO> =>
    get(`/experiments/${id}/counterexamples`),

  /** GET /api/v1/experiments/:id/forensics */
  getForensics: (id: string): Promise<ForensicReport> =>
    get(`/experiments/${id}/forensics`),

  /** POST /api/v1/experiments/:id/reproduce */
  reproduce: (id: string): Promise<ReproductionResult> =>
    post(`/experiments/${id}/reproduce`),

  /** GET /api/v1/experiments?status=...&problem_type=...&page=...&size=... */
  list: (params: {
    status?: ExperimentStatus;
    problemType?: ProblemType;
    page?: number;
    size?: number;
    sortBy?: string;
    direction?: 'asc' | 'desc';
  } = {}): Promise<PagedResponse<ExperimentSummary>> => {
    const q = new URLSearchParams();
    if (params.status) q.set('status', params.status);
    if (params.problemType) q.set('problemType', params.problemType);
    if (params.page !== undefined) q.set('page', String(params.page));
    if (params.size !== undefined) q.set('size', String(params.size));
    if (params.sortBy) q.set('sortBy', params.sortBy);
    if (params.direction) q.set('direction', params.direction);
    const qs = q.toString();
    return get(`/experiments${qs ? `?${qs}` : ''}`);
  },
};

// --- Error helpers --------------------------------------------------------

export function isApiError(err: unknown): err is ApiError {
  return typeof err === 'object' && err !== null && 'status' in err && 'detail' in err;
}

export function getErrorMessage(err: unknown): string {
  if (isApiError(err)) {
    return err.detail || `Request failed with status ${err.status}`;
  }
  if (err instanceof Error) {
    return err.message;
  }
  return 'An unexpected error occurred.';
}
