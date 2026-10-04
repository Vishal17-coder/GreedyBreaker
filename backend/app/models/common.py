from enum import Enum
from typing import Any, List, Optional
from pydantic import BaseModel, Field


class ProblemObjective(str, Enum):
    MINIMIZATION = "MINIMIZATION"
    MAXIMIZATION = "MAXIMIZATION"


class SearchStrategy(str, Enum):
    EXHAUSTIVE = "EXHAUSTIVE"
    BACKTRACKING = "BACKTRACKING"
    RANDOMIZED = "RANDOMIZED"
    HEURISTIC = "HEURISTIC"


class SearchMode(str, Enum):
    ANY = "ANY"
    SMALLEST = "SMALLEST"
    WORST = "WORST"


class ProblemTypeEnum(str, Enum):
    COIN_CHANGE = "coin_change"
    ACTIVITY_SELECTION = "activity_selection"
    KNAPSACK_01 = "knapsack_01"
    KNAPSACK_FRACTIONAL = "knapsack_fractional"


class TraceStepModel(BaseModel):
    step_number: int = Field(..., description="1-indexed step count")
    state_description: str = Field(..., description="Current state summary before decision")
    remaining_target: int = Field(..., description="Remaining capacity or target amount")
    available_choices: List[int] = Field(default_factory=list, description="Available valid choices at this state")
    selected_choice: Optional[int] = Field(None, description="Choice chosen at this step, if any")
    outcome_description: str = Field(..., description="Action description")
    remaining_after: int = Field(..., description="Remaining capacity/target after choice")
    accumulated_count: int = Field(0, description="Running cost or value accumulated")
    rationale: str = Field(..., description="Algorithmic rationale for selection or skipping")


class AlgorithmTraceModel(BaseModel):
    algorithm_name: str
    strategy_name: str
    success: bool
    steps: List[TraceStepModel] = Field(default_factory=list)
    final_solution: List[int] = Field(default_factory=list)
    final_cost: Optional[float] = None
    execution_notes: List[str] = Field(default_factory=list)


class ComparisonMetricsModel(BaseModel):
    is_counterexample: bool
    objective: ProblemObjective
    greedy_solved: bool
    optimal_solved: bool
    greedy_cost: Optional[float] = None
    optimal_cost: Optional[float] = None
    absolute_gap: Optional[float] = None
    ratio: Optional[float] = None
    overhead_percentage: Optional[float] = None
    interpretation: str


class DivergenceModel(BaseModel):
    has_diverged: bool
    divergence_step: Optional[int] = None
    state_remaining: int = 0
    greedy_choice: Optional[int] = None
    optimal_choices: List[int] = Field(default_factory=list)
    greedy_projected_cost: Optional[float] = None
    optimal_projected_cost: Optional[float] = None
    explanation: str = ""


class MinimalityModel(BaseModel):
    verified: bool = Field(..., description="True if canonical search guarantees minimality within bounds")
    description: str


class SearchStatisticsModel(BaseModel):
    candidates_generated: int
    candidates_evaluated: int
    counterexamples_found: int
    nodes_explored: int = 0
    branches_pruned: int = 0
    iterations: int = 0
    duration_ms: float
    search_completed: bool
    search_limit_reached: bool
    termination_reason: str


class APIErrorDetail(BaseModel):
    code: str
    message: str
    details: Optional[Any] = None


class APIErrorResponse(BaseModel):
    status: str = "error"
    error: APIErrorDetail
