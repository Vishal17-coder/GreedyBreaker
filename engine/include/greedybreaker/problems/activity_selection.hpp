#pragma once

#include "greedybreaker/core/problem.hpp"
#include <vector>
#include <string>

namespace greedybreaker {

/**
 * @brief Representation of an individual activity interval.
 */
struct Activity {
    int id{0};
    int start{0};
    int finish{0};

    bool operator==(const Activity& other) const = default;

    /**
     * @brief Canonical sorting: earliest finish time, then earliest start time, then smaller ID.
     */
    bool operator<(const Activity& other) const {
        if (finish != other.finish) return finish < other.finish;
        if (start != other.start) return start < other.start;
        return id < other.id;
    }
};

/**
 * @brief Problem candidate parameters for Activity Selection.
 */
struct ActivitySelectionCandidate {
    std::vector<Activity> activities;

    bool operator==(const ActivitySelectionCandidate& other) const = default;
};

/**
 * @brief Activity Selection problem implementation for GreedyBreaker.
 * 
 * Compares the canonical Earliest Finish Time First greedy algorithm against an
 * exact dynamic programming interval scheduling oracle.
 * 
 * Objective: MAXIMIZE total number of mutually compatible, non-overlapping activities.
 */
class ActivitySelectionProblem : public GreedyProblem<ActivitySelectionCandidate, Activity> {
public:
    static constexpr const char* STRATEGY_EARLIEST_FINISH = "earliest_finish_first";

    std::string name() const override;
    ProblemObjective objective() const override;
    std::string default_strategy() const override;
    std::vector<std::string> available_strategies() const override;

    /**
     * @brief Validates candidate activities according to problem constraints.
     * 
     * Rejection rules:
     * - Empty activity set.
     * - Negative start or finish times.
     * - Invalid interval where start >= finish.
     * - Duplicate activity IDs.
     */
    void validate_candidate(const ActivitySelectionCandidate& candidate) const override;

    /**
     * @brief Standard greedy solver: Earliest Finish Time First.
     * 
     * Iteratively selects the compatible activity that finishes earliest.
     */
    Solution<Activity> greedy_solution(
        const ActivitySelectionCandidate& candidate,
        const std::string& strategy = ""
    ) const override;

    /**
     * @brief Exact oracle: Dynamic Programming interval scheduling.
     * 
     * Computes the mathematically optimal maximum subset of mutually compatible activities.
     * Recurrence: OPT(j) = max(OPT(j-1), 1 + OPT(p(j))) where p(j) is latest compatible activity.
     */
    Solution<Activity> optimal_solution(const ActivitySelectionCandidate& candidate) const override;

    /**
     * @brief First-divergence analysis between greedy and optimal decisions.
     */
    DivergenceResult analyze_divergence(
        const ActivitySelectionCandidate& candidate,
        const Solution<Activity>& greedy,
        const Solution<Activity>& optimal
    ) const override;

    ComplexityReport complexity_info() const override;
};

} // namespace greedybreaker
