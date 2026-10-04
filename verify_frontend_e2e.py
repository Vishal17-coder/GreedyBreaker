import json
import urllib.request
import urllib.error
import sys

def test_health():
    print("Testing GET /api/v1/health...")
    req = urllib.request.Request("http://127.0.0.1:8000/api/v1/health")
    with urllib.request.urlopen(req) as resp:
        data = json.loads(resp.read().decode())
        print(f"  Health status: {data.get('status')}")
        print(f"  Engine path: {data.get('engine', {}).get('path')}")
        print(f"  Engine exists: {data.get('engine', {}).get('exists')}")
        assert data.get('status') in ['ok', 'healthy'], f"Health status unexpected: {data.get('status')}"
        assert data.get('engine', {}).get('exists') is True, "C++ Engine not found"
        print("  -> PASS\n")

def test_problems():
    print("Testing 1. GET /api/v1/problems (4 problem families)...")
    req = urllib.request.Request("http://127.0.0.1:8000/api/v1/problems")
    with urllib.request.urlopen(req) as resp:
        data = json.loads(resp.read().decode())
        ids = [p['id'] for p in data]
        print(f"  Loaded problems: {ids}")
        expected = ['coin_change', 'activity_selection', 'knapsack_01', 'knapsack_fractional']
        assert ids == expected, f"Expected {expected}, got {ids}"
        print("  -> PASS\n")

def test_coin_change_break_it():
    print("Testing 2. Coin Change Break It (finding {1,3,4}, target 6)...")
    payload = {
        "problem": "coin_change",
        "strategy": "BACKTRACKING",
        "mode": "SMALLEST",
        "min_denomination": 1,
        "max_denomination": 10,
        "min_coin_count": 3,
        "max_coin_count": 4,
        "min_target": 1,
        "max_target": 12,
        "require_unit_coin": True,
        "max_candidates": 5000
    }
    req = urllib.request.Request(
        "http://127.0.0.1:8000/api/v1/investigate",
        data=json.dumps(payload).encode(),
        headers={"Content-Type": "application/json"}
    )
    with urllib.request.urlopen(req) as resp:
        data = json.loads(resp.read().decode())
        print(f"  Counterexample found: {data.get('counterexample_found')}")
        print(f"  Candidate: {data.get('candidate')}")
        print(f"  Greedy cost: {data.get('metrics', {}).get('greedy_cost')}")
        print(f"  Optimal cost: {data.get('metrics', {}).get('optimal_cost')}")
        print(f"  First divergence: step {data.get('divergence', {}).get('divergence_step')}, rationale: {data.get('divergence', {}).get('explanation')}")
        assert data.get('counterexample_found') is True
        cand = data.get('candidate', {})
        assert cand.get('denominations') == [1, 3, 4]
        assert cand.get('target') == 6
        assert data.get('metrics', {}).get('greedy_cost') == 3
        assert data.get('metrics', {}).get('optimal_cost') == 2
        print("  -> PASS\n")

def test_knapsack_01_break_it():
    print("Testing 3. 0/1 Knapsack Break It (returns real counterexample)...")
    payload = {
        "problem": "knapsack_01",
        "strategy": "HEURISTIC",
        "mode": "ANY",
        "min_items": 3,
        "max_items": 4,
        "min_weight": 1,
        "max_weight": 8,
        "min_value": 1,
        "max_value": 20,
        "min_capacity": 5,
        "max_capacity": 15,
        "max_candidates": 5000
    }
    req = urllib.request.Request(
        "http://127.0.0.1:8000/api/v1/investigate",
        data=json.dumps(payload).encode(),
        headers={"Content-Type": "application/json"}
    )
    with urllib.request.urlopen(req) as resp:
        data = json.loads(resp.read().decode())
        print(f"  Counterexample found: {data.get('counterexample_found')}")
        print(f"  Candidate: {data.get('candidate')}")
        print(f"  Greedy value: {data.get('metrics', {}).get('greedy_cost')}")
        print(f"  Optimal value: {data.get('metrics', {}).get('optimal_cost')}")
        print(f"  Absolute gap: {data.get('metrics', {}).get('absolute_gap')}")
        assert data.get('counterexample_found') is True
        assert data.get('metrics', {}).get('greedy_cost') < data.get('metrics', {}).get('optimal_cost')
        print("  -> PASS\n")

def test_activity_selection_break_it():
    print("Testing 4. Activity Selection Break It (no counterexample in bounded space)...")
    payload = {
        "problem": "activity_selection",
        "strategy": "EXHAUSTIVE",
        "mode": "ANY",
        "min_activities": 3,
        "max_activities": 4,
        "min_time": 0,
        "max_time": 10,
        "min_duration": 1,
        "max_candidates": 1000
    }
    req = urllib.request.Request(
        "http://127.0.0.1:8000/api/v1/investigate",
        data=json.dumps(payload).encode(),
        headers={"Content-Type": "application/json"}
    )
    with urllib.request.urlopen(req) as resp:
        data = json.loads(resp.read().decode())
        print(f"  Counterexample found: {data.get('counterexample_found')}")
        print(f"  Message: {data.get('message')}")
        print(f"  Candidates evaluated: {data.get('search_statistics', {}).get('candidates_evaluated')}")
        print(f"  Termination reason: {data.get('search_statistics', {}).get('termination_reason')}")
        assert data.get('counterexample_found') is False
        assert "No counterexample" in data.get('message')
        print("  -> PASS\n")

def test_fractional_knapsack_break_it():
    print("Testing 5. Fractional Knapsack Break It (no counterexample in bounded space)...")
    payload = {
        "problem": "knapsack_fractional",
        "strategy": "RANDOMIZED",
        "mode": "ANY",
        "min_items": 3,
        "max_items": 4,
        "min_weight": 1,
        "max_weight": 8,
        "min_value": 1,
        "max_value": 20,
        "min_capacity": 5,
        "max_capacity": 15,
        "max_candidates": 500,
        "random_seed": 42
    }
    req = urllib.request.Request(
        "http://127.0.0.1:8000/api/v1/investigate",
        data=json.dumps(payload).encode(),
        headers={"Content-Type": "application/json"}
    )
    with urllib.request.urlopen(req) as resp:
        data = json.loads(resp.read().decode())
        print(f"  Counterexample found: {data.get('counterexample_found')}")
        print(f"  Message: {data.get('message')}")
        print(f"  Candidates evaluated: {data.get('search_statistics', {}).get('candidates_evaluated')}")
        assert data.get('counterexample_found') is False
        assert "No counterexample" in data.get('message')
        print("  -> PASS\n")

def test_verify_coin_change():
    print("Testing 6. Verify Coin Change {1,3,4}, target 6...")
    payload = {
        "problem": "coin_change",
        "candidate": {
            "denominations": [1, 3, 4],
            "target": 6
        }
    }
    req = urllib.request.Request(
        "http://127.0.0.1:8000/api/v1/verify",
        data=json.dumps(payload).encode(),
        headers={"Content-Type": "application/json"}
    )
    with urllib.request.urlopen(req) as resp:
        data = json.loads(resp.read().decode())
        print(f"  Counterexample detected: {data.get('counterexample_found')}")
        print(f"  Greedy cost: {data.get('metrics', {}).get('greedy_cost')}")
        print(f"  Optimal cost: {data.get('metrics', {}).get('optimal_cost')}")
        print(f"  Divergence step: {data.get('divergence', {}).get('divergence_step')}")
        assert data.get('counterexample_found') is True
        assert data.get('metrics', {}).get('greedy_cost') == 3
        assert data.get('metrics', {}).get('optimal_cost') == 2
        assert data.get('divergence', {}).get('has_diverged') is True
        print("  -> PASS\n")

def test_frontend_server():
    print("Testing Vite frontend server on http://127.0.0.1:5173...")
    req = urllib.request.Request("http://127.0.0.1:5173")
    with urllib.request.urlopen(req) as resp:
        html = resp.read().decode()
        assert resp.status == 200
        assert "<title>" in html or "id=\"root\"" in html
        print(f"  Frontend responding with HTTP {resp.status} (length: {len(html)} bytes)")
        print("  -> PASS\n")

if __name__ == "__main__":
    print("=" * 60)
    print("GREEDY BREAKER PHASE 4C INTEGRATION SUITE")
    print("=" * 60)
    try:
        test_health()
        test_frontend_server()
        test_problems()
        test_coin_change_break_it()
        test_knapsack_01_break_it()
        test_activity_selection_break_it()
        test_fractional_knapsack_break_it()
        test_verify_coin_change()
        print("=" * 60)
        print("ALL 6 LIVE INTEGRATION REQUIREMENTS PASSED PERFECTLY!")
        print("=" * 60)
    except Exception as e:
        print(f"FAILURE: {e}", file=sys.stderr)
        sys.exit(1)
