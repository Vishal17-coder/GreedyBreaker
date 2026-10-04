#pragma once

#include "greedybreaker/core/error.hpp"
#include "greedybreaker/core/solution.hpp"
#include "greedybreaker/core/verification_result.hpp"
#include "greedybreaker/search/search_config.hpp"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace greedybreaker::search {

struct SearchResult {
    core::Status status{core::Status::OK()};
    std::string experiment_id;
    std::string problem_type;
    std::string strategy_id;
    std::string oracle_id;

    SearchMode mode{SearchMode::ANY};
    SearchStrategy strategy{SearchStrategy::EXHAUSTIVE};
    SearchCoverage coverage{SearchCoverage::EXHAUSTIVE};

    uint64_t estimated_space_size{0};
    uint64_t instances_evaluated{0};
    uint64_t counterexamples_found{0};
    double failure_rate{0.0};
    double average_gap{0.0};
    double worst_gap{0.0};
    double elapsed_time_ms{0.0};
    double coverage_ratio{0.0};

    bool has_counterexample{false};
    nlohmann::json best_instance;
    core::GreedyResult best_greedy;
    core::OracleResult best_oracle;
    core::VerificationResult best_verification;

    nlohmann::json reproducibility_token;
};

void to_json(nlohmann::json& j, const SearchResult& r);
void from_json(const nlohmann::json& j, SearchResult& r);

} // namespace greedybreaker::search
