package com.greedybreaker.unit;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.greedybreaker.dto.engine.EngineOutputDTO;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

class ResultMappingTest {

    private final ObjectMapper mapper = new ObjectMapper();

    @Test
    @DisplayName("Engine JSON output deserializes correctly into EngineOutputDTO")
    void testEngineOutputDeserialization() throws Exception {
        String json = """
        {
          "search_result": {
            "instances_evaluated": 15,
            "counterexamples_found": 1,
            "has_counterexample": true,
            "status_code": 0,
            "worst_gap": 6.0,
            "average_gap": 6.0,
            "best_greedy": {
              "feasible": true,
              "objective_value": 85.0,
              "solution_description": "Total value: 85"
            },
            "best_oracle": {
              "confidence": "EXACT",
              "optimal_value": 91.0,
              "solution_description": "Exact optimum: 91"
            },
            "best_verification": {
              "status": "OPTIMALITY_FAILURE",
              "is_counterexample": true,
              "greedy_value": 85.0,
              "optimal_value": 91.0,
              "absolute_gap": 6.0
            }
          },
          "minimization_result": {
            "reduction_steps": 2,
            "reduction_percent": 24.9,
            "minimality_status": "Smallest verified counterexample under the configured ordering"
          },
          "forensic_report": {
            "problem_type": "knapsack_01",
            "divergence_step_index": 1,
            "critical_greedy_action": "Selected Item 4",
            "evidence_confidence": "HIGH_CONFIDENCE_EVIDENCE"
          }
        }
        """;

        EngineOutputDTO output = mapper.readValue(json, EngineOutputDTO.class);
        assertNotNull(output);
        assertNotNull(output.getSearchResult());
        assertTrue(output.getSearchResult().isHasCounterexample());
        assertEquals(15, output.getSearchResult().getInstancesEvaluated());
        assertEquals(85.0, output.getSearchResult().getBestGreedy().getObjectiveValue());
        assertEquals(91.0, output.getSearchResult().getBestOracle().getOptimalValue());
        assertEquals(6.0, output.getSearchResult().getBestVerification().getAbsoluteGap());

        assertNotNull(output.getMinimizationResult());
        assertEquals(2, output.getMinimizationResult().getReductionSteps());
        assertEquals("Smallest verified counterexample under the configured ordering",
                output.getMinimizationResult().getMinimalityStatus());

        assertNotNull(output.getForensicReport());
        assertEquals(1, output.getForensicReport().getDivergenceStepIndex());
        assertEquals("Selected Item 4", output.getForensicReport().getCriticalGreedyAction());
    }
}
