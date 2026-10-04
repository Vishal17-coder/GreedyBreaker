from fastapi.testclient import TestClient


def test_verify_coin_change_counterexample(client: TestClient):
    payload = {
        "problem": "coin_change",
        "candidate": {
            "denominations": [1, 3, 4],
            "target": 6
        }
    }
    response = client.post("/api/v1/verify", json=payload)
    assert response.status_code == 200
    data = response.json()

    assert data["status"] == "success"
    assert data["problem"] == "coin-change"
    assert data["counterexample_found"] is True

    assert data["greedy_solution"]["cost"] == 3.0
    assert data["optimal_solution"]["cost"] == 2.0
    assert data["metrics"]["absolute_gap"] == 1.0
    assert data["metrics"]["ratio"] == 1.5
    assert data["divergence"]["has_diverged"] is True
    assert data["divergence"]["greedy_choice"] == 4


def test_verify_knapsack_01_counterexample(client: TestClient):
    payload = {
        "problem": "knapsack_01",
        "candidate": {
            "capacity": 5,
            "items": [
                {"id": 1, "value": 20, "weight": 1},
                {"id": 2, "value": 19, "weight": 1},
                {"id": 3, "value": 20, "weight": 4}
            ]
        }
    }
    response = client.post("/api/v1/verify", json=payload)
    assert response.status_code == 200
    data = response.json()

    assert data["status"] == "success"
    assert data["problem"] == "knapsack-01"
    assert data["counterexample_found"] is True
    assert data["greedy_solution"]["cost"] == 39.0
    assert data["optimal_solution"]["cost"] == 40.0
    assert data["metrics"]["absolute_gap"] == 1.0


def test_verify_activity_selection_optimal_match(client: TestClient):
    payload = {
        "problem": "activity_selection",
        "candidate": {
            "activities": [
                {"id": 1, "start": 1, "finish": 4},
                {"id": 2, "start": 3, "finish": 5},
                {"id": 3, "start": 0, "finish": 6},
                {"id": 4, "start": 5, "finish": 7}
            ]
        }
    }
    response = client.post("/api/v1/verify", json=payload)
    assert response.status_code == 200
    data = response.json()

    assert data["status"] == "success"
    assert data["problem"] == "activity-selection"
    assert data["counterexample_found"] is False
    assert data["greedy_solution"]["cost"] == 2.0
    assert data["optimal_solution"]["cost"] == 2.0
    assert data["metrics"]["absolute_gap"] == 0.0


def test_verify_fractional_knapsack_optimal_match(client: TestClient):
    payload = {
        "problem": "knapsack_fractional",
        "candidate": {
            "capacity": 15,
            "items": [
                {"id": 1, "value": 60, "weight": 10},
                {"id": 2, "value": 100, "weight": 20}
            ]
        }
    }
    response = client.post("/api/v1/verify", json=payload)
    assert response.status_code == 200
    data = response.json()

    assert data["status"] == "success"
    assert data["problem"] == "knapsack-fractional"
    assert data["counterexample_found"] is False
    assert data["greedy_solution"]["cost"] == 85.0
    assert data["optimal_solution"]["cost"] == 85.0
