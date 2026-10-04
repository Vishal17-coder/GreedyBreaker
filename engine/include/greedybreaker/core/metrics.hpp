#pragma once

#include "greedybreaker/core/types.hpp"
#include <optional>
#include <string>

namespace greedybreaker {

/**
 * @brief Structured outcome of comparing a greedy solution with an exact optimal solution.
 */
struct ComparisonResult {
    bool is_counterexample{false};
    ProblemObjective objective{ProblemObjective::Minimize};
    bool greedy_solved{false};
    bool optimal_solved{false};
    std::optional<double> greedy_cost;
    std::optional<double> optimal_cost;
    std::optional<double> absolute_gap;
    std::optional<double> ratio;
    std::optional<double> overhead_percentage;
    std::string interpretation;
};

/**
 * @brief Computes mathematically rigorous comparison metrics.
 * 
 * @param objective Optimization direction (Minimize or Maximize).
 * @param greedy_solved Whether greedy found a feasible solution.
 * @param optimal_solved Whether optimal oracle found a feasible solution.
 * @param greedy_cost Solution value from greedy solver.
 * @param optimal_cost Solution value from exact oracle.
 * @return ComparisonResult containing gap, ratio, and overhead percentage.
 */
ComparisonResult calculate_comparison(
    ProblemObjective objective,
    bool greedy_solved,
    bool optimal_solved,
    std::optional<double> greedy_cost,
    std::optional<double> optimal_cost
);

} // namespace greedybreaker
