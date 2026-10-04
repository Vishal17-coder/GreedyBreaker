#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace greedybreaker::core {

enum class ObjectiveDirection : uint8_t {
    MINIMIZE = 0,
    MAXIMIZE = 1
};

[[nodiscard]] constexpr std::string_view to_string(ObjectiveDirection dir) noexcept {
    switch (dir) {
        case ObjectiveDirection::MINIMIZE: return "MINIMIZE";
        case ObjectiveDirection::MAXIMIZE: return "MAXIMIZE";
    }
    return "UNKNOWN";
}

enum class VerificationStatus : uint8_t {
    OPTIMAL = 0,
    FEASIBILITY_FAILURE = 1,
    OPTIMALITY_FAILURE = 2
};

[[nodiscard]] constexpr std::string_view to_string(VerificationStatus status) noexcept {
    switch (status) {
        case VerificationStatus::OPTIMAL: return "OPTIMAL";
        case VerificationStatus::FEASIBILITY_FAILURE: return "FEASIBILITY_FAILURE";
        case VerificationStatus::OPTIMALITY_FAILURE: return "OPTIMALITY_FAILURE";
    }
    return "UNKNOWN";
}

enum class OracleConfidence : uint8_t {
    EXACT = 0,
    APPROXIMATION = 1,
    HEURISTIC = 2
};

[[nodiscard]] constexpr std::string_view to_string(OracleConfidence conf) noexcept {
    switch (conf) {
        case OracleConfidence::EXACT: return "EXACT";
        case OracleConfidence::APPROXIMATION: return "APPROXIMATION";
        case OracleConfidence::HEURISTIC: return "HEURISTIC";
    }
    return "UNKNOWN";
}

struct ExecutionMetrics {
    uint64_t execution_time_ns{0};
    uint64_t states_evaluated{0};
    uint64_t memory_bytes_peak{0};

    [[nodiscard]] double execution_time_ms() const noexcept {
        return static_cast<double>(execution_time_ns) / 1'000'000.0;
    }
};

} // namespace greedybreaker::core
