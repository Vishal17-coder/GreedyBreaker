import logging
from fastapi import FastAPI, Request, status
from fastapi.encoders import jsonable_encoder
from fastapi.exceptions import RequestValidationError
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse

from backend.app.config import get_settings
from backend.app.api import api_router
from backend.app.services.cpp_engine import (
    EngineNotFoundError,
    EngineTimeoutError,
    EngineExecutionError,
    EngineMalformedJsonError,
)

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(name)s: %(message)s"
)
logger = logging.getLogger("greedybreaker.api")

settings = get_settings()

app = FastAPI(
    title=settings.app_name,
    version=settings.app_version,
    description="Automated Counterexample Discovery for Greedy Algorithms — DAA Investigation Engine API",
    docs_url="/docs",
    redoc_url="/redoc",
    openapi_url="/openapi.json"
)

# Configure CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=settings.cors_origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


@app.exception_handler(EngineNotFoundError)
async def engine_not_found_handler(_request: Request, exc: EngineNotFoundError):
    logger.error(f"Engine not found: {exc.message}")
    return JSONResponse(
        status_code=status.HTTP_502_BAD_GATEWAY,
        content={"status": "error", "error": {"code": exc.code, "message": exc.message, "details": exc.details}}
    )


@app.exception_handler(EngineTimeoutError)
async def engine_timeout_handler(_request: Request, exc: EngineTimeoutError):
    logger.error(f"Engine timed out: {exc.message}")
    return JSONResponse(
        status_code=status.HTTP_504_GATEWAY_TIMEOUT,
        content={"status": "error", "error": {"code": exc.code, "message": exc.message, "details": exc.details}}
    )


@app.exception_handler(EngineExecutionError)
async def engine_execution_handler(_request: Request, exc: EngineExecutionError):
    logger.error(f"Engine execution error [{exc.code}]: {exc.message}")
    # Distinguish client errors (e.g. invalid bounds) from engine crashes
    status_code = status.HTTP_400_BAD_REQUEST if "INVALID" in exc.code else status.HTTP_502_BAD_GATEWAY
    return JSONResponse(
        status_code=status_code,
        content={"status": "error", "error": {"code": exc.code, "message": exc.message, "details": exc.details}}
    )


@app.exception_handler(EngineMalformedJsonError)
async def engine_malformed_json_handler(_request: Request, exc: EngineMalformedJsonError):
    logger.error(f"Malformed engine JSON output: {exc.message}")
    return JSONResponse(
        status_code=status.HTTP_502_BAD_GATEWAY,
        content={"status": "error", "error": {"code": exc.code, "message": exc.message, "details": exc.details}}
    )


@app.exception_handler(RequestValidationError)
async def validation_exception_handler(_request: Request, exc: RequestValidationError):
    logger.warning(f"Request validation error: {exc.errors()}")
    first_error = exc.errors()[0] if exc.errors() else {"loc": [], "msg": "Validation error"}
    loc = ".".join(str(l) for l in first_error.get("loc", []))
    msg = f"{loc}: {first_error.get('msg', 'Invalid field')}" if loc else first_error.get('msg', 'Validation error')
    return JSONResponse(
        status_code=status.HTTP_400_BAD_REQUEST,
        content={
            "status": "error",
            "error": {
                "code": "VALIDATION_ERROR",
                "message": msg,
                "details": jsonable_encoder(exc.errors())
            }
        }
    )


# Include API routes
app.include_router(api_router)


@app.get("/", tags=["Root"])
async def root():
    return {
        "name": settings.app_name,
        "version": settings.app_version,
        "docs": "/docs",
        "health": "/api/v1/health",
        "problems": "/api/v1/problems"
    }
