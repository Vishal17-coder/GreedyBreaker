#pragma once

#include "greedybreaker/core/verification_result.hpp"
#include "greedybreaker/problems/coin_change.hpp"
#include "greedybreaker/problems/knapsack_01.hpp"
#include "greedybreaker/problems/activity_selection.hpp"
#include "greedybreaker/problems/huffman_coding.hpp"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace greedybreaker::minimization {

struct MinimizationResult {
    nlohmann::json original_instance;
    nlohmann::json minimal_instance;
    size_t reduction_steps{0};
    double original_size_metric{0.0};
    double minimal_size_metric{0.0};
    double reduction_percent{0.0};
    std::string ordering_criteria{
        "1. Minimal element count; 2. Minimal max parameter; 3. Minimal parameter sum; 4. Lexicographical"
    };
    std::string minimality_status{
        "Smallest verified counterexample under the configured ordering"
    };
    core::VerificationResult final_verification;
};

void to_json(nlohmann::json& j, const MinimizationResult& r);
void from_json(const nlohmann::json& j, MinimizationResult& r);

class CounterexampleMinimizer {
public:
    [[nodiscard]] static MinimizationResult minimize_coin_change(
        const problems::CoinChangeInstance& original,
        const std::string& strategy = "largest_denomination_first"
    );

    [[nodiscard]] static MinimizationResult minimize_knapsack_01(
        const problems::Knapsack01Instance& original,
        const std::string& strategy = "value_density_descending"
    );

    [[nodiscard]] static MinimizationResult minimize_activity_selection(
        const problems::ActivitySelectionInstance& original,
        const std::string& strategy = "shortest_interval"
    );

    [[nodiscard]] static MinimizationResult minimize_huffman(
        const problems::HuffmanCodingInstance& original,
        const std::string& strategy = "shannon_fano"
    );

    // Generic JSON dispatch
    [[nodiscard]] static MinimizationResult minimize(
        const std::string& problem_type,
        const nlohmann::json& instance_json,
        const std::string& strategy = "default"
    );
};

} // namespace greedybreaker::minimization
