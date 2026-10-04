from typing import List
from fastapi import APIRouter, Depends
from backend.app.models.problems import ProblemMetadataModel
from backend.app.services.investigation_service import InvestigationService

router = APIRouter(tags=["Problems"])


def get_investigation_service() -> InvestigationService:
    return InvestigationService()


@router.get(
    "/problems",
    response_model=List[ProblemMetadataModel],
    summary="List Registered DAA Problems",
    description="Returns the problem catalog directly from the authoritative C++20 engine, including default bounds and asymptotic complexity specifications."
)
async def list_problems(
    service: InvestigationService = Depends(get_investigation_service)
) -> List[ProblemMetadataModel]:
    return await service.get_problems()
