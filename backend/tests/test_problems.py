from fastapi.testclient import TestClient


def test_get_problems_returns_all_four_families(client: TestClient):
    response = client.get("/api/v1/problems")
    assert response.status_code == 200
    problems = response.json()
    assert isinstance(problems, list)
    assert len(problems) == 4

    problem_ids = [p["id"] for p in problems]
    expected_ids = ["coin_change", "activity_selection", "knapsack_01", "knapsack_fractional"]
    assert problem_ids == expected_ids

    for p in problems:
        assert "name" in p
        assert "objective" in p
        assert p["objective"] in ["MINIMIZATION", "MAXIMIZATION"]
        assert "description" in p
        assert "greedy_strategy" in p
        assert "oracle_strategy" in p
        assert "can_fail" in p
        assert "default_config" in p
        assert "solvers" in p
        assert len(p["solvers"]) >= 1

        for solver in p["solvers"]:
            assert "algorithm_name" in solver
            assert "time_best" in solver
            assert "time_average" in solver
            assert "time_worst" in solver
            assert "space_worst" in solver
            assert "explanation" in solver


def test_problem_objectives_correctly_classified(client: TestClient):
    response = client.get("/api/v1/problems")
    problems = {p["id"]: p for p in response.json()}

    assert problems["coin_change"]["objective"] == "MINIMIZATION"
    assert problems["coin_change"]["can_fail"] is True

    assert problems["activity_selection"]["objective"] == "MAXIMIZATION"
    assert problems["activity_selection"]["can_fail"] is False

    assert problems["knapsack_01"]["objective"] == "MAXIMIZATION"
    assert problems["knapsack_01"]["can_fail"] is True

    assert problems["knapsack_fractional"]["objective"] == "MAXIMIZATION"
    assert problems["knapsack_fractional"]["can_fail"] is False
