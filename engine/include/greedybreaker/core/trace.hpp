#pragma once

#include <string>
#include <vector>
#include <optional>

namespace greedybreaker {

/**
 * @brief Represents an individual decision step in an algorithm's execution trajectory.
 */
struct TraceStep {
    int step_number{0};
    std::string state_description;
    int remaining_target{0};
    std::vector<int> available_choices;
    std::optional<int> selected_choice;
    std::string outcome_description;
    int remaining_after{0};
    int accumulated_count{0};
    std::string rationale;
};

/**
 * @brief Complete chronological trace recorded during solver execution.
 */
struct AlgorithmTrace {
    std::string algorithm_name;
    std::string strategy_name;
    bool success{false};
    std::vector<TraceStep> steps;
    std::vector<int> final_solution;
    std::optional<double> final_cost;
    std::vector<std::string> execution_notes;
};

/**
 * @brief Forensic analysis of the earliest point where greedy decisions diverged from optimal choices.
 */
struct DivergenceResult {
    bool has_diverged{false};
    std::optional<int> divergence_step;
    int state_remaining{0};
    std::optional<int> greedy_choice;
    std::vector<int> optimal_choices;
    std::optional<double> greedy_projected_cost;
    std::optional<double> optimal_projected_cost;
    std::string explanation;
};

} // namespace greedybreaker
