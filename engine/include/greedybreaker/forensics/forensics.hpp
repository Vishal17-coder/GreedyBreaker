#pragma once

#include "greedybreaker/core/solution.hpp"
#include "greedybreaker/core/trace.hpp"
#include "greedybreaker/core/types.hpp"
#include "greedybreaker/core/verification_result.hpp"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace greedybreaker::forensics {

struct ForensicReport {
    std::string problem_type;
    std::string instance_summary;
    nlohmann::json instance_data;

    std::string greedy_strategy;
    std::string greedy_solution;
    double greedy_objective{0.0};

    std::string optimal_method;
    std::string optimal_solution;
    double optimal_objective{0.0};

    double absolute_gap{0.0};
    double relative_gap{1.0};
    double failure_percent{0.0};

    core::DecisionTrace greedy_trace;

    int64_t divergence_step_index{-1};
    std::string critical_greedy_action;
    std::string optimal_alternative_action;
    std::string causal_consequence;
    std::string evidence_confidence{"HIGH_CONFIDENCE_EVIDENCE"};

    std::string minimality_status;
    core::ExecutionMetrics execution_metrics;
};

void to_json(nlohmann::json& j, const ForensicReport& rep);
void from_json(const nlohmann::json& j, ForensicReport& rep);

class ForensicAnalyzer {
public:
    [[nodiscard]] static ForensicReport analyze(
        const std::string& problem_type,
        const nlohmann::json& instance_json,
        const core::GreedyResult& greedy,
        const core::OracleResult& oracle,
        const core::VerificationResult& verification,
        const std::string& minimality_status = "Unchecked"
    );
};

} // namespace greedybreaker::forensics
