from fastapi import APIRouter, Depends
from backend.app.models.verification import (
    VerificationRequestModel,
    VerificationResponseModel,
)
from backend.app.services.investigation_service import InvestigationService

router = APIRouter(tags=["Verification"])


def get_investigation_service() -> InvestigationService:
    return InvestigationService()


@router.post(
    "/verify",
    response_model=VerificationResponseModel,
    summary="Direct Candidate Verification",
    description="Directly evaluates a user-supplied candidate instance using Greedy and Exact Oracle solvers, producing comparison metrics and divergence analysis without searching."
)
async def verify_candidate(
    request: VerificationRequestModel,
    service: InvestigationService = Depends(get_investigation_service)
) -> VerificationResponseModel:
    return await service.verify(request)
