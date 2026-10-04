from fastapi import APIRouter, Depends
from backend.app.config import Settings, get_settings
from backend.app.services.cpp_engine import CppEngineService

router = APIRouter(tags=["Health"])


@router.get(
    "/health",
    summary="Service Health Check",
    description="Reports API operational status and verifies availability of the C++20 algorithm engine."
)
async def health_check(settings: Settings = Depends(get_settings)):
    engine = CppEngineService(cli_path=settings.greedybreaker_cli_path)
    engine_available = engine.check_binary_exists()

    return {
        "status": "healthy",
        "app_name": settings.app_name,
        "version": settings.app_version,
        "engine": {
            "status": "ready" if engine_available else "unavailable",
            "path": settings.greedybreaker_cli_path,
            "exists": engine_available
        }
    }
