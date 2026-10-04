package com.greedybreaker.model.enums;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonValue;

public enum ProblemType {
    COIN_CHANGE("coin_change"),
    KNAPSACK_01("knapsack_01"),
    ACTIVITY_SELECTION("activity_selection"),
    FRACTIONAL_KNAPSACK("fractional_knapsack"),
    HUFFMAN_CODING("huffman_coding");

    private final String id;

    ProblemType(String id) {
        this.id = id;
    }

    @JsonValue
    public String getId() {
        return id;
    }

    @JsonCreator
    public static ProblemType fromString(String value) {
        if (value == null) return null;
        for (ProblemType pt : values()) {
            if (pt.id.equalsIgnoreCase(value) || pt.name().equalsIgnoreCase(value)) {
                return pt;
            }
        }
        throw new IllegalArgumentException("Unknown problem type: " + value);
    }
}
