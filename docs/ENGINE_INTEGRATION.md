# Greedy Breaker V2 — Engine Integration Specification

**Status**: APPROVED DESIGN  
**Version**: 2.0.0  

---

## 1. Subprocess Architecture & Security

The C++20 engine is invoked strictly as an external subprocess via `java.lang.ProcessBuilder`.

### 1.1 Process Execution Policy
1. **No Shell Wrapping**: Never invoke via `cmd.exe /c` or `bash -c`. Arguments are passed as an explicit array:
   ```java
   ProcessBuilder pb = new ProcessBuilder(binaryPath);
   ```
2. **Standard I/O Piping**: Communication is bidirectional over standard streams:
   - `stdin`: Request JSON string serialized with Jackson (`StandardCharsets.UTF_8`).
   - `stdout`: Structured JSON response.
   - `stderr`: Diagnostic logging and error stack traces.
3. **Environment Isolation**: Inherit minimal environment variables; do not pass database credentials or server secrets to the subprocess.
4. **Working Directory**: Set to a sandboxed directory within the engine binary path.

---

## 2. Java Subprocess Supervisor Implementation Pattern

```java
public EngineExecutionResult execute(SearchConfigDTO config, long timeoutMs) {
    String inputJson = objectMapper.writeValueAsString(config);
    ProcessBuilder pb = new ProcessBuilder(engineBinaryPath);
    pb.redirectErrorStream(false);

    long startTime = System.nanoTime();
    Process process = pb.start();

    // 1. Asynchronously drain stdout and stderr to prevent pipe deadlock
    CompletableFuture<String> stdoutFuture = CompletableFuture.supplyAsync(() -> readStream(process.getInputStream()));
    CompletableFuture<String> stderrFuture = CompletableFuture.supplyAsync(() -> readStream(process.getErrorStream()));

    // 2. Write input JSON to stdin and close to signal EOF
    try (OutputStream os = process.getOutputStream()) {
        os.write(inputJson.getBytes(StandardCharsets.UTF_8));
        os.flush();
    }

    // 3. Await process exit with timeout watchdog
    boolean finished = process.waitFor(timeoutMs, TimeUnit.MILLISECONDS);
    if (!finished) {
        process.destroyForcibly();
        long elapsedMs = (System.nanoTime() - startTime) / 1_000_000;
        return EngineExecutionResult.timedOut(elapsedMs);
    }

    int exitCode = process.exitValue();
    String stdout = stdoutFuture.join();
    String stderr = stderrFuture.join();
    long elapsedMs = (System.nanoTime() - startTime) / 1_000_000;

    if (exitCode != 0) {
        return EngineExecutionResult.failed(exitCode, stderr, elapsedMs);
    }

    EngineOutputDTO output = objectMapper.readValue(stdout, EngineOutputDTO.class);
    return EngineExecutionResult.success(output, elapsedMs);
}
```

---

## 3. Communication Schema

### 3.1 Input Schema (Piped to Stdin)
```json
{
  "experiment_id": "exp-12345",
  "problem_type": "knapsack_01",
  "strategy_id": "value_density_descending",
  "oracle_id": "dp_exact",
  "mode": "ANY",
  "strategy": "RANDOMIZED",
  "random_seed": 12345,
  "max_candidates": 500,
  "max_time_ms": 5000,
  "search_space": {
    "item_count": { "min": 3, "max": 4 },
    "weight_range": { "min": 1, "max": 20 },
    "value_range": { "min": 1, "max": 50 },
    "capacity_range": { "min": 10, "max": 30 }
  }
}
```

### 3.2 Output Schema (Emitted on Stdout)
```json
{
  "search_result": {
    "experiment_id": "exp-12345",
    "problem_type": "knapsack_01",
    "strategy_id": "value_density_descending",
    "oracle_id": "dp_exact",
    "mode": "ANY",
    "strategy": "RANDOMIZED",
    "has_counterexample": true,
    "instances_evaluated": 8,
    "counterexamples_found": 1,
    "failure_rate": 0.125,
    "worst_gap": 6.0,
    "average_gap": 6.0,
    "status_code": 0,
    "status_message": "",
    "coverage": "RANDOMIZED_SAMPLE",
    "coverage_ratio": 3.8e-13,
    "elapsed_time_ms": 1.088,
    "best_instance": {
      "capacity": 14.0,
      "items": [
        { "id": 1, "value": 44.0, "weight": 4.0 },
        { "id": 2, "value": 10.0, "weight": 10.0 },
        { "id": 3, "value": 47.0, "weight": 9.0 },
        { "id": 4, "value": 41.0, "weight": 4.0 }
      ]
    },
    "best_greedy": {
      "feasible": true,
      "objective_value": 85.0,
      "items_selected": [1, 4],
      "solution_description": "Total value: 85, used capacity: 8/14",
      "trace": { "total_steps": 4, "steps": [...] }
    },
    "best_oracle": {
      "confidence": "EXACT",
      "optimal_value": 91.0,
      "items_selected": [1, 3],
      "oracle_method": "Dynamic Programming 2D Table O(N*W)",
      "solution_description": "Exact optimum value: 91 using 2 items"
    },
    "best_verification": {
      "status": "OPTIMALITY_FAILURE",
      "is_counterexample": true,
      "greedy_value": 85.0,
      "optimal_value": 91.0,
      "absolute_gap": 6.0,
      "relative_gap": 1.0705,
      "failure_percent": 6.593
    }
  },
  "minimization_result": {
    "original_instance": { "capacity": 14.0, "items": [...] },
    "minimal_instance": {
      "capacity": 13.0,
      "items": [
        { "id": 1, "value": 44.0, "weight": 4.0 },
        { "id": 3, "value": 47.0, "weight": 9.0 },
        { "id": 4, "value": 41.0, "weight": 4.0 }
      ]
    },
    "reduction_steps": 2,
    "original_size_metric": 4014.0,
    "minimal_size_metric": 3013.0,
    "reduction_percent": 24.937,
    "ordering_criteria": "1. Minimal element count; 2. Minimal max parameter; 3. Minimal parameter sum; 4. Lexicographical",
    "minimality_status": "Smallest verified counterexample under the configured ordering",
    "final_verification": { ... }
  },
  "forensic_report": {
    "problem_type": "knapsack_01",
    "divergence_step_index": 1,
    "critical_greedy_action": "Selected Item 4 (v=41.000000, w=4.000000)",
    "optimal_alternative_action": "Optimal solution excludes item 4 to preserve capacity for higher total value set.",
    "causal_consequence": "Inclusion of item 4 stranded capacity, precluding inclusion of optimal items.",
    "evidence_confidence": "HIGH_CONFIDENCE_EVIDENCE",
    "minimality_status": "Smallest verified counterexample under the configured ordering"
  }
}
```
