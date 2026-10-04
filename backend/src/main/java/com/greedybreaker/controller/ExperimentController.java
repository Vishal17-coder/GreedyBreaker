package com.greedybreaker.controller;

import com.greedybreaker.dto.request.CreateExperimentRequest;
import com.greedybreaker.dto.response.*;
import com.greedybreaker.model.enums.ExperimentStatus;
import com.greedybreaker.model.enums.ProblemType;
import com.greedybreaker.service.ExperimentService;
import jakarta.validation.Valid;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Sort;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/api/v1/experiments")
public class ExperimentController {

    private final ExperimentService experimentService;

    public ExperimentController(ExperimentService experimentService) {
        this.experimentService = experimentService;
    }

    @PostMapping
    public ResponseEntity<ExperimentSummaryDTO> createExperiment(@Valid @RequestBody CreateExperimentRequest request) {
        ExperimentSummaryDTO created = experimentService.createExperiment(request);
        return ResponseEntity.status(HttpStatus.CREATED).body(created);
    }

    @PostMapping("/{id}/validate")
    public ResponseEntity<ValidationResultDTO> validateExperiment(@PathVariable String id) {
        ValidationResultDTO result = experimentService.validateExperiment(id);
        if (!result.isValid()) {
            return ResponseEntity.status(HttpStatus.UNPROCESSABLE_ENTITY).body(result);
        }
        return ResponseEntity.ok(result);
    }

    @PostMapping("/{id}/run")
    public ResponseEntity<ExecutionResultDTO> runExperiment(@PathVariable String id) {
        ExecutionResultDTO result = experimentService.runExperiment(id);
        return ResponseEntity.ok(result);
    }

    @GetMapping("/{id}")
    public ResponseEntity<ExperimentDetailDTO> getExperiment(@PathVariable String id) {
        return ResponseEntity.ok(experimentService.getExperiment(id));
    }

    @GetMapping("/{id}/status")
    public ResponseEntity<ExecutionStatusDTO> getStatus(@PathVariable String id) {
        return ResponseEntity.ok(experimentService.getExperimentStatus(id));
    }

    @GetMapping("/{id}/result")
    public ResponseEntity<ExecutionResultDTO> getResult(@PathVariable String id) {
        ExperimentDetailDTO detail = experimentService.getExperiment(id);
        ExecutionResultDTO result = new ExecutionResultDTO();
        result.setExperimentId(detail.getId());
        result.setStatus(detail.getStatus());
        return ResponseEntity.ok(result);
    }

    @GetMapping("/{id}/counterexamples")
    public ResponseEntity<CounterexampleDTO> getCounterexamples(@PathVariable String id) {
        return ResponseEntity.ok(experimentService.getCounterexample(id));
    }

    @GetMapping("/{id}/forensics")
    public ResponseEntity<ForensicReportDTO> getForensics(@PathVariable String id) {
        return ResponseEntity.ok(experimentService.getForensicReport(id));
    }

    @PostMapping("/{id}/reproduce")
    public ResponseEntity<ReproductionResultDTO> reproduceExperiment(@PathVariable String id) {
        ReproductionResultDTO result = experimentService.reproduceExperiment(id);
        return ResponseEntity.status(HttpStatus.CREATED).body(result);
    }

    @GetMapping
    public ResponseEntity<Page<ExperimentSummaryDTO>> listExperiments(
            @RequestParam(required = false) ProblemType problemType,
            @RequestParam(required = false) ExperimentStatus status,
            @RequestParam(defaultValue = "0") int page,
            @RequestParam(defaultValue = "20") int size,
            @RequestParam(defaultValue = "createdAt") String sortBy,
            @RequestParam(defaultValue = "desc") String direction) {

        Sort sort = direction.equalsIgnoreCase("desc") ? Sort.by(sortBy).descending() : Sort.by(sortBy).ascending();
        PageRequest pageRequest = PageRequest.of(page, size, sort);
        return ResponseEntity.ok(experimentService.listExperiments(problemType, status, pageRequest));
    }
}
