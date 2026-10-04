package com.greedybreaker.exception;

import com.greedybreaker.dto.response.ErrorResponseDTO;
import com.greedybreaker.dto.response.ValidationResultDTO;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.validation.FieldError;
import org.springframework.web.bind.MethodArgumentNotValidException;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.RestControllerAdvice;

@RestControllerAdvice
public class GlobalExceptionHandler {

    @ExceptionHandler(ResourceNotFoundException.class)
    public ResponseEntity<ErrorResponseDTO> handleNotFound(ResourceNotFoundException ex) {
        ErrorResponseDTO err = new ErrorResponseDTO("Resource Not Found", HttpStatus.NOT_FOUND.value(), ex.getMessage());
        return ResponseEntity.status(HttpStatus.NOT_FOUND).body(err);
    }

    @ExceptionHandler(InvalidConfigurationException.class)
    public ResponseEntity<ErrorResponseDTO> handleInvalidConfig(InvalidConfigurationException ex) {
        ErrorResponseDTO err = new ErrorResponseDTO("Unprocessable Configuration", HttpStatus.UNPROCESSABLE_ENTITY.value(), ex.getMessage());
        if (ex.getErrors() != null) {
            for (ValidationResultDTO.ValidationErrorItem item : ex.getErrors()) {
                err.getErrors().add(item.getField() + ": " + item.getMessage() + " (rejected: " + item.getRejectedValue() + ")");
            }
        }
        return ResponseEntity.status(HttpStatus.UNPROCESSABLE_ENTITY).body(err);
    }

    @ExceptionHandler(MethodArgumentNotValidException.class)
    public ResponseEntity<ErrorResponseDTO> handleValidationErrors(MethodArgumentNotValidException ex) {
        ErrorResponseDTO err = new ErrorResponseDTO("Validation Failed", HttpStatus.BAD_REQUEST.value(), "Request payload failed schema validation.");
        for (FieldError fe : ex.getBindingResult().getFieldErrors()) {
            err.getErrors().add(fe.getField() + ": " + fe.getDefaultMessage());
        }
        return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(err);
    }

    @ExceptionHandler(IllegalArgumentException.class)
    public ResponseEntity<ErrorResponseDTO> handleIllegalArgument(IllegalArgumentException ex) {
        ErrorResponseDTO err = new ErrorResponseDTO("Bad Request", HttpStatus.BAD_REQUEST.value(), ex.getMessage());
        return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(err);
    }

    @ExceptionHandler(EngineExecutionException.class)
    public ResponseEntity<ErrorResponseDTO> handleEngineExecution(EngineExecutionException ex) {
        ErrorResponseDTO err = new ErrorResponseDTO("Engine Execution Failure", HttpStatus.INTERNAL_SERVER_ERROR.value(), ex.getMessage());
        if (ex.getStderr() != null && !ex.getStderr().isBlank()) {
            err.getErrors().add("Engine stderr: " + ex.getStderr().trim());
        }
        return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body(err);
    }

    @ExceptionHandler(Exception.class)
    public ResponseEntity<ErrorResponseDTO> handleGeneric(Exception ex) {
        ErrorResponseDTO err = new ErrorResponseDTO("Internal Server Error", HttpStatus.INTERNAL_SERVER_ERROR.value(), ex.getMessage());
        return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body(err);
    }
}
