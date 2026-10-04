package com.greedybreaker.integration;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.greedybreaker.dto.request.CreateExperimentRequest;
import com.greedybreaker.dto.response.*;
import com.greedybreaker.model.enums.ExperimentStatus;
import com.greedybreaker.model.enums.ProblemType;
import com.greedybreaker.model.enums.SearchMode;
import com.greedybreaker.model.enums.SearchStrategy;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.MethodOrderer;
import org.junit.jupiter.api.Order;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestMethodOrder;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.web.client.TestRestTemplate;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;

import static org.junit.jupiter.api.Assertions.*;

@SpringBootTest(webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
@TestMethodOrder(MethodOrderer.OrderAnnotation.class)
class ExperimentEndToEndIntegrationTest {

    @Autowired
    private TestRestTemplate restTemplate;

    @Autowired
    private ObjectMapper objectMapper;

    @Test
    @Order(1)
    @DisplayName("E2E: Problem Registry endpoints return supported problem families")
    void testProblemRegistry() {
        ResponseEntity<ProblemMetadataDTO[]> response =
                restTemplate.getForEntity("/api/v1/problems", ProblemMetadataDTO[].class);

        assertEquals(HttpStatus.OK, response.getStatusCode());
        assertNotNull(response.getBody());
        assertTrue(response.getBody().length >= 5);

        // Fetch single problem
        ResponseEntity<ProblemMetadataDTO> single =
                restTemplate.getForEntity("/api/v1/problems/coin_change", ProblemMetadataDTO.class);
        assertEquals(HttpStatus.OK, single.getStatusCode());
        assertNotNull(single.getBody());
        assertEquals("coin_change", single.getBody().getId());
        assertEquals("MINIMIZE", single.getBody().getObjective());
    }

    @Test
    @Order(2)
    @DisplayName("E2E: Coin Change canonical counterexample discovery, minimization, forensics, and reproduction")
    void testCoinChangeDiscoveryPipeline() throws Exception {
        // 1. Create Experiment
        CreateExperimentRequest req = new CreateExperimentRequest();
        req.setName("Coin Change Integration Test");
        req.setProblemType(ProblemType.COIN_CHANGE);
        req.setStrategyId("largest_denomination_first");
        req.setOracleId("dp_exact");
        req.setMode(SearchMode.ANY);
        req.setStrategy(SearchStrategy.EXHAUSTIVE);
        req.setMaxCandidates(100L);
        req.setMaxTimeMs(5000L);
        req.setSearchSpace(objectMapper.readTree("""
            {
                "denomination_count": { "min": 3, "max": 3 },
                "denomination_range": { "min": 1, "max": 4 },
                "target_range": { "min": 6, "max": 6 }
            }
        """));

        ResponseEntity<ExperimentSummaryDTO> createRes =
                restTemplate.postForEntity("/api/v1/experiments", req, ExperimentSummaryDTO.class);
        assertEquals(HttpStatus.CREATED, createRes.getStatusCode());
        assertNotNull(createRes.getBody());
        String expId = createRes.getBody().getId();

        // 2. Validate Experiment
        ResponseEntity<ValidationResultDTO> valRes =
                restTemplate.postForEntity("/api/v1/experiments/" + expId + "/validate", null, ValidationResultDTO.class);
        assertEquals(HttpStatus.OK, valRes.getStatusCode());
        assertTrue(valRes.getBody().isValid());
        assertEquals(ExperimentStatus.VALID, valRes.getBody().getStatus());

        // 3. Execute Experiment Run (Calls C++ Engine)
        ResponseEntity<ExecutionResultDTO> runRes =
                restTemplate.postForEntity("/api/v1/experiments/" + expId + "/run", null, ExecutionResultDTO.class);
        assertEquals(HttpStatus.OK, runRes.getStatusCode());
        assertNotNull(runRes.getBody());
        assertEquals(ExperimentStatus.COMPLETED, runRes.getBody().getStatus());
        assertTrue(runRes.getBody().getSearchResult().isHasCounterexample());
        assertEquals(3.0, runRes.getBody().getSearchResult().getBestGreedy().getObjectiveValue());
        assertEquals(2.0, runRes.getBody().getSearchResult().getBestOracle().getOptimalValue());

        // 4. Retrieve Status
        ResponseEntity<ExecutionStatusDTO> statusRes =
                restTemplate.getForEntity("/api/v1/experiments/" + expId + "/status", ExecutionStatusDTO.class);
        assertEquals(HttpStatus.OK, statusRes.getStatusCode());
        assertEquals(ExperimentStatus.COMPLETED, statusRes.getBody().getStatus());
        assertEquals(1, statusRes.getBody().getCounterexamplesFound());

        // 5. Retrieve Counterexample Details
        ResponseEntity<CounterexampleDTO> ceRes =
                restTemplate.getForEntity("/api/v1/experiments/" + expId + "/counterexamples", CounterexampleDTO.class);
        assertEquals(HttpStatus.OK, ceRes.getStatusCode());
        assertTrue(ceRes.getBody().isHasCounterexample());
        assertEquals(3.0, ceRes.getBody().getFinalVerification().getGreedyValue());
        assertEquals(2.0, ceRes.getBody().getFinalVerification().getOptimalValue());
        assertEquals("Smallest verified counterexample under the configured ordering",
                ceRes.getBody().getMinimalityStatus());

        // 6. Retrieve Forensic Report
        ResponseEntity<ForensicReportDTO> frRes =
                restTemplate.getForEntity("/api/v1/experiments/" + expId + "/forensics", ForensicReportDTO.class);
        assertEquals(HttpStatus.OK, frRes.getStatusCode());
        assertEquals("coin_change", frRes.getBody().getProblemType());
        assertEquals(0, frRes.getBody().getDivergenceStepIndex());
        assertEquals("HIGH_CONFIDENCE_EVIDENCE", frRes.getBody().getEvidenceConfidence());
        assertNotNull(frRes.getBody().getGreedyTrace());

        // 7. Reproduce Experiment
        ResponseEntity<ReproductionResultDTO> reproRes =
                restTemplate.postForEntity("/api/v1/experiments/" + expId + "/reproduce", null, ReproductionResultDTO.class);
        assertEquals(HttpStatus.CREATED, reproRes.getStatusCode());
        assertNotNull(reproRes.getBody());
        assertTrue(reproRes.getBody().isDeterministicMatch());
        assertNotEquals(expId, reproRes.getBody().getReproductionExperimentId());
        assertEquals(expId, reproRes.getBody().getOriginalExperimentId());
    }

    @Test
    @Order(3)
    @DisplayName("E2E: 0/1 Knapsack live randomized discovery and minimization")
    void testKnapsack01RandomizedDiscovery() throws Exception {
        CreateExperimentRequest req = new CreateExperimentRequest();
        req.setName("Knapsack 0/1 Randomized Search");
        req.setProblemType(ProblemType.KNAPSACK_01);
        req.setStrategyId("value_density_descending");
        req.setOracleId("dp_exact");
        req.setMode(SearchMode.ANY);
        req.setStrategy(SearchStrategy.RANDOMIZED);
        req.setRandomSeed(12345L);
        req.setMaxCandidates(500L);
        req.setMaxTimeMs(5000L);
        req.setSearchSpace(objectMapper.readTree("""
            {
                "item_count": { "min": 3, "max": 4 },
                "weight_range": { "min": 1, "max": 20 },
                "value_range": { "min": 1, "max": 50 },
                "capacity_range": { "min": 10, "max": 30 }
            }
        """));

        // Create & Run directly
        ResponseEntity<ExperimentSummaryDTO> createRes =
                restTemplate.postForEntity("/api/v1/experiments", req, ExperimentSummaryDTO.class);
        String expId = createRes.getBody().getId();

        ResponseEntity<ExecutionResultDTO> runRes =
                restTemplate.postForEntity("/api/v1/experiments/" + expId + "/run", null, ExecutionResultDTO.class);
        assertEquals(HttpStatus.OK, runRes.getStatusCode());
        assertEquals(ExperimentStatus.COMPLETED, runRes.getBody().getStatus());
        assertTrue(runRes.getBody().getSearchResult().isHasCounterexample());

        // Verify Counterexample and Minimization
        ResponseEntity<CounterexampleDTO> ceRes =
                restTemplate.getForEntity("/api/v1/experiments/" + expId + "/counterexamples", CounterexampleDTO.class);
        assertEquals(HttpStatus.OK, ceRes.getStatusCode());
        assertTrue(ceRes.getBody().getReductionSteps() >= 1);
        assertTrue(ceRes.getBody().getReductionPercent() > 0.0);
        assertEquals("Smallest verified counterexample under the configured ordering",
                ceRes.getBody().getMinimalityStatus());

        // Verify Forensics
        ResponseEntity<ForensicReportDTO> frRes =
                restTemplate.getForEntity("/api/v1/experiments/" + expId + "/forensics", ForensicReportDTO.class);
        assertEquals(HttpStatus.OK, frRes.getStatusCode());
        assertTrue(frRes.getBody().getDivergenceStepIndex() >= 0);
        assertEquals("HIGH_CONFIDENCE_EVIDENCE", frRes.getBody().getEvidenceConfidence());
    }

    @Test
    @Order(4)
    @DisplayName("E2E: Activity Selection Earliest Finish Time yields NO_COUNTEREXAMPLE")
    void testActivitySelectionNoCounterexample() throws Exception {
        CreateExperimentRequest req = new CreateExperimentRequest();
        req.setName("Activity Selection Optimal Invariance");
        req.setProblemType(ProblemType.ACTIVITY_SELECTION);
        req.setStrategyId("earliest_finish_time");
        req.setOracleId("dp_exact");
        req.setMode(SearchMode.PROFILE);
        req.setStrategy(SearchStrategy.RANDOMIZED);
        req.setRandomSeed(777L);
        req.setMaxCandidates(100L);
        req.setMaxTimeMs(5000L);
        req.setSearchSpace(objectMapper.readTree("""
            {
                "activity_count": { "min": 3, "max": 5 },
                "time_range": { "min": 0, "max": 20 },
                "duration_range": { "min": 1, "max": 10 }
            }
        """));

        ResponseEntity<ExperimentSummaryDTO> createRes =
                restTemplate.postForEntity("/api/v1/experiments", req, ExperimentSummaryDTO.class);
        String expId = createRes.getBody().getId();

        ResponseEntity<ExecutionResultDTO> runRes =
                restTemplate.postForEntity("/api/v1/experiments/" + expId + "/run", null, ExecutionResultDTO.class);
        assertEquals(HttpStatus.OK, runRes.getStatusCode());
        assertEquals(ExperimentStatus.NO_COUNTEREXAMPLE, runRes.getBody().getStatus());
        assertFalse(runRes.getBody().getSearchResult().isHasCounterexample());
        assertEquals(0, runRes.getBody().getSearchResult().getCounterexamplesFound());
    }

    @Test
    @Order(5)
    @DisplayName("E2E: Invalid range configuration is rejected with HTTP 422")
    void testInvalidConfigurationRejection() throws Exception {
        CreateExperimentRequest req = new CreateExperimentRequest();
        req.setName("Invalid Range Test");
        req.setProblemType(ProblemType.COIN_CHANGE);
        req.setSearchSpace(objectMapper.readTree("""
            {
                "denomination_count": { "min": 3, "max": 3 },
                "denomination_range": { "min": 10, "max": 2 },
                "target_range": { "min": 1, "max": 10 }
            }
        """));

        ResponseEntity<ExperimentSummaryDTO> createRes =
                restTemplate.postForEntity("/api/v1/experiments", req, ExperimentSummaryDTO.class);
        String expId = createRes.getBody().getId();

        ResponseEntity<ValidationResultDTO> valRes =
                restTemplate.postForEntity("/api/v1/experiments/" + expId + "/validate", null, ValidationResultDTO.class);
        assertEquals(HttpStatus.UNPROCESSABLE_ENTITY, valRes.getStatusCode());
        assertFalse(valRes.getBody().isValid());
        assertEquals(ExperimentStatus.INVALID, valRes.getBody().getStatus());
    }

    @Test
    @Order(6)
    @DisplayName("E2E: Non-existent experiment returns HTTP 404")
    void testNonExistentExperimentReturns404() {
        ResponseEntity<ErrorResponseDTO> response =
                restTemplate.getForEntity("/api/v1/experiments/exp-non-existent-xyz", ErrorResponseDTO.class);
        assertEquals(HttpStatus.NOT_FOUND, response.getStatusCode());
    }
}
