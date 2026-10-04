package com.greedybreaker.model.enums;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonValue;

public enum VerificationStatus {
    OPTIMAL_MATCH("OPTIMAL_MATCH"),
    OPTIMALITY_FAILURE("OPTIMALITY_FAILURE"),
    FEASIBILITY_FAILURE("FEASIBILITY_FAILURE"),
    ORACLE_FAILURE("ORACLE_FAILURE"),
    INCONCLUSIVE("INCONCLUSIVE");

    private final String value;

    VerificationStatus(String value) {
        this.value = value;
    }

    @JsonValue
    public String getValue() {
        return value;
    }

    @JsonCreator
    public static VerificationStatus fromString(String val) {
        if (val == null) return INCONCLUSIVE;
        for (VerificationStatus s : values()) {
            if (s.value.equalsIgnoreCase(val) || s.name().equalsIgnoreCase(val)) {
                return s;
            }
        }
        return INCONCLUSIVE;
    }
}
