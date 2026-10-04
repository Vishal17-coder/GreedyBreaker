#pragma once

#include "greedybreaker/core/error.hpp"
#include "greedybreaker/core/solution.hpp"
#include "greedybreaker/core/types.hpp"
#include "greedybreaker/problems/knapsack_01.hpp"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace greedybreaker::problems {

struct FractionalKnapsackInstance {
    std::vector<KnapsackItem> items;
    double capacity{0.0};

    [[nodiscard]] bool operator==(const FractionalKnapsackInstance& other) const noexcept {
        return capacity == other.capacity && items == other.items;
    }
};

void to_json(nlohmann::json& j, const FractionalKnapsackInstance& inst);
void from_json(const nlohmann::json& j, FractionalKnapsackInstance& inst);

class FractionalKnapsackProblem {
public:
    [[nodiscard]] static std::string problem_id() noexcept { return "fractional_knapsack"; }
    [[nodiscard]] static std::string display_name() noexcept { return "Fractional Knapsack"; }
    [[nodiscard]] static core::ObjectiveDirection objective() noexcept { return core::ObjectiveDirection::MAXIMIZE; }

    [[nodiscard]] static bool is_valid_instance(const FractionalKnapsackInstance& inst, std::string* error_msg = nullptr) noexcept;

    // Greedy solver supporting:
    // - "value_density_descending" (Mathematically optimal)
    // - "max_value_first" (Breakable heuristic)
    // - "min_weight_first" (Breakable heuristic)
    [[nodiscard]] static core::GreedyResult solve_greedy(
        const FractionalKnapsackInstance& inst,
        const std::string& strategy = "value_density_descending"
    );

    // Exact Continuous Linear Programming / Relaxation Oracle
    [[nodiscard]] static core::OracleResult solve_exact_lp(
        const FractionalKnapsackInstance& inst
    );
};

} // namespace greedybreaker::problems
