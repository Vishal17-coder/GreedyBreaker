package com.greedybreaker.unit;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.greedybreaker.config.EngineProperties;
import com.greedybreaker.dto.request.CreateExperimentRequest;
import com.greedybreaker.dto.response.ValidationResultDTO;
import com.greedybreaker.model.enums.ExperimentStatus;
import com.greedybreaker.model.enums.ProblemType;
import com.greedybreaker.model.enums.SearchStrategy;
import com.greedybreaker.validation.ExperimentValidator;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

class ExperimentValidatorTest {

    private ExperimentValidator validator;
    private ObjectMapper objectMapper;

    @BeforeEach
    void setUp() {
        EngineProperties props = new EngineProperties();
        props.setMaxCandidatesLimit(100000L);
        props.setMaxDurationLimitMs(30000L);
        validator = new ExperimentValidator(props);
        objectMapper = new ObjectMapper();
    }

    @Test
    @DisplayName("Valid Coin Change configuration passes validation")
    void testValidCoinChangeConfiguration() throws Exception {
        CreateExperimentRequest req = new CreateExperimentRequest();
        req.setName("Coin Change Valid");
        req.setProblemType(ProblemType.COIN_CHANGE);
        req.setMaxCandidates(500L);
        req.setMaxTimeMs(2000L);
        req.setStrategy(SearchStrategy.EXHAUSTIVE);
        req.setSearchSpace(objectMapper.readTree("""
            {
                "denomination_count": { "min": 3, "max": 3 },
                "denomination_range": { "min": 1, "max": 10 },
                "target_range": { "min": 1, "max": 20 }
            }
        """));

        ValidationResultDTO res = validator.validate(req, "exp-1");
        assertTrue(res.isValid());
        assertEquals(ExperimentStatus.VALID, res.getStatus());
        assertTrue(res.getValidationErrors().isEmpty());
        assertTrue(res.getEstimatedSpaceSize() > 0);
    }

    @Test
    @DisplayName("Min exceeding Max in range produces structured validation error")
    void testMinExceedsMaxProducesValidationError() throws Exception {
        CreateExperimentRequest req = new CreateExperimentRequest();
        req.setName("Invalid Range");
        req.setProblemType(ProblemType.COIN_CHANGE);
        req.setSearchSpace(objectMapper.readTree("""
            {
                "denomination_count": { "min": 3, "max": 3 },
                "denomination_range": { "min": 1, "max": 10 },
                "target_range": { "min": 25, "max": 10 }
            }
        """));

        ValidationResultDTO res = validator.validate(req, "exp-2");
        assertFalse(res.isValid());
        assertEquals(ExperimentStatus.INVALID, res.getStatus());
        assertFalse(res.getValidationErrors().isEmpty());
        assertTrue(res.getValidationErrors().stream().anyMatch(e -> e.getField().equals("target_range")));
    }

    @Test
    @DisplayName("Exceeding candidate ceiling is rejected")
    void testExceedingCandidateCeilingIsRejected() throws Exception {
        CreateExperimentRequest req = new CreateExperimentRequest();
        req.setName("Too Many Candidates");
        req.setProblemType(ProblemType.KNAPSACK_01);
        req.setMaxCandidates(200000L); // exceeds 100,000 limit
        req.setSearchSpace(objectMapper.readTree("""
            {
                "item_count": { "min": 3, "max": 4 },
                "weight_range": { "min": 1, "max": 20 },
                "value_range": { "min": 1, "max": 50 },
                "capacity_range": { "min": 10, "max": 30 }
            }
        """));

        ValidationResultDTO res = validator.validate(req, "exp-3");
        assertFalse(res.isValid());
        assertTrue(res.getValidationErrors().stream().anyMatch(e -> e.getField().equals("max_candidates")));
    }

    @Test
    @DisplayName("Missing search_space payload is rejected")
    void testMissingSearchSpaceIsRejected() {
        CreateExperimentRequest req = new CreateExperimentRequest();
        req.setName("Missing Space");
        req.setProblemType(ProblemType.ACTIVITY_SELECTION);
        req.setSearchSpace(null);

        ValidationResultDTO res = validator.validate(req, "exp-4");
        assertFalse(res.isValid());
        assertTrue(res.getValidationErrors().stream().anyMatch(e -> e.getField().equals("search_space")));
    }
}
