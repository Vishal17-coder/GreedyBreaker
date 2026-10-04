from fastapi.testclient import TestClient


def test_invalid_problem_name_rejected(client: TestClient):
    payload = {
        "problem": "traveling_salesperson",
        "strategy": "EXHAUSTIVE",
        "mode": "ANY"
    }
    response = client.post("/api/v1/investigate", json=payload)
    assert response.status_code == 400
    data = response.json()
    assert data["status"] == "error"
    assert data["error"]["code"] == "VALIDATION_ERROR"


def test_invalid_strategy_rejected(client: TestClient):
    payload = {
        "problem": "coin_change",
        "strategy": "QUANTUM_ANNEALING",
        "mode": "ANY"
    }
    response = client.post("/api/v1/investigate", json=payload)
    assert response.status_code == 400
    data = response.json()
    assert data["status"] == "error"
    assert data["error"]["code"] == "VALIDATION_ERROR"


def test_invalid_mode_rejected(client: TestClient):
    payload = {
        "problem": "coin_change",
        "strategy": "EXHAUSTIVE",
        "mode": "MAGIC_MODE"
    }
    response = client.post("/api/v1/investigate", json=payload)
    assert response.status_code == 400
    data = response.json()
    assert data["status"] == "error"


def test_inverted_denominations_rejected(client: TestClient):
    payload = {
        "problem": "coin_change",
        "min_denomination": 20,
        "max_denomination": 10
    }
    response = client.post("/api/v1/investigate", json=payload)
    assert response.status_code == 400
    data = response.json()
    assert data["status"] == "error"
    assert "min_denomination" in data["error"]["message"]


def test_inverted_capacity_rejected(client: TestClient):
    payload = {
        "problem": "knapsack_01",
        "min_capacity": 50,
        "max_capacity": 10
    }
    response = client.post("/api/v1/investigate", json=payload)
    assert response.status_code == 400
    data = response.json()
    assert data["status"] == "error"
    assert "min_capacity" in data["error"]["message"]


def test_negative_target_rejected(client: TestClient):
    payload = {
        "problem": "coin_change",
        "min_target": -5
    }
    response = client.post("/api/v1/investigate", json=payload)
    assert response.status_code == 400
    data = response.json()
    assert data["status"] == "error"
