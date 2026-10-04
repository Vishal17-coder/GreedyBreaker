package com.greedybreaker.controller;

import com.greedybreaker.dto.response.ProblemMetadataDTO;
import com.greedybreaker.exception.ResourceNotFoundException;
import com.greedybreaker.service.ProblemRegistryService;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/v1/problems")
public class ProblemController {

    private final ProblemRegistryService registryService;

    public ProblemController(ProblemRegistryService registryService) {
        this.registryService = registryService;
    }

    @GetMapping
    public ResponseEntity<List<ProblemMetadataDTO>> listProblems() {
        return ResponseEntity.ok(registryService.listAllProblems());
    }

    @GetMapping("/{problemId}")
    public ResponseEntity<ProblemMetadataDTO> getProblem(@PathVariable String problemId) {
        return registryService.findProblemById(problemId)
                .map(ResponseEntity::ok)
                .orElseThrow(() -> new ResourceNotFoundException("Problem family not found: " + problemId));
    }
}
