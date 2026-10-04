package com.greedybreaker.validation;

import com.fasterxml.jackson.databind.JsonNode;
import com.greedybreaker.config.EngineProperties;
import com.greedybreaker.dto.request.CreateExperimentRequest;
import com.greedybreaker.dto.response.ValidationResultDTO;
import com.greedybreaker.model.enums.ExperimentStatus;
import com.greedybreaker.model.enums.ProblemType;
import com.greedybreaker.model.enums.SearchStrategy;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.List;

@Component
public class ExperimentValidator {

    private final EngineProperties engineProperties;

    public ExperimentValidator(EngineProperties engineProperties) {
        this.engineProperties = engineProperties;
    }

    public ValidationResultDTO validate(CreateExperimentRequest request, String experimentId) {
        ValidationResultDTO result = new ValidationResultDTO();
        result.setExperimentId(experimentId);
        List<ValidationResultDTO.ValidationErrorItem> errors = new ArrayList<>();
        List<String> warnings = new ArrayList<>();

        if (request.getProblemType() == null) {
            errors.add(new ValidationResultDTO.ValidationErrorItem("problem_type", "null", "problem_type is required."));
            result.setValid(false);
            result.setStatus(ExperimentStatus.INVALID);
            result.setValidationErrors(errors);
            return result;
        }

        // 1. Candidate and Duration Hard Limits
        if (request.getMaxCandidates() != null && request.getMaxCandidates() > engineProperties.getMaxCandidatesLimit()) {
            errors.add(new ValidationResultDTO.ValidationErrorItem(
                    "max_candidates",
                    String.valueOf(request.getMaxCandidates()),
                    "Candidate count exceeds system maximum ceiling of " + engineProperties.getMaxCandidatesLimit()
            ));
        }

        if (request.getMaxTimeMs() != null && request.getMaxTimeMs() > engineProperties.getMaxDurationLimitMs()) {
            errors.add(new ValidationResultDTO.ValidationErrorItem(
                    "max_time_ms",
                    String.valueOf(request.getMaxTimeMs()),
                    "Max duration exceeds system maximum ceiling of " + engineProperties.getMaxDurationLimitMs() + " ms"
            ));
        }

        // 2. Search Space Structure & Range Checks
        JsonNode space = request.getSearchSpace();
        if (space == null || space.isNull() || space.isEmpty()) {
            errors.add(new ValidationResultDTO.ValidationErrorItem("search_space", "null", "search_space definition cannot be empty."));
        } else {
            validateSearchSpaceRanges(request.getProblemType(), space, errors, warnings);
        }

        // 3. Search Space Cardinality Estimation
        long estimatedSize = estimateCardinality(request.getProblemType(), space);
        result.setEstimatedSpaceSize(estimatedSize);

        if (request.getStrategy() == SearchStrategy.EXHAUSTIVE && estimatedSize > 1_000_000_000_000L) {
            errors.add(new ValidationResultDTO.ValidationErrorItem(
                    "strategy",
                    "EXHAUSTIVE",
                    "Exhaustive search space exceeds 10^12 instances. Switch to RANDOMIZED or GUIDED strategy, or restrict bounds."
            ));
        } else if (estimatedSize > 100_000_000L && request.getStrategy() == SearchStrategy.EXHAUSTIVE) {
            warnings.add("Large search space (" + estimatedSize + " instances). Search may be terminated early by max_candidates budget.");
        }

        if (errors.isEmpty()) {
            result.setValid(true);
            result.setStatus(ExperimentStatus.VALID);
        } else {
            result.setValid(false);
            result.setStatus(ExperimentStatus.INVALID);
        }
        result.setWarnings(warnings);
        result.setValidationErrors(errors);
        return result;
    }

    private void validateSearchSpaceRanges(ProblemType type, JsonNode space,
                                           List<ValidationResultDTO.ValidationErrorItem> errors,
                                           List<String> warnings) {
        switch (type) {
            case COIN_CHANGE -> {
                validateRange(space, "denomination_count", errors, 1, 50);
                validateRange(space, "denomination_range", errors, 1, 100_000);
                validateRange(space, "target_range", errors, 1, 100_000);
            }
            case KNAPSACK_01, FRACTIONAL_KNAPSACK -> {
                validateRange(space, "item_count", errors, 1, 50);
                validateRange(space, "weight_range", errors, 1, 100_000);
                validateRange(space, "value_range", errors, 1, 100_000);
                validateRange(space, "capacity_range", errors, 1, 100_000);
            }
            case ACTIVITY_SELECTION -> {
                validateRange(space, "activity_count", errors, 1, 50);
                validateRange(space, "time_range", errors, 0, 100_000);
                validateRange(space, "duration_range", errors, 1, 100_000);
            }
            case HUFFMAN_CODING -> {
                validateRange(space, "symbol_count", errors, 2, 50);
                validateRange(space, "frequency_range", errors, 1, 100_000);
            }
        }
    }

    private void validateRange(JsonNode parent, String field, List<ValidationResultDTO.ValidationErrorItem> errors, long hardMin, long hardMax) {
        if (!parent.has(field)) {
            errors.add(new ValidationResultDTO.ValidationErrorItem(field, "missing", "Required range field '" + field + "' is missing."));
            return;
        }
        JsonNode r = parent.get(field);
        if (!r.has("min") || !r.has("max")) {
            errors.add(new ValidationResultDTO.ValidationErrorItem(field, r.toString(), "Range '" + field + "' must specify both 'min' and 'max'."));
            return;
        }
        long min = r.get("min").asLong();
        long max = r.get("max").asLong();

        if (min > max) {
            errors.add(new ValidationResultDTO.ValidationErrorItem(
                    field,
                    "{min: " + min + ", max: " + max + "}",
                    "Minimum value (" + min + ") cannot exceed maximum value (" + max + ") for range '" + field + "'."
            ));
        }
        if (min < hardMin) {
            errors.add(new ValidationResultDTO.ValidationErrorItem(
                    field + ".min",
                    String.valueOf(min),
                    "Minimum value (" + min + ") cannot be less than system lower bound " + hardMin + "."
            ));
        }
        if (max > hardMax) {
            errors.add(new ValidationResultDTO.ValidationErrorItem(
                    field + ".max",
                    String.valueOf(max),
                    "Maximum value (" + max + ") cannot exceed system upper bound " + hardMax + "."
            ));
        }
    }

    private long estimateCardinality(ProblemType type, JsonNode space) {
        if (space == null || space.isNull()) return 0;
        try {
            switch (type) {
                case COIN_CHANGE -> {
                    long kMin = space.path("denomination_count").path("min").asLong(3);
                    long kMax = space.path("denomination_count").path("max").asLong(3);
                    long dSpan = Math.max(0, space.path("denomination_range").path("max").asLong(10) - space.path("denomination_range").path("min").asLong(1) + 1);
                    long tSpan = Math.max(0, space.path("target_range").path("max").asLong(20) - space.path("target_range").path("min").asLong(1) + 1);
                    long combos = 0;
                    for (long k = kMin; k <= kMax; ++k) {
                        combos += nCr(dSpan, k);
                    }
                    return combos * tSpan;
                }
                case KNAPSACK_01, FRACTIONAL_KNAPSACK -> {
                    long nMax = space.path("item_count").path("max").asLong(4);
                    long wSpan = Math.max(1, space.path("weight_range").path("max").asLong(20) - space.path("weight_range").path("min").asLong(1) + 1);
                    long vSpan = Math.max(1, space.path("value_range").path("max").asLong(50) - space.path("value_range").path("min").asLong(1) + 1);
                    long cSpan = Math.max(1, space.path("capacity_range").path("max").asLong(30) - space.path("capacity_range").path("min").asLong(10) + 1);
                    double itemPairs = (double) wSpan * vSpan;
                    double total = Math.pow(itemPairs, nMax) * cSpan;
                    return (total > 1e12) ? 1_000_000_000_000L : (long) total;
                }
                case ACTIVITY_SELECTION -> {
                    long nMax = space.path("activity_count").path("max").asLong(4);
                    long tSpan = Math.max(1, space.path("time_range").path("max").asLong(20) - space.path("time_range").path("min").asLong(0) + 1);
                    long dSpan = Math.max(1, space.path("duration_range").path("max").asLong(10) - space.path("duration_range").path("min").asLong(1) + 1);
                    double options = (double) tSpan * dSpan;
                    double total = Math.pow(options, nMax);
                    return (total > 1e12) ? 1_000_000_000_000L : (long) total;
                }
                case HUFFMAN_CODING -> {
                    long nMax = space.path("symbol_count").path("max").asLong(4);
                    long fSpan = Math.max(1, space.path("frequency_range").path("max").asLong(100) - space.path("frequency_range").path("min").asLong(1) + 1);
                    double total = Math.pow(fSpan, nMax);
                    return (total > 1e12) ? 1_000_000_000_000L : (long) total;
                }
            }
        } catch (Exception e) {
            return 0;
        }
        return 0;
    }

    private static long nCr(long n, long r) {
        if (r > n) return 0;
        if (r == 0 || r == n) return 1;
        if (r > n / 2) r = n - r;
        long res = 1;
        for (long i = 1; i <= r; ++i) {
            res = res * (n - i + 1) / i;
        }
        return res;
    }
}
