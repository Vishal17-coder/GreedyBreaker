package com.greedybreaker.dto.request;

public class IntRangeDTO {
    private long min;
    private long max;

    public IntRangeDTO() {}

    public IntRangeDTO(long min, long max) {
        this.min = min;
        this.max = max;
    }

    public long getMin() { return min; }
    public void setMin(long min) { this.min = min; }

    public long getMax() { return max; }
    public void setMax(long max) { this.max = max; }

    public boolean isValid() {
        return min <= max;
    }

    public long span() {
        return (max >= min) ? (max - min + 1) : 0;
    }
}
