from typing import Any, Dict, List
from pydantic import BaseModel, Field
from backend.app.models.common import ProblemObjective


class SolverComplexityModel(BaseModel):
    algorithm_name: str
    time_best: str
    time_average: str
    time_worst: str
    space_worst: str
    explanation: str


class ProblemMetadataModel(BaseModel):
    id: str = Field(..., description="Unique problem identifier slug")
    name: str = Field(..., description="Human-readable problem name")
    objective: ProblemObjective = Field(..., description="Optimization direction (MINIMIZATION or MAXIMIZATION)")
    description: str = Field(..., description="Formal DAA problem description")
    greedy_strategy: str = Field(..., description="Heuristic greedy strategy under investigation")
    oracle_strategy: str = Field(..., description="Exact algorithm used as ground-truth oracle")
    can_fail: bool = Field(..., description="Whether a greedy heuristic is provably susceptible to counterexamples")
    default_config: Dict[str, Any] = Field(default_factory=dict, description="Recommended default search bounds")
    solvers: List[SolverComplexityModel] = Field(default_factory=list, description="Asymptotic complexity specifications")
