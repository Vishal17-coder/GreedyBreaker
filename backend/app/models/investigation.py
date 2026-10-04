from typing import Any, Dict, Optional
from pydantic import BaseModel, Field, model_validator
from backend.app.models.common import (
    ProblemTypeEnum,
    SearchStrategy,
    SearchMode,
    SearchStatisticsModel,
    MinimalityModel,
    ComparisonMetricsModel,
    DivergenceModel,
)


class InvestigationRequestModel(BaseModel):
    problem: ProblemTypeEnum = Field(..., description="Target problem family to investigate")
    strategy: SearchStrategy = Field(default=SearchStrategy.EXHAUSTIVE, description="Search algorithm strategy")
    mode: SearchMode = Field(default=SearchMode.SMALLEST, description="Investigation objective mode")

    # Coin Change parameters
    min_denomination: int = Field(default=1, ge=1, le=100)
    max_denomination: int = Field(default=10, ge=1, le=1000)
    min_coin_count: int = Field(default=2, ge=1, le=20)
    max_coin_count: int = Field(default=4, ge=1, le=20)
    min_target: int = Field(default=1, ge=1, le=10000)
    max_target: int = Field(default=30, ge=1, le=10000)
    require_unit_coin: bool = Field(default=True)

    # Activity Selection parameters
    min_activities: int = Field(default=3, ge=1, le=50)
    max_activities: int = Field(default=5, ge=1, le=50)
    min_time: int = Field(default=0, ge=0, le=1000)
    max_time: int = Field(default=10, ge=1, le=1000)
    min_duration: int = Field(default=1, ge=1, le=100)

    # Knapsack parameters
    min_items: int = Field(default=3, ge=1, le=50)
    max_items: int = Field(default=5, ge=1, le=50)
    min_weight: int = Field(default=1, ge=1, le=1000)
    max_weight: int = Field(default=10, ge=1, le=1000)
    min_value: int = Field(default=1, ge=1, le=1000)
    max_value: int = Field(default=20, ge=1, le=1000)
    min_capacity: int = Field(default=5, ge=1, le=1000)
    max_capacity: int = Field(default=20, ge=1, le=1000)

    # Safety and PRNG parameters
    random_seed: int = Field(default=42, ge=0)
    max_iterations: int = Field(default=10000, ge=1, le=1000000)
    max_candidates: int = Field(default=50000, ge=1, le=500000)

    @model_validator(mode="after")
    def validate_bounds(self) -> "InvestigationRequestModel":
        if self.min_denomination > self.max_denomination:
            raise ValueError(f"min_denomination ({self.min_denomination}) cannot exceed max_denomination ({self.max_denomination})")
        if self.min_coin_count > self.max_coin_count:
            raise ValueError(f"min_coin_count ({self.min_coin_count}) cannot exceed max_coin_count ({self.max_coin_count})")
        if self.min_target > self.max_target:
            raise ValueError(f"min_target ({self.min_target}) cannot exceed max_target ({self.max_target})")

        if self.min_activities > self.max_activities:
            raise ValueError(f"min_activities ({self.min_activities}) cannot exceed max_activities ({self.max_activities})")
        if self.min_time >= self.max_time:
            raise ValueError(f"min_time ({self.min_time}) must be less than max_time ({self.max_time})")

        if self.min_items > self.max_items:
            raise ValueError(f"min_items ({self.min_items}) cannot exceed max_items ({self.max_items})")
        if self.min_weight > self.max_weight:
            raise ValueError(f"min_weight ({self.min_weight}) cannot exceed max_weight ({self.max_weight})")
        if self.min_value > self.max_value:
            raise ValueError(f"min_value ({self.min_value}) cannot exceed max_value ({self.max_value})")
        if self.min_capacity > self.max_capacity:
            raise ValueError(f"min_capacity ({self.min_capacity}) cannot exceed max_capacity ({self.max_capacity})")

        return self


class InvestigationResponseModel(BaseModel):
    status: str = Field(default="success")
    problem: str
    message: str
    counterexample_found: bool
    search_config: Dict[str, Any]
    search_statistics: SearchStatisticsModel
    minimality: MinimalityModel
    candidate_summary: Optional[str] = None
    candidate: Optional[Dict[str, Any]] = None
    greedy_solution: Optional[Dict[str, Any]] = None
    optimal_solution: Optional[Dict[str, Any]] = None
    metrics: Optional[ComparisonMetricsModel] = None
    divergence: Optional[DivergenceModel] = None
    error: Optional[Dict[str, Any]] = None
