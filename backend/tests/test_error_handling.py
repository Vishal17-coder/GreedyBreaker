from unittest.mock import AsyncMock, patch
from fastapi.testclient import TestClient
from backend.app.services.cpp_engine import (
    EngineNotFoundError,
    EngineTimeoutError,
    EngineMalformedJsonError,
    EngineExecutionError,
)


def test_missing_executable_returns_502(client: TestClient):
    with patch(
        "backend.app.services.cpp_engine.CppEngineService._execute_subprocess",
        side_effect=EngineNotFoundError("/nonexistent/greedybreaker_cli.exe")
    ):
        response = client.get("/api/v1/problems")
        assert response.status_code == 502
        data = response.json()
        assert data["status"] == "error"
        assert data["error"]["code"] == "ENGINE_NOT_FOUND"


def test_subprocess_timeout_returns_504(client: TestClient):
    with patch(
        "backend.app.services.cpp_engine.CppEngineService._execute_subprocess",
        side_effect=EngineTimeoutError(5.0)
    ):
        payload = {
            "problem": "coin_change",
            "strategy": "EXHAUSTIVE",
            "mode": "SMALLEST"
        }
        response = client.post("/api/v1/investigate", json=payload)
        assert response.status_code == 504
        data = response.json()
        assert data["status"] == "error"
        assert data["error"]["code"] == "ENGINE_TIMEOUT"


def test_malformed_json_output_returns_502(client: TestClient):
    with patch(
        "backend.app.services.cpp_engine.CppEngineService._execute_subprocess",
        side_effect=EngineMalformedJsonError("not json at all", "Expecting value")
    ):
        payload = {
            "problem": "coin_change",
            "strategy": "EXHAUSTIVE",
            "mode": "SMALLEST"
        }
        response = client.post("/api/v1/investigate", json=payload)
        assert response.status_code == 502
        data = response.json()
        assert data["status"] == "error"
        assert data["error"]["code"] == "ENGINE_MALFORMED_JSON"


def test_engine_execution_error_returns_appropriate_status(client: TestClient):
    with patch(
        "backend.app.services.cpp_engine.CppEngineService._execute_subprocess",
        side_effect=EngineExecutionError("INVALID_BOUNDS", "Denomination bounds invalid", exit_code=1)
    ):
        payload = {
            "problem": "coin_change",
            "strategy": "EXHAUSTIVE",
            "mode": "SMALLEST"
        }
        response = client.post("/api/v1/investigate", json=payload)
        assert response.status_code == 400
        data = response.json()
        assert data["status"] == "error"
        assert data["error"]["code"] == "INVALID_BOUNDS"
