package com.greedybreaker.unit;

import com.greedybreaker.model.Experiment;
import com.greedybreaker.model.enums.ExperimentStatus;
import com.greedybreaker.model.enums.ProblemType;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

class ExperimentStatusTransitionTest {

    @Test
    @DisplayName("Terminal states are correctly identified")
    void testTerminalStates() {
        assertFalse(ExperimentStatus.CREATED.isTerminal());
        assertFalse(ExperimentStatus.VALIDATING.isTerminal());
        assertFalse(ExperimentStatus.VALID.isTerminal());
        assertFalse(ExperimentStatus.QUEUED.isTerminal());
        assertFalse(ExperimentStatus.RUNNING.isTerminal());

        assertTrue(ExperimentStatus.INVALID.isTerminal());
        assertTrue(ExperimentStatus.COMPLETED.isTerminal());
        assertTrue(ExperimentStatus.NO_COUNTEREXAMPLE.isTerminal());
        assertTrue(ExperimentStatus.SEARCH_INCOMPLETE.isTerminal());
        assertTrue(ExperimentStatus.TIMED_OUT.isTerminal());
        assertTrue(ExperimentStatus.FAILED.isTerminal());
        assertTrue(ExperimentStatus.CANCELLED.isTerminal());
    }

    @Test
    @DisplayName("Experiment initial state is CREATED")
    void testInitialStateIsCreated() {
        Experiment exp = new Experiment("exp-1", "Test", ProblemType.COIN_CHANGE);
        assertEquals(ExperimentStatus.CREATED, exp.getStatus());
        assertNotNull(exp.getCreatedAt());
        assertNotNull(exp.getUpdatedAt());
    }
}
