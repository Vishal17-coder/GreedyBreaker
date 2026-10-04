from fastapi import APIRouter, HTTPException
from backend.app.models.benchmark import (
    BenchmarkRequestModel,
    BenchmarkResponseModel,
)

router = APIRouter(tags=["Benchmarks"])


@router.post(
    "/benchmarks",
    response_model=BenchmarkResponseModel,
    summary="Benchmark Comparative Strategies",
    description="Comparative analysis across Exhaustive, Backtracking, Randomized, and Heuristic search strategies (Scaffolding for future Benchmark Lab)."
)
async def run_benchmark(_request: BenchmarkRequestModel):
    # Will be fully fleshed out in subsequent milestone after investigation is proven end-to-end
    raise HTTPException(
        status_code=501,
        detail="Benchmark Lab is scheduled for post-investigation milestone."
    )
