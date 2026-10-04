# GreedyBreaker FastAPI Backend

FastAPI + Pydantic v2 orchestration backend for **GreedyBreaker: Automated Counterexample Discovery for Greedy Algorithms**.

The C++20 engine (`greedybreaker_cli`) acts as the authoritative algorithm engine. This backend provides a clean RESTful interface, strong request/response typing, CORS middleware, subprocess timeout enforcement, and structured error handling.

## Requirements
* Python 3.11+
* Compiled C++20 engine binary (`greedybreaker_cli.exe` or `greedybreaker_cli`)

## Installation
```bash
pip install -r requirements.txt
```

## Running the Server
```bash
uvicorn backend.app.main:app --host 127.0.0.1 --port 8000 --reload
```
Interactive OpenAPI documentation will be accessible at:
* Swagger UI: http://127.0.0.1:8000/docs
* ReDoc: http://127.0.0.1:8000/redoc

## Configuration
Configuration can be customized via environment variables:
* `GREEDYBREAKER_CLI_PATH`: Path to the compiled `greedybreaker_cli` executable.
* `SUBPROCESS_TIMEOUT_SECONDS`: Subprocess timeout limit in seconds (default `30.0`).
* `CORS_ORIGINS`: Comma-separated list of allowed origins.

## Endpoints
* `GET /api/v1/health`: Operational status and C++ engine verification.
* `GET /api/v1/problems`: Catalog of registered DAA problems directly from C++.
* `POST /api/v1/investigate`: Executes automated counterexample search.
* `POST /api/v1/verify`: Direct evaluation of a specific candidate instance without search.
