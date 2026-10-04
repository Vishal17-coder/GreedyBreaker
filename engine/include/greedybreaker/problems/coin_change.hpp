#pragma once

#include "greedybreaker/core/error.hpp"
#include "greedybreaker/core/solution.hpp"
#include "greedybreaker/core/types.hpp"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace greedybreaker::problems {

struct CoinChangeInstance {
    std::vector<int64_t> denominations;
    int64_t target{0};

    [[nodiscard]] bool operator==(const CoinChangeInstance& other) const noexcept {
        return target == other.target && denominations == other.denominations;
    }
};

void to_json(nlohmann::json& j, const CoinChangeInstance& inst);
void from_json(const nlohmann::json& j, CoinChangeInstance& inst);

class CoinChangeProblem {
public:
    [[nodiscard]] static std::string problem_id() noexcept { return "coin_change"; }
    [[nodiscard]] static std::string display_name() noexcept { return "Coin Change"; }
    [[nodiscard]] static core::ObjectiveDirection objective() noexcept { return core::ObjectiveDirection::MINIMIZE; }

    [[nodiscard]] static bool is_valid_instance(const CoinChangeInstance& inst, std::string* error_msg = nullptr) noexcept;

    // Greedy solver with decision tracing
    [[nodiscard]] static core::GreedyResult solve_greedy(
        const CoinChangeInstance& inst,
        const std::string& strategy = "largest_denomination_first"
    );

    // Exact Dynamic Programming Oracle
    [[nodiscard]] static core::OracleResult solve_exact_dp(
        const CoinChangeInstance& inst
    );

    // Alternative Exact Oracle for Cross-Validation (BFS Branch & Bound)
    [[nodiscard]] static core::OracleResult solve_exact_bfs(
        const CoinChangeInstance& inst
    );
};

} // namespace greedybreaker::problems
