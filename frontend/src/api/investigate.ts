import { apiClient } from './client';
import type { InvestigationRequest, InvestigationResponse } from '../types/api';

export async function runInvestigation(request: InvestigationRequest): Promise<InvestigationResponse> {
  return apiClient<InvestigationResponse>('/api/v1/investigate', {
    method: 'POST',
    body: JSON.stringify(request),
  });
}
