from typing import Any, Dict, List
from pydantic import BaseModel, Field
from backend.app.models.common import ProblemTypeEnum, SearchStrategy


class BenchmarkRequestModel(BaseModel):
    problem: ProblemTypeEnum = Field(..., description="Problem family to benchmark")
    strategies: List[SearchStrategy] = Field(
        default=[SearchStrategy.EXHAUSTIVE, SearchStrategy.BACKTRACKING, SearchStrategy.RANDOMIZED, SearchStrategy.HEURISTIC],
        description="Search strategies to benchmark comparatively"
    )
    config_overrides: Dict[str, Any] = Field(default_factory=dict, description="Shared search parameters for all runs")


class BenchmarkStrategyResult(BaseModel):
    strategy: SearchStrategy
    duration_ms: float
    candidates_evaluated: int
    counterexample_found: bool
    counterexamples_count: int
    branches_pruned: int = 0
    iterations: int = 0


class BenchmarkResponseModel(BaseModel):
    status: str = "success"
    problem: str
    benchmark_results: List[BenchmarkStrategyResult] = Field(default_factory=list)
