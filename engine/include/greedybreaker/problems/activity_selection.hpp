#pragma once

#include "greedybreaker/core/error.hpp"
#include "greedybreaker/core/solution.hpp"
#include "greedybreaker/core/types.hpp"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace greedybreaker::problems {

struct Activity {
    int64_t id{0};
    int64_t start_time{0};
    int64_t finish_time{0};

    [[nodiscard]] int64_t duration() const noexcept {
        return finish_time - start_time;
    }

    [[nodiscard]] bool operator==(const Activity& other) const noexcept {
        return id == other.id && start_time == other.start_time && finish_time == other.finish_time;
    }
};

void to_json(nlohmann::json& j, const Activity& act);
void from_json(const nlohmann::json& j, Activity& act);

struct ActivitySelectionInstance {
    std::vector<Activity> activities;

    [[nodiscard]] bool operator==(const ActivitySelectionInstance& other) const noexcept {
        return activities == other.activities;
    }
};

void to_json(nlohmann::json& j, const ActivitySelectionInstance& inst);
void from_json(const nlohmann::json& j, ActivitySelectionInstance& inst);

class ActivitySelectionProblem {
public:
    [[nodiscard]] static std::string problem_id() noexcept { return "activity_selection"; }
    [[nodiscard]] static std::string display_name() noexcept { return "Activity Selection"; }
    [[nodiscard]] static core::ObjectiveDirection objective() noexcept { return core::ObjectiveDirection::MAXIMIZE; }

    [[nodiscard]] static bool is_valid_instance(const ActivitySelectionInstance& inst, std::string* error_msg = nullptr) noexcept;

    // Greedy solver supporting:
    // - "earliest_finish_time" (Mathematically optimal)
    // - "shortest_interval" (Breakable heuristic)
    // - "earliest_start_time" (Breakable heuristic)
    [[nodiscard]] static core::GreedyResult solve_greedy(
        const ActivitySelectionInstance& inst,
        const std::string& strategy = "earliest_finish_time"
    );

    // Exact Dynamic Programming Oracle OPT(j) = max(OPT(j-1), 1 + OPT(p(j)))
    [[nodiscard]] static core::OracleResult solve_exact_dp(
        const ActivitySelectionInstance& inst
    );

    // Exact Recursive Exhaustive Search (Cross-Validation for small N <= 20)
    [[nodiscard]] static core::OracleResult solve_exact_exhaustive(
        const ActivitySelectionInstance& inst
    );
};

} // namespace greedybreaker::problems
