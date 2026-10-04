#include "greedybreaker/problems/activity_selection.hpp"
#include <algorithm>
#include <chrono>
#include <sstream>

namespace greedybreaker::problems {

void to_json(nlohmann::json& j, const Activity& act) {
    j = nlohmann::json{
        {"id", act.id},
        {"start_time", act.start_time},
        {"finish_time", act.finish_time},
        {"duration", act.duration()}
    };
}

void from_json(const nlohmann::json& j, Activity& act) {
    act.id = j.value("id", int64_t{0});
    act.start_time = j.value("start_time", int64_t{0});
    act.finish_time = j.value("finish_time", int64_t{0});
}

void to_json(nlohmann::json& j, const ActivitySelectionInstance& inst) {
    j = nlohmann::json{
        {"activities", inst.activities}
    };
}

void from_json(const nlohmann::json& j, ActivitySelectionInstance& inst) {
    if (j.contains("activities")) {
        inst.activities = j.at("activities").get<std::vector<Activity>>();
    }
}

bool ActivitySelectionProblem::is_valid_instance(const ActivitySelectionInstance& inst, std::string* error_msg) noexcept {
    if (inst.activities.empty()) {
        if (error_msg) *error_msg = "Activities list cannot be empty.";
        return false;
    }
    for (const auto& act : inst.activities) {
        if (act.start_time < 0) {
            if (error_msg) *error_msg = "Start time cannot be negative.";
            return false;
        }
        if (act.start_time >= act.finish_time) {
            if (error_msg) *error_msg = "Activity start time must be strictly less than finish time.";
            return false;
        }
    }
    return true;
}

core::GreedyResult ActivitySelectionProblem::solve_greedy(
    const ActivitySelectionInstance& inst,
    const std::string& strategy)
{
    const auto start_time = std::chrono::steady_clock::now();
    core::GreedyResult result;

    if (!is_valid_instance(inst)) {
        result.feasible = false;
        result.objective_value = 0.0;
        result.solution_description = "Invalid Activity Selection instance.";
        return result;
    }

    auto sorted_acts = inst.activities;
    std::string rule_name = "Earliest Finish Time First";

    if (strategy == "shortest_interval") {
        rule_name = "Shortest Interval First";
        std::sort(sorted_acts.begin(), sorted_acts.end(), [](const Activity& a, const Activity& b) {
            if (a.duration() != b.duration()) return a.duration() < b.duration();
            if (a.start_time != b.start_time) return a.start_time < b.start_time;
            return a.id < b.id;
        });
    } else if (strategy == "earliest_start_time") {
        rule_name = "Earliest Start Time First";
        std::sort(sorted_acts.begin(), sorted_acts.end(), [](const Activity& a, const Activity& b) {
            if (a.start_time != b.start_time) return a.start_time < b.start_time;
            if (a.finish_time != b.finish_time) return a.finish_time < b.finish_time;
            return a.id < b.id;
        });
    } else {
        // Default: Earliest finish time
        std::sort(sorted_acts.begin(), sorted_acts.end(), [](const Activity& a, const Activity& b) {
            if (a.finish_time != b.finish_time) return a.finish_time < b.finish_time;
            if (a.start_time != b.start_time) return a.start_time < b.start_time;
            return a.id < b.id;
        });
    }

    int64_t last_finish = -1;
    std::vector<int64_t> selected_ids;

    for (size_t i = 0; i < sorted_acts.size(); ++i) {
        const auto& act = sorted_acts[i];
        core::DecisionStep step;
        step.step_index = i;
        step.state_before = "Last scheduled finish: " + std::to_string(last_finish);
        step.heuristic_rule = rule_name;

        core::CandidateChoice choice;
        choice.identifier = "Activity " + std::to_string(act.id) + " [" + std::to_string(act.start_time) + ", " + std::to_string(act.finish_time) + "]";
        choice.heuristic_score = static_cast<double>(act.finish_time);
        choice.feasible = (act.start_time >= last_finish);
        step.candidates_considered.push_back(choice);

        if (act.start_time >= last_finish) {
            selected_ids.push_back(act.id);
            last_finish = act.finish_time;
            step.action_description = "Scheduled " + choice.identifier;
            choice.notes = "Compatible; scheduled";
        } else {
            step.action_description = "Skipped " + choice.identifier + " (overlaps with scheduled activity)";
            choice.notes = "Overlaps with last finish " + std::to_string(last_finish);
        }

        step.state_after = "Last scheduled finish: " + std::to_string(last_finish);
        step.cumulative_objective = static_cast<double>(selected_ids.size());
        result.trace.add_step(step);
    }

    const auto end_time = std::chrono::steady_clock::now();
    result.metrics.execution_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    result.metrics.states_evaluated = sorted_acts.size();

    result.feasible = true;
    result.objective_value = static_cast<double>(selected_ids.size());
    result.items_selected = selected_ids;

    std::ostringstream oss;
    oss << "Scheduled " << selected_ids.size() << " non-overlapping activities";
    result.solution_description = oss.str();

    return result;
}

core::OracleResult ActivitySelectionProblem::solve_exact_dp(const ActivitySelectionInstance& inst) {
    const auto start_time = std::chrono::steady_clock::now();
    core::OracleResult result;
    result.confidence = core::OracleConfidence::EXACT;
    result.oracle_method = "Interval Scheduling Dynamic Programming O(N log N)";

    if (!is_valid_instance(inst)) {
        result.optimal_value = 0.0;
        result.solution_description = "Invalid Activity Selection instance.";
        return result;
    }

    const size_t N = inst.activities.size();
    auto sorted = inst.activities;
    std::sort(sorted.begin(), sorted.end(), [](const Activity& a, const Activity& b) {
        if (a.finish_time != b.finish_time) return a.finish_time < b.finish_time;
        return a.start_time < b.start_time;
    });

    // Compute predecessor p(j): last compatible activity index before j (1-based index)
    std::vector<int64_t> p(N + 1, 0);
    uint64_t states_count = 0;

    for (size_t j = 1; j <= N; ++j) {
        for (size_t i = j - 1; i > 0; --i) {
            ++states_count;
            if (sorted[i - 1].finish_time <= sorted[j - 1].start_time) {
                p[j] = static_cast<int64_t>(i);
                break;
            }
        }
    }

    // DP: opt[j] = max(opt[j-1], 1 + opt[p[j]])
    std::vector<int64_t> opt(N + 1, 0);
    std::vector<bool> take(N + 1, false);

    for (size_t j = 1; j <= N; ++j) {
        ++states_count;
        int64_t include_val = 1 + opt[static_cast<size_t>(p[j])];
        int64_t exclude_val = opt[j - 1];
        if (include_val > exclude_val) {
            opt[j] = include_val;
            take[j] = true;
        } else {
            opt[j] = exclude_val;
            take[j] = false;
        }
    }

    // Backtrack to find chosen activities
    std::vector<int64_t> optimal_ids;
    size_t curr = N;
    while (curr > 0) {
        if (take[curr]) {
            optimal_ids.push_back(sorted[curr - 1].id);
            curr = static_cast<size_t>(p[curr]);
        } else {
            --curr;
        }
    }
    std::reverse(optimal_ids.begin(), optimal_ids.end());

    const auto end_time = std::chrono::steady_clock::now();
    result.metrics.execution_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    result.metrics.states_evaluated = states_count;

    result.optimal_value = static_cast<double>(opt[N]);
    result.items_selected = optimal_ids;

    std::ostringstream oss;
    oss << "Exact optimal schedule contains " << opt[N] << " activities";
    result.solution_description = oss.str();

    return result;
}

core::OracleResult ActivitySelectionProblem::solve_exact_exhaustive(const ActivitySelectionInstance& inst) {
    const auto start_time = std::chrono::steady_clock::now();
    core::OracleResult result;
    result.confidence = core::OracleConfidence::EXACT;
    result.oracle_method = "Exhaustive Compatibility Search O(2^N)";

    if (!is_valid_instance(inst)) {
        result.optimal_value = 0.0;
        result.solution_description = "Invalid Activity Selection instance.";
        return result;
    }

    const size_t N = inst.activities.size();
    if (N > 25) {
        return solve_exact_dp(inst);
    }

    const uint64_t total_subsets = 1ULL << N;
    int64_t best_count = 0;
    uint64_t best_mask = 0;
    uint64_t states_count = 0;

    for (uint64_t mask = 0; mask < total_subsets; ++mask) {
        ++states_count;
        std::vector<Activity> subset;
        for (size_t i = 0; i < N; ++i) {
            if (mask & (1ULL << i)) {
                subset.push_back(inst.activities[i]);
            }
        }
        // Check if subset is mutually compatible
        std::sort(subset.begin(), subset.end(), [](const Activity& a, const Activity& b) {
            return a.start_time < b.start_time;
        });
        bool compatible = true;
        for (size_t i = 1; i < subset.size(); ++i) {
            if (subset[i].start_time < subset[i - 1].finish_time) {
                compatible = false;
                break;
            }
        }
        if (compatible && static_cast<int64_t>(subset.size()) > best_count) {
            best_count = static_cast<int64_t>(subset.size());
            best_mask = mask;
        }
    }

    std::vector<int64_t> optimal_ids;
    for (size_t i = 0; i < N; ++i) {
        if (best_mask & (1ULL << i)) {
            optimal_ids.push_back(inst.activities[i].id);
        }
    }

    const auto end_time = std::chrono::steady_clock::now();
    result.metrics.execution_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    result.metrics.states_evaluated = states_count;

    result.optimal_value = static_cast<double>(best_count);
    result.items_selected = optimal_ids;

    std::ostringstream oss;
    oss << "Exhaustive optimum contains " << best_count << " activities";
    result.solution_description = oss.str();

    return result;
}

} // namespace greedybreaker::problems
