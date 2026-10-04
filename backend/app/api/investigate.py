from fastapi import APIRouter, Depends
from backend.app.models.investigation import (
    InvestigationRequestModel,
    InvestigationResponseModel,
)
from backend.app.services.investigation_service import InvestigationService

router = APIRouter(tags=["Investigation"])


def get_investigation_service() -> InvestigationService:
    return InvestigationService()


@router.post(
    "/investigate",
    response_model=InvestigationResponseModel,
    summary="Execute Counterexample Investigation",
    description="Launches an automated counterexample search in the C++ engine according to the selected strategy and bounds."
)
async def run_investigation(
    request: InvestigationRequestModel,
    service: InvestigationService = Depends(get_investigation_service)
) -> InvestigationResponseModel:
    return await service.investigate(request)
