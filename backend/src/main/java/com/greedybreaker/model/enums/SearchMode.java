package com.greedybreaker.model.enums;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonValue;

public enum SearchMode {
    ANY("ANY"),
    SMALLEST("SMALLEST"),
    WORST("WORST"),
    PROFILE("PROFILE");

    private final String value;

    SearchMode(String value) {
        this.value = value;
    }

    @JsonValue
    public String getValue() {
        return value;
    }

    @JsonCreator
    public static SearchMode fromString(String val) {
        if (val == null) return ANY;
        for (SearchMode m : values()) {
            if (m.value.equalsIgnoreCase(val) || m.name().equalsIgnoreCase(val)) {
                return m;
            }
        }
        return ANY;
    }
}
