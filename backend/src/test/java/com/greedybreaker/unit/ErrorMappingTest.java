package com.greedybreaker.unit;

import com.greedybreaker.dto.response.ErrorResponseDTO;
import com.greedybreaker.exception.EngineExecutionException;
import com.greedybreaker.exception.GlobalExceptionHandler;
import com.greedybreaker.exception.InvalidConfigurationException;
import com.greedybreaker.exception.ResourceNotFoundException;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;

import static org.junit.jupiter.api.Assertions.*;

class ErrorMappingTest {

    private final GlobalExceptionHandler handler = new GlobalExceptionHandler();

    @Test
    @DisplayName("ResourceNotFoundException maps to HTTP 404")
    void testResourceNotFoundMapsTo404() {
        ResponseEntity<ErrorResponseDTO> response = handler.handleNotFound(new ResourceNotFoundException("Not found"));
        assertEquals(HttpStatus.NOT_FOUND, response.getStatusCode());
        assertNotNull(response.getBody());
        assertEquals("Resource Not Found", response.getBody().getTitle());
        assertEquals(404, response.getBody().getStatus());
    }

    @Test
    @DisplayName("InvalidConfigurationException maps to HTTP 422")
    void testInvalidConfigMapsTo422() {
        ResponseEntity<ErrorResponseDTO> response = handler.handleInvalidConfig(new InvalidConfigurationException("Invalid parameters"));
        assertEquals(HttpStatus.UNPROCESSABLE_ENTITY, response.getStatusCode());
        assertNotNull(response.getBody());
        assertEquals(422, response.getBody().getStatus());
    }

    @Test
    @DisplayName("EngineExecutionException maps to HTTP 500")
    void testEngineExecutionExceptionMapsTo500() {
        ResponseEntity<ErrorResponseDTO> response = handler.handleEngineExecution(
                new EngineExecutionException("Engine crashed", 139, "Segmentation fault"));
        assertEquals(HttpStatus.INTERNAL_SERVER_ERROR, response.getStatusCode());
        assertNotNull(response.getBody());
        assertEquals(500, response.getBody().getStatus());
        assertTrue(response.getBody().getErrors().stream().anyMatch(e -> e.contains("Segmentation fault")));
    }
}
