#pragma once

#include "greedybreaker/core/problem.hpp"
#include <vector>
#include <string>
#include <cmath>

namespace greedybreaker {

/**
 * @brief Representation of an individual item in a 0/1 Knapsack candidate.
 */
struct KnapsackItem {
    int id{0};
    int value{0};
    int weight{0};

    [[nodiscard]] double ratio() const {
        return weight > 0 ? static_cast<double>(value) / weight : 0.0;
    }

    bool operator==(const KnapsackItem& other) const = default;
};

/**
 * @brief Item representation for Fractional Knapsack, including fraction taken.
 */
struct FractionalKnapsackItem {
    int id{0};
    int value{0};
    int weight{0};
    double fraction{1.0};
    double value_taken{0.0};
    double weight_taken{0.0};

    bool operator==(const FractionalKnapsackItem& other) const = default;
};

/**
 * @brief Candidate input parameters for Knapsack optimization problems.
 */
struct KnapsackCandidate {
    std::vector<KnapsackItem> items;
    int capacity{0};

    bool operator==(const KnapsackCandidate& other) const = default;
};

/**
 * @brief Canonical deterministic comparator for knapsack items sorted by ratio.
 * 
 * Order:
 * 1. Higher value/weight ratio
 * 2. Higher absolute value
 * 3. Lower weight
 * 4. Lower item ID
 */
inline bool knapsack_item_ratio_comparator(const KnapsackItem& a, const KnapsackItem& b) {
    const double r_a = a.ratio();
    const double r_b = b.ratio();
    if (std::abs(r_a - r_b) > 1e-9) {
        return r_a > r_b;
    }
    if (a.value != b.value) {
        return a.value > b.value;
    }
    if (a.weight != b.weight) {
        return a.weight < b.weight;
    }
    return a.id < b.id;
}

/**
 * @brief 0/1 Knapsack problem implementation for GreedyBreaker.
 * 
 * Compares the ratio-based greedy heuristic against the exact pseudo-polynomial
 * dynamic programming oracle O(nW).
 * 
 * Objective: MAXIMIZE total item value without exceeding knapsack capacity W.
 */
class Knapsack01Problem : public GreedyProblem<KnapsackCandidate, KnapsackItem> {
public:
    static constexpr const char* STRATEGY_VALUE_WEIGHT_RATIO = "value_weight_ratio";

    std::string name() const override;
    ProblemObjective objective() const override;
    std::string default_strategy() const override;
    std::vector<std::string> available_strategies() const override;

    /**
     * @brief Validates candidate according to 0/1 knapsack domain constraints.
     * 
     * Rejection rules:
     * - Empty items list.
     * - Non-positive knapsack capacity (W <= 0).
     * - Non-positive item weight (weight <= 0).
     * - Negative item value (value < 0).
     * - Duplicate item IDs.
     */
    void validate_candidate(const KnapsackCandidate& candidate) const override;

    /**
     * @brief Ratio-based greedy solver for 0/1 Knapsack.
     */
    Solution<KnapsackItem> greedy_solution(
        const KnapsackCandidate& candidate,
        const std::string& strategy = ""
    ) const override;

    /**
     * @brief Exact dynamic programming oracle for 0/1 Knapsack.
     * 
     * Recurrence: dp[i][w] = max(dp[i-1][w], value[i-1] + dp[i-1][w - weight[i-1]])
     * Time: O(nW), Space: O(nW).
     */
    Solution<KnapsackItem> optimal_solution(const KnapsackCandidate& candidate) const override;

    /**
     * @brief Pinpoints the earliest decision step where the greedy heuristic diverged
     * from decisions compatible with the optimal subset.
     */
    DivergenceResult analyze_divergence(
        const KnapsackCandidate& candidate,
        const Solution<KnapsackItem>& greedy,
        const Solution<KnapsackItem>& optimal
    ) const override;

    ComplexityReport complexity_info() const override;
};

/**
 * @brief Fractional Knapsack problem implementation for GreedyBreaker.
 * 
 * Demonstrates the mathematically correct domain where ratio-based greedy achieves
 * global optimality.
 * 
 * Objective: MAXIMIZE total item value allowing continuous fractional quantities.
 */
class FractionalKnapsackProblem : public GreedyProblem<KnapsackCandidate, FractionalKnapsackItem> {
public:
    static constexpr const char* STRATEGY_FRACTIONAL_RATIO = "fractional_ratio";

    std::string name() const override;
    ProblemObjective objective() const override;
    std::string default_strategy() const override;
    std::vector<std::string> available_strategies() const override;

    void validate_candidate(const KnapsackCandidate& candidate) const override;

    Solution<FractionalKnapsackItem> greedy_solution(
        const KnapsackCandidate& candidate,
        const std::string& strategy = ""
    ) const override;

    Solution<FractionalKnapsackItem> optimal_solution(
        const KnapsackCandidate& candidate
    ) const override;

    DivergenceResult analyze_divergence(
        const KnapsackCandidate& candidate,
        const Solution<FractionalKnapsackItem>& greedy,
        const Solution<FractionalKnapsackItem>& optimal
    ) const override;

    ComplexityReport complexity_info() const override;
};

} // namespace greedybreaker
