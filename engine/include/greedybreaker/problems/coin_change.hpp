#pragma once

#include "greedybreaker/core/problem.hpp"
#include <vector>
#include <string>

namespace greedybreaker {

/**
 * @brief Problem candidate parameters for Coin Change.
 */
struct CoinChangeCandidate {
    std::vector<int> denominations;
    int target{0};
};

/**
 * @brief Coin Change problem implementation for GreedyBreaker.
 * 
 * Analyzes the Largest Denomination First greedy heuristic against the
 * exact dynamic programming coin change oracle.
 */
class CoinChangeProblem : public GreedyProblem<CoinChangeCandidate, int> {
public:
    static constexpr const char* STRATEGY_LARGEST_FIRST = "largest_first";

    std::string name() const override;
    ProblemObjective objective() const override;
    std::string default_strategy() const override;
    std::vector<std::string> available_strategies() const override;

    void validate_candidate(const CoinChangeCandidate& candidate) const override;
    Solution<int> greedy_solution(const CoinChangeCandidate& candidate, const std::string& strategy = "") const override;
    Solution<int> optimal_solution(const CoinChangeCandidate& candidate) const override;
    DivergenceResult analyze_divergence(
        const CoinChangeCandidate& candidate,
        const Solution<int>& greedy,
        const Solution<int>& optimal
    ) const override;
    ComplexityReport complexity_info() const override;
};

} // namespace greedybreaker
