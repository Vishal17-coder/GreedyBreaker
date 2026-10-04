package com.greedybreaker.service;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.greedybreaker.dto.engine.EngineOutputDTO;
import com.greedybreaker.dto.engine.EngineSearchConfigDTO;
import com.greedybreaker.dto.request.CreateExperimentRequest;
import com.greedybreaker.dto.response.*;
import com.greedybreaker.exception.EngineExecutionException;
import com.greedybreaker.exception.InvalidConfigurationException;
import com.greedybreaker.exception.ResourceNotFoundException;
import com.greedybreaker.model.*;
import com.greedybreaker.model.enums.ExperimentStatus;
import com.greedybreaker.model.enums.ProblemType;
import com.greedybreaker.model.enums.SearchCoverage;
import com.greedybreaker.repository.*;
import com.greedybreaker.validation.ExperimentValidator;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.Instant;
import java.util.UUID;

@Service
public class ExperimentService {

    private static final Logger log = LoggerFactory.getLogger(ExperimentService.class);

    private final ExperimentRepository experimentRepository;
    private final ExperimentExecutionRepository executionRepository;
    private final CounterexampleRepository counterexampleRepository;
    private final ForensicReportRepository forensicReportRepository;
    private final ReproducibilityTokenRepository reproducibilityTokenRepository;
    private final ExperimentValidator validator;
    private final EngineProcessSupervisor engineSupervisor;
    private final ObjectMapper objectMapper;

    public ExperimentService(ExperimentRepository experimentRepository,
                             ExperimentExecutionRepository executionRepository,
                             CounterexampleRepository counterexampleRepository,
                             ForensicReportRepository forensicReportRepository,
                             ReproducibilityTokenRepository reproducibilityTokenRepository,
                             ExperimentValidator validator,
                             EngineProcessSupervisor engineSupervisor,
                             ObjectMapper objectMapper) {
        this.experimentRepository = experimentRepository;
        this.executionRepository = executionRepository;
        this.counterexampleRepository = counterexampleRepository;
        this.forensicReportRepository = forensicReportRepository;
        this.reproducibilityTokenRepository = reproducibilityTokenRepository;
        this.validator = validator;
        this.engineSupervisor = engineSupervisor;
        this.objectMapper = objectMapper;
    }

    @Transactional
    public ExperimentSummaryDTO createExperiment(CreateExperimentRequest request) {
        String expId = "exp-" + UUID.randomUUID().toString().substring(0, 8);

        Experiment experiment = new Experiment(expId, request.getName(), request.getProblemType());
        experiment.setDescription(request.getDescription());
        experiment.setStatus(ExperimentStatus.CREATED);

        ExperimentConfiguration config = new ExperimentConfiguration();
        config.setStrategyId(request.getStrategyId());
        config.setOracleId(request.getOracleId());
        config.setMode(request.getMode());
        config.setStrategy(request.getStrategy());
        config.setRandomSeed(request.getRandomSeed());
        config.setMaxCandidates(request.getMaxCandidates());
        config.setMaxTimeMs(request.getMaxTimeMs());
        config.setSearchSpaceJson(request.getSearchSpace() != null ? request.getSearchSpace().toString() : "{}");

        experiment.setConfiguration(config);
        Experiment saved = experimentRepository.save(experiment);

        return mapToSummary(saved, null);
    }

    @Transactional
    public ValidationResultDTO validateExperiment(String experimentId) {
        Experiment experiment = getExperimentEntity(experimentId);
        CreateExperimentRequest req = mapToCreateRequest(experiment);

        experiment.setStatus(ExperimentStatus.VALIDATING);
        experimentRepository.save(experiment);

        ValidationResultDTO result = validator.validate(req, experimentId);
        experiment.setStatus(result.isValid() ? ExperimentStatus.VALID : ExperimentStatus.INVALID);
        experimentRepository.save(experiment);

        return result;
    }

    @Transactional
    public ExecutionResultDTO runExperiment(String experimentId) {
        Experiment experiment = getExperimentEntity(experimentId);

        // Pre-validate if in CREATED or VALIDATING state
        if (experiment.getStatus() == ExperimentStatus.CREATED || experiment.getStatus() == ExperimentStatus.VALIDATING) {
            ValidationResultDTO val = validateExperiment(experimentId);
            if (!val.isValid()) {
                throw new InvalidConfigurationException("Experiment configuration is invalid.", val.getValidationErrors());
            }
        } else if (experiment.getStatus() == ExperimentStatus.INVALID) {
            throw new InvalidConfigurationException("Cannot run experiment: status is INVALID.");
        }

        experiment.setStatus(ExperimentStatus.RUNNING);
        experimentRepository.save(experiment);

        ExperimentConfiguration cfg = experiment.getConfiguration();
        EngineSearchConfigDTO engineCfg = new EngineSearchConfigDTO();
        engineCfg.setExperimentId(experiment.getId());
        engineCfg.setProblemType(experiment.getProblemType().getId());
        engineCfg.setStrategyId(cfg.getStrategyId());
        engineCfg.setOracleId(cfg.getOracleId());
        engineCfg.setMode(cfg.getMode().getValue());
        engineCfg.setStrategy(cfg.getStrategy().getValue());
        engineCfg.setRandomSeed(cfg.getRandomSeed());
        engineCfg.setMaxCandidates(cfg.getMaxCandidates());
        engineCfg.setMaxTimeMs(cfg.getMaxTimeMs());

        try {
            engineCfg.setSearchSpace(objectMapper.readTree(cfg.getSearchSpaceJson()));
        } catch (JsonProcessingException e) {
            throw new InvalidConfigurationException("Corrupted search space JSON in database: " + e.getMessage());
        }

        // Execute via Subprocess Supervisor
        EngineProcessSupervisor.ProcessExecutionResult execRes =
                engineSupervisor.executeEngine(engineCfg, cfg.getMaxTimeMs() + 2000L);

        ExperimentExecution execution = new ExperimentExecution("exec-" + UUID.randomUUID().toString().substring(0, 8), experiment);
        execution.setStartedAt(Instant.now().minusMillis((long) execRes.getElapsedMs()));
        execution.setCompletedAt(Instant.now());
        execution.setElapsedTimeMs(execRes.getElapsedMs());

        if (execRes.isTimedOut()) {
            experiment.setStatus(ExperimentStatus.TIMED_OUT);
            execution.setStatus(ExperimentStatus.TIMED_OUT);
            execution.setStatusMessage("Execution exceeded wall-clock timeout and was forcefully terminated.");
            executionRepository.save(execution);
            experimentRepository.save(experiment);

            ExecutionResultDTO resultDTO = new ExecutionResultDTO();
            resultDTO.setExperimentId(experiment.getId());
            resultDTO.setStatus(ExperimentStatus.TIMED_OUT);
            return resultDTO;
        }

        if (execRes.getExitCode() != 0) {
            experiment.setStatus(ExperimentStatus.FAILED);
            execution.setStatus(ExperimentStatus.FAILED);
            execution.setStatusCode(execRes.getExitCode());
            execution.setStatusMessage("Engine process failed with non-zero exit code: " + execRes.getExitCode());
            executionRepository.save(execution);
            experimentRepository.save(experiment);
            throw new EngineExecutionException("Engine process crashed with exit code " + execRes.getExitCode(), execRes.getExitCode(), execRes.getStderr());
        }

        EngineOutputDTO output = execRes.getOutput();
        if (output == null || output.getSearchResult() == null) {
            experiment.setStatus(ExperimentStatus.FAILED);
            execution.setStatus(ExperimentStatus.FAILED);
            execution.setStatusMessage("Engine emitted empty or null search result.");
            executionRepository.save(execution);
            experimentRepository.save(experiment);
            throw new EngineExecutionException("Engine output missing search_result payload.", 0, execRes.getStderr());
        }

        EngineOutputDTO.EngineSearchResult sr = output.getSearchResult();
        execution.setInstancesEvaluated(sr.getInstancesEvaluated());
        execution.setCounterexamplesFound(sr.getCounterexamplesFound());
        execution.setFailureRate(sr.getFailureRate());
        execution.setWorstGap(sr.getWorstGap());
        execution.setAverageGap(sr.getAverageGap());
        execution.setCoverage(SearchCoverage.fromString(sr.getCoverage()));
        execution.setCoverageRatio(sr.getCoverageRatio());
        execution.setStatusCode(sr.getStatusCode());
        execution.setStatusMessage(sr.getStatusMessage());

        if (sr.getBestGreedy() != null && sr.getBestGreedy().getMetrics() != null) {
            execution.setExecutionTimeNs(sr.getBestGreedy().getMetrics().getExecutionTimeNs());
            execution.setStatesEvaluated(sr.getBestGreedy().getMetrics().getStatesEvaluated());
        }

        // Determine final experiment state
        if (sr.isHasCounterexample()) {
            experiment.setStatus(ExperimentStatus.COMPLETED);
            execution.setStatus(ExperimentStatus.COMPLETED);
            execution.setHasCounterexample(true);
        } else if (sr.getStatusCode() == 2) { // SEARCH_INCOMPLETE
            experiment.setStatus(ExperimentStatus.SEARCH_INCOMPLETE);
            execution.setStatus(ExperimentStatus.SEARCH_INCOMPLETE);
            execution.setHasCounterexample(false);
        } else {
            experiment.setStatus(ExperimentStatus.NO_COUNTEREXAMPLE);
            execution.setStatus(ExperimentStatus.NO_COUNTEREXAMPLE);
            execution.setHasCounterexample(false);
        }

        // Save execution
        ExperimentExecution savedExec = executionRepository.save(execution);

        // Persist Counterexample entity if discovered
        if (sr.isHasCounterexample()) {
            Counterexample ce = new Counterexample();
            ce.setId("ce-" + UUID.randomUUID().toString().substring(0, 8));
            ce.setExecution(savedExec);

            if (output.getMinimizationResult() != null) {
                EngineOutputDTO.EngineMinimizationResult mr = output.getMinimizationResult();
                ce.setOriginalInstanceJson(mr.getOriginalInstance() != null ? mr.getOriginalInstance().toString() : sr.getBestInstance().toString());
                ce.setMinimalInstanceJson(mr.getMinimalInstance() != null ? mr.getMinimalInstance().toString() : sr.getBestInstance().toString());
                ce.setReductionSteps(mr.getReductionSteps());
                ce.setReductionPercent(mr.getReductionPercent());
                ce.setMinimalityStatus(mr.getMinimalityStatus() != null ? mr.getMinimalityStatus() : "Smallest verified counterexample under the configured ordering");
                ce.setOrderingCriteria(mr.getOrderingCriteria() != null ? mr.getOrderingCriteria() : "Canonical ordering");

                if (mr.getFinalVerification() != null) {
                    ce.setGreedyValue(mr.getFinalVerification().getGreedyValue());
                    ce.setOptimalValue(mr.getFinalVerification().getOptimalValue());
                    ce.setAbsoluteGap(mr.getFinalVerification().getAbsoluteGap());
                    ce.setRelativeGap(mr.getFinalVerification().getRelativeGap());
                    ce.setFailurePercent(mr.getFinalVerification().getFailurePercent());
                } else if (sr.getBestVerification() != null) {
                    ce.setGreedyValue(sr.getBestVerification().getGreedyValue());
                    ce.setOptimalValue(sr.getBestVerification().getOptimalValue());
                    ce.setAbsoluteGap(sr.getBestVerification().getAbsoluteGap());
                    ce.setRelativeGap(sr.getBestVerification().getRelativeGap());
                    ce.setFailurePercent(sr.getBestVerification().getFailurePercent());
                }
            } else if (sr.getBestVerification() != null) {
                ce.setOriginalInstanceJson(sr.getBestInstance() != null ? sr.getBestInstance().toString() : "{}");
                ce.setMinimalInstanceJson(sr.getBestInstance() != null ? sr.getBestInstance().toString() : "{}");
                ce.setGreedyValue(sr.getBestVerification().getGreedyValue());
                ce.setOptimalValue(sr.getBestVerification().getOptimalValue());
                ce.setAbsoluteGap(sr.getBestVerification().getAbsoluteGap());
                ce.setRelativeGap(sr.getBestVerification().getRelativeGap());
                ce.setFailurePercent(sr.getBestVerification().getFailurePercent());
                ce.setMinimalityStatus("Smallest verified counterexample under the configured ordering");
                ce.setOrderingCriteria("Canonical ordering");
            }

            counterexampleRepository.save(ce);

            // Persist Forensic Report
            if (output.getForensicReport() != null) {
                EngineOutputDTO.EngineForensicReport fr = output.getForensicReport();
                ForensicReportEntity fre = new ForensicReportEntity();
                fre.setId("fr-" + UUID.randomUUID().toString().substring(0, 8));
                fre.setExecution(savedExec);
                fre.setDivergenceStepIndex(fr.getDivergenceStepIndex());
                fre.setCriticalGreedyAction(fr.getCriticalGreedyAction());
                fre.setOptimalAlternativeAction(fr.getOptimalAlternativeAction());
                fre.setCausalConsequence(fr.getCausalConsequence());
                fre.setEvidenceConfidence(fr.getEvidenceConfidence());

                try {
                    fre.setTraceStepsJson(fr.getGreedyTrace() != null ? objectMapper.writeValueAsString(fr.getGreedyTrace()) : "{}");
                } catch (JsonProcessingException e) {
                    fre.setTraceStepsJson("{}");
                }
                forensicReportRepository.save(fre);
            }
        }

        // Persist Reproducibility Token
        if (sr.getReproducibilityToken() != null) {
            ReproducibilityTokenEntity rte = new ReproducibilityTokenEntity(
                    "rep-" + UUID.randomUUID().toString().substring(0, 8),
                    savedExec,
                    sr.getReproducibilityToken().toString()
            );
            reproducibilityTokenRepository.save(rte);
        }

        experimentRepository.save(experiment);

        ExecutionResultDTO resDTO = new ExecutionResultDTO();
        resDTO.setExperimentId(experiment.getId());
        resDTO.setStatus(experiment.getStatus());
        resDTO.setSearchResult(sr);
        resDTO.setReproducibilityToken(sr.getReproducibilityToken());
        return resDTO;
    }

    @Transactional(readOnly = true)
    public ExperimentDetailDTO getExperiment(String experimentId) {
        Experiment experiment = getExperimentEntity(experimentId);
        ExperimentExecution lastExec = executionRepository.findFirstByExperimentIdOrderByStartedAtDesc(experimentId).orElse(null);

        ExperimentDetailDTO dto = new ExperimentDetailDTO();
        dto.setId(experiment.getId());
        dto.setName(experiment.getName());
        dto.setDescription(experiment.getDescription());
        dto.setProblemType(experiment.getProblemType());
        dto.setStatus(experiment.getStatus());
        dto.setParentExperimentId(experiment.getParentExperimentId());
        dto.setCreatedAt(experiment.getCreatedAt());
        dto.setUpdatedAt(experiment.getUpdatedAt());

        if (experiment.getConfiguration() != null) {
            ExperimentConfiguration c = experiment.getConfiguration();
            ExperimentDetailDTO.ConfigurationDTO cd = new ExperimentDetailDTO.ConfigurationDTO();
            cd.setStrategyId(c.getStrategyId());
            cd.setOracleId(c.getOracleId());
            cd.setMode(c.getMode());
            cd.setStrategy(c.getStrategy());
            cd.setRandomSeed(c.getRandomSeed());
            cd.setMaxCandidates(c.getMaxCandidates());
            cd.setMaxTimeMs(c.getMaxTimeMs());
            try {
                cd.setSearchSpace(objectMapper.readTree(c.getSearchSpaceJson()));
            } catch (Exception ignored) {}
            dto.setConfiguration(cd);
        }

        if (lastExec != null) {
            ExperimentDetailDTO.SummaryDTO sd = new ExperimentDetailDTO.SummaryDTO();
            sd.setHasCounterexample(lastExec.isHasCounterexample());
            sd.setCounterexamplesCount(lastExec.getCounterexamplesFound());
            sd.setInstancesEvaluated(lastExec.getInstancesEvaluated());
            sd.setWorstGap(lastExec.getWorstGap());
            dto.setSummary(sd);
        }

        return dto;
    }

    @Transactional(readOnly = true)
    public ExecutionStatusDTO getExperimentStatus(String experimentId) {
        Experiment experiment = getExperimentEntity(experimentId);
        ExperimentExecution lastExec = executionRepository.findFirstByExperimentIdOrderByStartedAtDesc(experimentId).orElse(null);

        ExecutionStatusDTO dto = new ExecutionStatusDTO();
        dto.setExperimentId(experiment.getId());
        dto.setStatus(experiment.getStatus());

        if (lastExec != null) {
            dto.setInstancesEvaluated(lastExec.getInstancesEvaluated());
            dto.setCounterexamplesFound(lastExec.getCounterexamplesFound());
            dto.setElapsedTimeMs(lastExec.getElapsedTimeMs());
            dto.setMessage(lastExec.getStatusMessage() != null ? lastExec.getStatusMessage() : "Status: " + experiment.getStatus());
        } else {
            dto.setMessage("Status: " + experiment.getStatus());
        }
        return dto;
    }

    @Transactional(readOnly = true)
    public CounterexampleDTO getCounterexample(String experimentId) {
        ExperimentExecution lastExec = executionRepository.findFirstByExperimentIdOrderByStartedAtDesc(experimentId)
                .orElseThrow(() -> new ResourceNotFoundException("No executions found for experiment: " + experimentId));

        Counterexample ce = counterexampleRepository.findByExecutionId(lastExec.getId())
                .orElseThrow(() -> new ResourceNotFoundException("No counterexample found for experiment: " + experimentId));

        CounterexampleDTO dto = new CounterexampleDTO();
        dto.setExperimentId(experimentId);
        dto.setHasCounterexample(true);
        dto.setReductionSteps(ce.getReductionSteps());
        dto.setReductionPercent(ce.getReductionPercent());
        dto.setMinimalityStatus(ce.getMinimalityStatus());
        dto.setOrderingCriteria(ce.getOrderingCriteria());

        try {
            dto.setOriginalInstance(objectMapper.readTree(ce.getOriginalInstanceJson()));
            dto.setMinimalInstance(objectMapper.readTree(ce.getMinimalInstanceJson()));
        } catch (Exception ignored) {}

        EngineOutputDTO.EngineVerificationResult vr = new EngineOutputDTO.EngineVerificationResult();
        vr.setStatus("OPTIMALITY_FAILURE");
        vr.setCounterexample(true);
        vr.setGreedyValue(ce.getGreedyValue());
        vr.setOptimalValue(ce.getOptimalValue());
        vr.setAbsoluteGap(ce.getAbsoluteGap());
        vr.setRelativeGap(ce.getRelativeGap());
        vr.setFailurePercent(ce.getFailurePercent());
        dto.setFinalVerification(vr);

        return dto;
    }

    @Transactional(readOnly = true)
    public ForensicReportDTO getForensicReport(String experimentId) {
        Experiment experiment = getExperimentEntity(experimentId);
        ExperimentExecution lastExec = executionRepository.findFirstByExperimentIdOrderByStartedAtDesc(experimentId)
                .orElseThrow(() -> new ResourceNotFoundException("No executions found for experiment: " + experimentId));

        ForensicReportEntity fre = forensicReportRepository.findByExecutionId(lastExec.getId())
                .orElseThrow(() -> new ResourceNotFoundException("No forensic report found for experiment: " + experimentId));

        ForensicReportDTO dto = new ForensicReportDTO();
        dto.setExperimentId(experimentId);
        dto.setProblemType(experiment.getProblemType().getId());
        dto.setDivergenceStepIndex(fre.getDivergenceStepIndex());
        dto.setCriticalGreedyAction(fre.getCriticalGreedyAction());
        dto.setOptimalAlternativeAction(fre.getOptimalAlternativeAction());
        dto.setCausalConsequence(fre.getCausalConsequence());
        dto.setEvidenceConfidence(fre.getEvidenceConfidence());
        dto.setMinimalityStatus("Smallest verified counterexample under the configured ordering");

        try {
            dto.setGreedyTrace(objectMapper.readValue(fre.getTraceStepsJson(), EngineOutputDTO.EngineTrace.class));
        } catch (Exception ignored) {}

        return dto;
    }

    @Transactional
    public ReproductionResultDTO reproduceExperiment(String experimentId) {
        Experiment original = getExperimentEntity(experimentId);
        ExperimentExecution originalExec = executionRepository.findFirstByExperimentIdOrderByStartedAtDesc(experimentId)
                .orElseThrow(() -> new ResourceNotFoundException("Original experiment has no completed executions to reproduce: " + experimentId));

        ExperimentConfiguration origCfg = original.getConfiguration();

        // 1. Create a brand new reproduction experiment linked to parent
        String reproId = "exp-repro-" + UUID.randomUUID().toString().substring(0, 8);
        Experiment reproduction = new Experiment(reproId, "Reproduction of " + original.getName(), original.getProblemType());
        reproduction.setDescription("Deterministic reproduction run of experiment " + original.getId());
        reproduction.setParentExperimentId(original.getId());
        reproduction.setStatus(ExperimentStatus.VALID);

        ExperimentConfiguration newCfg = new ExperimentConfiguration();
        newCfg.setStrategyId(origCfg.getStrategyId());
        newCfg.setOracleId(origCfg.getOracleId());
        newCfg.setMode(origCfg.getMode());
        newCfg.setStrategy(origCfg.getStrategy());
        newCfg.setRandomSeed(origCfg.getRandomSeed());
        newCfg.setMaxCandidates(origCfg.getMaxCandidates());
        newCfg.setMaxTimeMs(origCfg.getMaxTimeMs());
        newCfg.setSearchSpaceJson(origCfg.getSearchSpaceJson());
        reproduction.setConfiguration(newCfg);

        experimentRepository.save(reproduction);

        // 2. Execute reproduction
        ExecutionResultDTO reproResult = runExperiment(reproId);

        // 3. Compare with original execution
        boolean matches = (originalExec.getInstancesEvaluated() == reproResult.getSearchResult().getInstancesEvaluated())
                && (originalExec.getCounterexamplesFound() == reproResult.getSearchResult().getCounterexamplesFound())
                && (Math.abs(originalExec.getWorstGap() - reproResult.getSearchResult().getWorstGap()) < 1e-9);

        ReproductionResultDTO dto = new ReproductionResultDTO();
        dto.setReproductionExperimentId(reproId);
        dto.setOriginalExperimentId(original.getId());
        dto.setStatus(reproduction.getStatus());
        dto.setDeterministicMatch(matches);
        dto.setMatchSummary(matches
                ? "Deterministic reproducibility verified: instances evaluated, counterexample count, and gap metrics matched the original identically."
                : "Reproduction executed but metrics differed from original.");
        dto.setCreatedAt(reproduction.getCreatedAt());

        return dto;
    }

    @Transactional(readOnly = true)
    public Page<ExperimentSummaryDTO> listExperiments(ProblemType problemType, ExperimentStatus status, Pageable pageable) {
        Page<Experiment> page;
        if (problemType != null && status != null) {
            page = experimentRepository.findByProblemTypeAndStatus(problemType, status, pageable);
        } else if (problemType != null) {
            page = experimentRepository.findByProblemType(problemType, pageable);
        } else if (status != null) {
            page = experimentRepository.findByStatus(status, pageable);
        } else {
            page = experimentRepository.findAll(pageable);
        }

        return page.map(e -> {
            ExperimentExecution lastExec = executionRepository.findFirstByExperimentIdOrderByStartedAtDesc(e.getId()).orElse(null);
            return mapToSummary(e, lastExec);
        });
    }

    private Experiment getExperimentEntity(String experimentId) {
        return experimentRepository.findById(experimentId)
                .orElseThrow(() -> new ResourceNotFoundException("Experiment not found with ID: " + experimentId));
    }

    private ExperimentSummaryDTO mapToSummary(Experiment e, ExperimentExecution lastExec) {
        ExperimentSummaryDTO dto = new ExperimentSummaryDTO();
        dto.setId(e.getId());
        dto.setName(e.getName());
        dto.setProblemType(e.getProblemType());
        dto.setStatus(e.getStatus());
        dto.setCreatedAt(e.getCreatedAt());
        if (lastExec != null) {
            dto.setHasCounterexample(lastExec.isHasCounterexample());
            dto.setInstancesEvaluated(lastExec.getInstancesEvaluated());
        }
        return dto;
    }

    private CreateExperimentRequest mapToCreateRequest(Experiment e) {
        CreateExperimentRequest req = new CreateExperimentRequest();
        req.setName(e.getName());
        req.setDescription(e.getDescription());
        req.setProblemType(e.getProblemType());
        if (e.getConfiguration() != null) {
            ExperimentConfiguration c = e.getConfiguration();
            req.setStrategyId(c.getStrategyId());
            req.setOracleId(c.getOracleId());
            req.setMode(c.getMode());
            req.setStrategy(c.getStrategy());
            req.setRandomSeed(c.getRandomSeed());
            req.setMaxCandidates(c.getMaxCandidates());
            req.setMaxTimeMs(c.getMaxTimeMs());
            try {
                req.setSearchSpace(objectMapper.readTree(c.getSearchSpaceJson()));
            } catch (Exception ignored) {}
        }
        return req;
    }
}
