from typing import Any, Dict, Optional
from pydantic import BaseModel, Field
from backend.app.models.common import (
    ProblemTypeEnum,
    ComparisonMetricsModel,
    DivergenceModel,
)


class VerificationRequestModel(BaseModel):
    problem: ProblemTypeEnum = Field(..., description="Problem type for candidate instance")
    candidate: Dict[str, Any] = Field(..., description="Candidate instance parameters")


class VerificationResponseModel(BaseModel):
    status: str = Field(default="success")
    problem: str
    counterexample_found: bool
    candidate: Dict[str, Any]
    greedy_solution: Dict[str, Any]
    optimal_solution: Dict[str, Any]
    metrics: ComparisonMetricsModel
    divergence: DivergenceModel
    error: Optional[Dict[str, Any]] = None
