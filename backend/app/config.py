import os
import sys
from functools import lru_cache
from pathlib import Path
from typing import List
from pydantic_settings import BaseSettings, SettingsConfigDict


def get_default_cli_path() -> str:
    """Computes a repository-relative default path for greedybreaker_cli executable."""
    repo_root = Path(__file__).resolve().parent.parent.parent
    exe_name = "greedybreaker_cli.exe" if sys.platform == "win32" else "greedybreaker_cli"

    candidate_paths = [
        repo_root / "engine" / "build" / exe_name,
        repo_root / "engine" / "build" / "Release" / exe_name,
        repo_root / "engine" / "build" / "Debug" / exe_name,
        repo_root / "build" / exe_name,
    ]

    for p in candidate_paths:
        if p.exists() and p.is_file():
            return str(p)

    # Fallback to primary expected build location even if not yet compiled
    return str(repo_root / "engine" / "build" / exe_name)


class Settings(BaseSettings):
    model_config = SettingsConfigDict(
        env_file=".env",
        env_file_encoding="utf-8",
        extra="ignore"
    )

    app_name: str = "GreedyBreaker API"
    app_version: str = "1.0.0"
    environment: str = "development"
    debug: bool = False

    api_host: str = "127.0.0.1"
    api_port: int = 8000

    # Path to C++20 greedybreaker_cli executable
    greedybreaker_cli_path: str = os.getenv("GREEDYBREAKER_CLI_PATH", get_default_cli_path())

    # Subprocess execution timeout in seconds
    subprocess_timeout_seconds: float = 30.0

    # CORS configuration for frontend clients
    cors_origins: List[str] = [
        "http://localhost:5173",
        "http://127.0.0.1:5173",
        "http://localhost:3000",
        "http://127.0.0.1:3000",
    ]


@lru_cache()
def get_settings() -> Settings:
    return Settings()
