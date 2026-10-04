from fastapi.testclient import TestClient


def test_investigate_coin_change_smallest_finds_counterexample(client: TestClient):
    payload = {
        "problem": "coin_change",
        "strategy": "EXHAUSTIVE",
        "mode": "SMALLEST",
        "min_denomination": 1,
        "max_denomination": 6,
        "min_coin_count": 2,
        "max_coin_count": 3,
        "min_target": 1,
        "max_target": 10
    }
    response = client.post("/api/v1/investigate", json=payload)
    assert response.status_code == 200
    data = response.json()

    assert data["status"] == "success"
    assert data["problem"] == "coin-change"
    assert data["counterexample_found"] is True
    assert data["minimality"]["verified"] is True

    # Candidate should be {1, 3, 4}, target 6
    cand = data["candidate"]
    assert cand["denominations"] == [1, 3, 4]
    assert cand["target"] == 6

    # Greedy vs Optimal
    assert data["greedy_solution"]["cost"] == 3.0
    assert data["greedy_solution"]["items"] == [4, 1, 1]

    assert data["optimal_solution"]["cost"] == 2.0
    assert data["optimal_solution"]["items"] == [3, 3]

    # Metrics
    metrics = data["metrics"]
    assert metrics["is_counterexample"] is True
    assert metrics["absolute_gap"] == 1.0
    assert metrics["ratio"] == 1.5
    assert metrics["overhead_percentage"] == 50.0

    # Divergence
    div = data["divergence"]
    assert div["has_diverged"] is True
    assert div["divergence_step"] == 1
    assert div["greedy_choice"] == 4
    assert 3 in div["optimal_choices"]


def test_investigate_knapsack_01_smallest_finds_counterexample(client: TestClient):
    payload = {
        "problem": "knapsack_01",
        "strategy": "EXHAUSTIVE",
        "mode": "SMALLEST",
        "min_capacity": 5,
        "max_capacity": 6,
        "min_items": 3,
        "max_items": 3,
        "min_weight": 1,
        "max_weight": 4,
        "min_value": 1,
        "max_value": 6
    }
    response = client.post("/api/v1/investigate", json=payload)
    assert response.status_code == 200
    data = response.json()

    assert data["status"] == "success"
    assert data["problem"] == "knapsack-01"
    assert data["counterexample_found"] is True
    assert data["minimality"]["verified"] is True
    assert data["candidate"] is not None
    assert data["metrics"]["is_counterexample"] is True
    assert data["metrics"]["absolute_gap"] > 0
    assert data["divergence"]["has_diverged"] is True


def test_investigate_activity_selection_finds_no_counterexample(client: TestClient):
    payload = {
        "problem": "activity_selection",
        "strategy": "EXHAUSTIVE",
        "mode": "ANY",
        "min_activities": 3,
        "max_activities": 3,
        "min_time": 0,
        "max_time": 5
    }
    response = client.post("/api/v1/investigate", json=payload)
    assert response.status_code == 200
    data = response.json()

    assert data["status"] == "success"
    assert data["problem"] == "activity-selection"
    assert data["counterexample_found"] is False
    assert data["message"] == "No counterexample was found within the configured search space."
    assert data["candidate"] is None
    assert data["greedy_solution"] is None
    assert data["optimal_solution"] is None
    assert data["metrics"] is None


def test_investigate_fractional_knapsack_finds_no_counterexample(client: TestClient):
    payload = {
        "problem": "knapsack_fractional",
        "strategy": "EXHAUSTIVE",
        "mode": "ANY",
        "min_capacity": 5,
        "max_capacity": 6,
        "min_items": 3,
        "max_items": 3,
        "min_weight": 1,
        "max_weight": 3,
        "min_value": 1,
        "max_value": 3
    }
    response = client.post("/api/v1/investigate", json=payload)
    assert response.status_code == 200
    data = response.json()

    assert data["status"] == "success"
    assert data["problem"] == "knapsack-fractional"
    assert data["counterexample_found"] is False
    assert data["message"] == "No counterexample was found within the configured search space."
