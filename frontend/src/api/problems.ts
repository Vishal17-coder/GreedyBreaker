import { apiClient } from './client';
import type { HealthResponse, ProblemMetadata } from '../types/api';

export async function getHealth(): Promise<HealthResponse> {
  return apiClient<HealthResponse>('/api/v1/health');
}

export async function getProblems(): Promise<ProblemMetadata[]> {
  return apiClient<ProblemMetadata[]>('/api/v1/problems');
}
