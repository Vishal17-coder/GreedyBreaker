package com.greedybreaker.model.enums;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonValue;

public enum SearchStrategy {
    EXHAUSTIVE("EXHAUSTIVE"),
    RANDOMIZED("RANDOMIZED"),
    GUIDED("GUIDED"),
    HYBRID("HYBRID");

    private final String value;

    SearchStrategy(String value) {
        this.value = value;
    }

    @JsonValue
    public String getValue() {
        return value;
    }

    @JsonCreator
    public static SearchStrategy fromString(String val) {
        if (val == null) return EXHAUSTIVE;
        for (SearchStrategy s : values()) {
            if (s.value.equalsIgnoreCase(val) || s.name().equalsIgnoreCase(val)) {
                return s;
            }
        }
        return EXHAUSTIVE;
    }
}
