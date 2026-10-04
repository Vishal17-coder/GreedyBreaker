#pragma once

#include "greedybreaker/core/error.hpp"
#include "greedybreaker/core/solution.hpp"
#include "greedybreaker/core/types.hpp"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace greedybreaker::problems {

struct KnapsackItem {
    int64_t id{0};
    double value{0.0};
    double weight{0.0};

    [[nodiscard]] double density() const noexcept {
        return weight > 0 ? (value / weight) : 0.0;
    }

    [[nodiscard]] bool operator==(const KnapsackItem& other) const noexcept {
        return id == other.id && value == other.value && weight == other.weight;
    }
};

void to_json(nlohmann::json& j, const KnapsackItem& item);
void from_json(const nlohmann::json& j, KnapsackItem& item);

struct Knapsack01Instance {
    std::vector<KnapsackItem> items;
    double capacity{0.0};

    [[nodiscard]] bool operator==(const Knapsack01Instance& other) const noexcept {
        return capacity == other.capacity && items == other.items;
    }
};

void to_json(nlohmann::json& j, const Knapsack01Instance& inst);
void from_json(const nlohmann::json& j, Knapsack01Instance& inst);

class Knapsack01Problem {
public:
    [[nodiscard]] static std::string problem_id() noexcept { return "knapsack_01"; }
    [[nodiscard]] static std::string display_name() noexcept { return "0/1 Knapsack"; }
    [[nodiscard]] static core::ObjectiveDirection objective() noexcept { return core::ObjectiveDirection::MAXIMIZE; }

    [[nodiscard]] static bool is_valid_instance(const Knapsack01Instance& inst, std::string* error_msg = nullptr) noexcept;

    // Greedy solver supporting strategies:
    // - "value_density_descending" (default ratio greedy)
    // - "max_value_first"
    // - "min_weight_first"
    [[nodiscard]] static core::GreedyResult solve_greedy(
        const Knapsack01Instance& inst,
        const std::string& strategy = "value_density_descending"
    );

    // Exact Dynamic Programming Oracle (for integer or scaled capacities/weights)
    [[nodiscard]] static core::OracleResult solve_exact_dp(
        const Knapsack01Instance& inst
    );

    // Exact Exhaustive Subset Search (Cross-Validation for small N <= 20)
    [[nodiscard]] static core::OracleResult solve_exact_exhaustive(
        const Knapsack01Instance& inst
    );
};

} // namespace greedybreaker::problems
