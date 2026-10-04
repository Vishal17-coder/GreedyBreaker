from fastapi.testclient import TestClient
from backend.app.config import get_settings


def test_health_check_returns_healthy(client: TestClient):
    response = client.get("/api/v1/health")
    assert response.status_code == 200
    data = response.json()
    assert data["status"] == "healthy"
    assert "engine" in data
    assert data["engine"]["status"] == "ready"
    assert data["engine"]["exists"] is True


def test_root_endpoint_links(client: TestClient):
    response = client.get("/")
    assert response.status_code == 200
    data = response.json()
    assert "docs" in data
    assert data["health"] == "/api/v1/health"
    assert data["problems"] == "/api/v1/problems"
