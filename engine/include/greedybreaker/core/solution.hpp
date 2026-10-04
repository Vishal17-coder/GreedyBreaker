#pragma once

#include "greedybreaker/core/trace.hpp"
#include "greedybreaker/core/types.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace greedybreaker::core {

struct GreedyResult {
    bool feasible{false};
    double objective_value{0.0};
    std::string solution_description;
    std::vector<int64_t> items_selected;
    DecisionTrace trace;
    ExecutionMetrics metrics;
};

struct OracleResult {
    OracleConfidence confidence{OracleConfidence::EXACT};
    double optimal_value{0.0};
    std::string solution_description;
    std::vector<int64_t> items_selected;
    std::string oracle_method;
    ExecutionMetrics metrics;

    [[nodiscard]] bool is_exact() const noexcept {
        return confidence == OracleConfidence::EXACT;
    }
};

} // namespace greedybreaker::core
