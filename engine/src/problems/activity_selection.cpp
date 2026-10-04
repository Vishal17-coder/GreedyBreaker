#include "greedybreaker/problems/activity_selection.hpp"
#include <algorithm>
#include <unordered_set>
#include <sstream>

namespace greedybreaker {

std::string ActivitySelectionProblem::name() const {
    return "Activity Selection";
}

ProblemObjective ActivitySelectionProblem::objective() const {
    return ProblemObjective::Maximize;
}

std::string ActivitySelectionProblem::default_strategy() const {
    return STRATEGY_EARLIEST_FINISH;
}

std::vector<std::string> ActivitySelectionProblem::available_strategies() const {
    return {STRATEGY_EARLIEST_FINISH};
}

void ActivitySelectionProblem::validate_candidate(const ActivitySelectionCandidate& candidate) const {
    if (candidate.activities.empty()) {
        throw InvalidCandidateException("Activity set cannot be empty.");
    }

    std::unordered_set<int> seen_ids;
    for (const auto& act : candidate.activities) {
        if (act.start < 0 || act.finish < 0) {
            throw InvalidCandidateException("Activity times must be non-negative: found start=" +
                                            std::to_string(act.start) + ", finish=" + std::to_string(act.finish));
        }
        if (act.start >= act.finish) {
            throw InvalidCandidateException("Invalid interval: start time (" + std::to_string(act.start) +
                                            ") must be strictly less than finish time (" +
                                            std::to_string(act.finish) + ") for Activity ID " +
                                            std::to_string(act.id));
        }
        if (!seen_ids.insert(act.id).second) {
            throw InvalidCandidateException("Duplicate activity ID detected: " + std::to_string(act.id));
        }
    }
}

Solution<Activity> ActivitySelectionProblem::greedy_solution(
    const ActivitySelectionCandidate& candidate,
    const std::string& strategy
) const {
    validate_candidate(candidate);

    Solution<Activity> solution;
    solution.trace.algorithm_name = "Greedy Activity Selection";
    solution.trace.strategy_name = strategy.empty() ? STRATEGY_EARLIEST_FINISH : strategy;

    std::vector<Activity> sorted = candidate.activities;
    std::sort(sorted.begin(), sorted.end());

    int last_finish = -1;
    int step_num = 1;

    for (const auto& act : sorted) {
        TraceStep step;
        step.step_number = step_num++;
        step.state_description = "Evaluating Activity " + std::to_string(act.id) +
                                 " [" + std::to_string(act.start) + ", " + std::to_string(act.finish) + "]";
        step.remaining_target = (last_finish == -1) ? 0 : last_finish;
        step.available_choices = {act.id};

        if (last_finish == -1 || act.start >= last_finish) {
            step.selected_choice = act.id;
            step.outcome_description = "SELECTED: Activity " + std::to_string(act.id) +
                                       " [" + std::to_string(act.start) + ", " + std::to_string(act.finish) + "]";
            step.remaining_after = act.finish;
            solution.items.push_back(act);
            last_finish = act.finish;
            step.accumulated_count = static_cast<int>(solution.items.size());
            step.rationale = "Compatible with previous finish time (" +
                             (step.remaining_target == 0 ? "none" : std::to_string(step.remaining_target)) +
                             "); earliest available finish time (" + std::to_string(act.finish) + ").";
        } else {
            step.selected_choice = std::nullopt;
            step.outcome_description = "SKIPPED: Conflict detected";
            step.remaining_after = last_finish;
            step.accumulated_count = static_cast<int>(solution.items.size());
            step.rationale = "Incompatible: Activity starts at " + std::to_string(act.start) +
                             ", before preceding finish time " + std::to_string(last_finish) + ".";
        }

        solution.trace.steps.push_back(step);
    }

    solution.success = true;
    solution.cost = static_cast<double>(solution.items.size());
    solution.trace.success = true;
    solution.trace.final_cost = solution.cost;
    for (const auto& act : solution.items) {
        solution.trace.final_solution.push_back(act.id);
    }

    return solution;
}

Solution<Activity> ActivitySelectionProblem::optimal_solution(
    const ActivitySelectionCandidate& candidate
) const {
    validate_candidate(candidate);

    Solution<Activity> solution;
    solution.trace.algorithm_name = "Exact DP Interval Scheduling Oracle";
    solution.trace.strategy_name = "Dynamic Programming Recurrence OPT(j) = max(OPT(j-1), 1 + OPT(p(j)))";

    std::vector<Activity> sorted = candidate.activities;
    std::sort(sorted.begin(), sorted.end());

    const size_t n = sorted.size();

    // Compute predecessor p[j]: latest activity i < j compatible with j
    std::vector<int> p(n + 1, 0);
    for (size_t j = 1; j <= n; ++j) {
        for (int i = static_cast<int>(j) - 1; i >= 1; --i) {
            if (sorted[i - 1].finish <= sorted[j - 1].start) {
                p[j] = i;
                break;
            }
        }
    }

    // DP table: dp[j] = max non-overlapping activities from subset {1..j}
    std::vector<int> dp(n + 1, 0);
    for (size_t j = 1; j <= n; ++j) {
        dp[j] = std::max(dp[j - 1], 1 + dp[p[j]]);
    }

    // Reconstruct optimal subset
    size_t curr = n;
    std::vector<Activity> optimal_subset;
    while (curr > 0) {
        if (1 + dp[p[curr]] > dp[curr - 1]) {
            optimal_subset.push_back(sorted[curr - 1]);
            curr = p[curr];
        } else {
            curr = curr - 1;
        }
    }

    std::reverse(optimal_subset.begin(), optimal_subset.end());
    solution.items = optimal_subset;
    solution.success = true;
    solution.cost = static_cast<double>(solution.items.size());
    solution.trace.success = true;
    solution.trace.final_cost = solution.cost;

    for (const auto& act : solution.items) {
        solution.trace.final_solution.push_back(act.id);
    }

    // Record summary trace
    TraceStep summary_step;
    summary_step.step_number = 1;
    summary_step.state_description = "DP Interval Scheduling Recurrence evaluated on " + std::to_string(n) + " intervals.";
    summary_step.outcome_description = "Optimal activity count: " + std::to_string(solution.items.size());
    summary_step.accumulated_count = static_cast<int>(solution.items.size());
    summary_step.rationale = "Mathematically optimal interval scheduling verified via dynamic programming.";
    solution.trace.steps.push_back(summary_step);

    return solution;
}

DivergenceResult ActivitySelectionProblem::analyze_divergence(
    const ActivitySelectionCandidate& candidate,
    const Solution<Activity>& greedy,
    const Solution<Activity>& optimal
) const {
    DivergenceResult result;
    const double g_val = greedy.cost.value_or(0.0);
    const double opt_val = optimal.cost.value_or(0.0);

    if (g_val >= opt_val) {
        result.has_diverged = false;
        result.explanation = "Greedy Earliest-Finish-Time achieved the optimal schedule size of " +
                             std::to_string(static_cast<int>(g_val)) + " activities. No divergence detected.";
        return result;
    }

    // In the canonical formulation earliest-finish-time is optimal, so this branch is only reached
    // if a suboptimal greedy strategy is used or an instance fails.
    result.has_diverged = true;
    result.divergence_step = 1;
    result.explanation = "Greedy schedule obtained " + std::to_string(static_cast<int>(g_val)) +
                         " activities, strictly less than optimal capacity " +
                         std::to_string(static_cast<int>(opt_val)) + ".";
    return result;
}

ComplexityReport ActivitySelectionProblem::complexity_info() const {
    ComplexityReport report;
    report.problem_name = "Activity Selection";
    report.solvers = {
        ComplexityEntry{
            "Greedy (Earliest Finish Time First)",
            "O(n log n)",
            "O(n log n)",
            "O(n log n)",
            "O(n)",
            "Sorting n activities by finish time takes O(n log n). A single linear pass of O(n) "
            "selects compatible activities. Auxiliary space is O(n) for sorted copy and trace."
        },
        ComplexityEntry{
            "Dynamic Programming (Exact Optimal Oracle)",
            "O(n log n)",
            "O(n log n)",
            "O(n^2)",
            "O(n)",
            "Interval scheduling DP recurrence OPT(j) = max(OPT(j-1), 1 + OPT(p(j))). Finding predecessors "
            "takes O(n log n) with binary search or O(n^2) linearly. DP table and reconstruction require O(n) space."
        }
    };
    return report;
}

} // namespace greedybreaker
