import logging
from typing import Any, Dict, List, Optional
from backend.app.models.problems import ProblemMetadataModel
from backend.app.models.investigation import (
    InvestigationRequestModel,
    InvestigationResponseModel,
)
from backend.app.models.verification import (
    VerificationRequestModel,
    VerificationResponseModel,
)
from backend.app.services.cpp_engine import CppEngineService

logger = logging.getLogger(__name__)


class InvestigationService:
    """
    Application-level orchestration service for counterexample discovery and verification.
    """

    def __init__(self, engine_service: Optional[CppEngineService] = None):
        self.engine = engine_service or CppEngineService()

    async def get_problems(self) -> List[ProblemMetadataModel]:
        """Fetches the authoritative list of registered problems directly from C++."""
        raw_catalog = await self.engine.get_problem_catalog()
        return [ProblemMetadataModel.model_validate(p) for p in raw_catalog]

    async def investigate(self, request: InvestigationRequestModel) -> InvestigationResponseModel:
        """
        Executes an investigation search via the C++ engine and returns a validated Pydantic model.
        """
        config_payload = request.model_dump(mode="json")
        raw_result = await self.engine.run_investigation(config_payload)
        return InvestigationResponseModel.model_validate(raw_result)

    async def verify(self, request: VerificationRequestModel) -> VerificationResponseModel:
        """
        Evaluates a single user-supplied candidate instance directly without running a search.
        """
        verify_payload = request.model_dump(mode="json")
        raw_result = await self.engine.verify_candidate(verify_payload)
        return VerificationResponseModel.model_validate(raw_result)
