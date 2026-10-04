package com.greedybreaker.model.enums;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonValue;

public enum SearchCoverage {
    EXHAUSTIVE("EXHAUSTIVE"),
    PARTIAL("PARTIAL"),
    RANDOMIZED_SAMPLE("RANDOMIZED_SAMPLE");

    private final String value;

    SearchCoverage(String value) {
        this.value = value;
    }

    @JsonValue
    public String getValue() {
        return value;
    }

    @JsonCreator
    public static SearchCoverage fromString(String val) {
        if (val == null) return PARTIAL;
        for (SearchCoverage c : values()) {
            if (c.value.equalsIgnoreCase(val) || c.name().equalsIgnoreCase(val)) {
                return c;
            }
        }
        return PARTIAL;
    }
}
