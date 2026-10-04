package com.greedybreaker.exception;

import com.greedybreaker.dto.response.ValidationResultDTO;
import java.util.Collections;
import java.util.List;

public class InvalidConfigurationException extends RuntimeException {
    private final List<ValidationResultDTO.ValidationErrorItem> errors;

    public InvalidConfigurationException(String message) {
        super(message);
        this.errors = Collections.emptyList();
    }

    public InvalidConfigurationException(String message, List<ValidationResultDTO.ValidationErrorItem> errors) {
        super(message);
        this.errors = errors;
    }

    public List<ValidationResultDTO.ValidationErrorItem> getErrors() {
        return errors;
    }
}
