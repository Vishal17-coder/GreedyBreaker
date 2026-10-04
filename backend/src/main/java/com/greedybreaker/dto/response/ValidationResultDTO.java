package com.greedybreaker.dto.response;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.greedybreaker.model.enums.ExperimentStatus;
import java.util.ArrayList;
import java.util.List;

public class ValidationResultDTO {
    @JsonProperty("experiment_id")
    private String experimentId;
    @JsonProperty("is_valid")
    private boolean isValid;
    private ExperimentStatus status;
    @JsonProperty("estimated_space_size")
    private long estimatedSpaceSize;
    private List<String> warnings = new ArrayList<>();
    @JsonProperty("validation_errors")
    private List<ValidationErrorItem> validationErrors = new ArrayList<>();

    public ValidationResultDTO() {}

    public static class ValidationErrorItem {
        private String field;
        @JsonProperty("rejected_value")
        private String rejectedValue;
        private String message;

        public ValidationErrorItem() {}
        public ValidationErrorItem(String field, String rejectedValue, String message) {
            this.field = field;
            this.rejectedValue = rejectedValue;
            this.message = message;
        }

        public String getField() { return field; }
        public void setField(String field) { this.field = field; }
        public String getRejectedValue() { return rejectedValue; }
        public void setRejectedValue(String rejectedValue) { this.rejectedValue = rejectedValue; }
        public String getMessage() { return message; }
        public void setMessage(String message) { this.message = message; }
    }

    public String getExperimentId() { return experimentId; }
    public void setExperimentId(String experimentId) { this.experimentId = experimentId; }
    public boolean isValid() { return isValid; }
    public void setValid(boolean valid) { isValid = valid; }
    public ExperimentStatus getStatus() { return status; }
    public void setStatus(ExperimentStatus status) { this.status = status; }
    public long getEstimatedSpaceSize() { return estimatedSpaceSize; }
    public void setEstimatedSpaceSize(long estimatedSpaceSize) { this.estimatedSpaceSize = estimatedSpaceSize; }
    public List<String> getWarnings() { return warnings; }
    public void setWarnings(List<String> warnings) { this.warnings = warnings; }
    public List<ValidationErrorItem> getValidationErrors() { return validationErrors; }
    public void setValidationErrors(List<ValidationErrorItem> validationErrors) { this.validationErrors = validationErrors; }
}
