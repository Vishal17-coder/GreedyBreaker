import { apiClient } from './client';
import type { VerificationRequest, VerificationResponse } from '../types/api';

export async function verifyCandidate(request: VerificationRequest): Promise<VerificationResponse> {
  return apiClient<VerificationResponse>('/api/v1/verify', {
    method: 'POST',
    body: JSON.stringify(request),
  });
}
