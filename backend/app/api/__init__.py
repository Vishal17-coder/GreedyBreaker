from fastapi import APIRouter
from backend.app.api.health import router as health_router
from backend.app.api.problems import router as problems_router
from backend.app.api.investigate import router as investigate_router
from backend.app.api.verify import router as verify_router
from backend.app.api.benchmarks import router as benchmarks_router

api_router = APIRouter(prefix="/api/v1")
api_router.include_router(health_router)
api_router.include_router(problems_router)
api_router.include_router(investigate_router)
api_router.include_router(verify_router)
api_router.include_router(benchmarks_router)

__all__ = ["api_router"]
