#pragma once

#include "greedybreaker/core/types.hpp"
#include "greedybreaker/core/metrics.hpp"
#include "greedybreaker/core/trace.hpp"
#include "greedybreaker/core/complexity.hpp"

#include <string>
#include <vector>
#include <optional>

namespace greedybreaker {

/**
 * @brief Universal solution container representing solver output.
 * 
 * @tparam ItemType Type representing items in the solution (e.g. coin denomination, activity, knapsack item).
 */
template <typename ItemType = int>
struct Solution {
    bool success{false};
    std::optional<double> cost;
    std::vector<ItemType> items;
    AlgorithmTrace trace;
};

/**
 * @brief Abstract base class for all optimization problems analyzed by GreedyBreaker.
 * 
 * Encapsulates problem definition, candidate validation, greedy heuristics,
 * exact DP oracles, comparison, first-divergence analysis, and complexity metadata.
 * 
 * @tparam CandidateType Problem-specific input representation.
 * @tparam ItemType Concrete item chosen in the solution.
 */
template <typename CandidateType, typename ItemType = int>
class GreedyProblem {
public:
    virtual ~GreedyProblem() = default;

    virtual std::string name() const = 0;
    virtual ProblemObjective objective() const = 0;
    virtual std::string default_strategy() const = 0;
    virtual std::vector<std::string> available_strategies() const = 0;

    virtual void validate_candidate(const CandidateType& candidate) const = 0;
    virtual Solution<ItemType> greedy_solution(const CandidateType& candidate, const std::string& strategy = "") const = 0;
    virtual Solution<ItemType> optimal_solution(const CandidateType& candidate) const = 0;

    virtual ComparisonResult compare_solutions(
        const Solution<ItemType>& greedy,
        const Solution<ItemType>& optimal
    ) const {
        return calculate_comparison(
            objective(),
            greedy.success,
            optimal.success,
            greedy.cost,
            optimal.cost
        );
    }

    virtual DivergenceResult analyze_divergence(
        const CandidateType& candidate,
        const Solution<ItemType>& greedy,
        const Solution<ItemType>& optimal
    ) const = 0;

    virtual ComplexityReport complexity_info() const = 0;
};

} // namespace greedybreaker
