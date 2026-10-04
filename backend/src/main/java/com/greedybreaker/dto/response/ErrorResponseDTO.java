package com.greedybreaker.dto.response;

import java.time.Instant;
import java.util.ArrayList;
import java.util.List;

public class ErrorResponseDTO {
    private String title;
    private int status;
    private String detail;
    private Instant timestamp;
    private List<String> errors = new ArrayList<>();

    public ErrorResponseDTO() {
        this.timestamp = Instant.now();
    }

    public ErrorResponseDTO(String title, int status, String detail) {
        this();
        this.title = title;
        this.status = status;
        this.detail = detail;
    }

    public String getTitle() { return title; }
    public void setTitle(String title) { this.title = title; }
    public int getStatus() { return status; }
    public void setStatus(int status) { this.status = status; }
    public String getDetail() { return detail; }
    public void setDetail(String detail) { this.detail = detail; }
    public Instant getTimestamp() { return timestamp; }
    public void setTimestamp(Instant timestamp) { this.timestamp = timestamp; }
    public List<String> getErrors() { return errors; }
    public void setErrors(List<String> errors) { this.errors = errors; }
}
