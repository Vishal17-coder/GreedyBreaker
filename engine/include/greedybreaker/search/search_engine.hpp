#pragma once

#include "greedybreaker/search/search_config.hpp"
#include "greedybreaker/search/search_result.hpp"
#include "greedybreaker/problems/coin_change.hpp"
#include "greedybreaker/problems/activity_selection.hpp"
#include "greedybreaker/problems/knapsack.hpp"
#include <memory>

namespace greedybreaker {

/**
 * @brief Evaluated instance outcome produced by the verification pipeline for Coin Change.
 */
struct CandidateEvaluation {
    bool is_counterexample{false};
    Solution<int> greedy;
    Solution<int> optimal;
    ComparisonResult comparison;
    std::optional<DivergenceResult> divergence;
};

/**
 * @brief Evaluated instance outcome produced by the verification pipeline for Activity Selection.
 */
struct ActivityCandidateEvaluation {
    bool is_counterexample{false};
    Solution<Activity> greedy;
    Solution<Activity> optimal;
    ComparisonResult comparison;
    std::optional<DivergenceResult> divergence;
};

/**
 * @brief Evaluated instance outcome produced by the verification pipeline for 0/1 Knapsack.
 */
struct KnapsackCandidateEvaluation {
    bool is_counterexample{false};
    Solution<KnapsackItem> greedy;
    Solution<KnapsackItem> optimal;
    ComparisonResult comparison;
    std::optional<DivergenceResult> divergence;
};

/**
 * @brief Evaluated instance outcome produced by the verification pipeline for Fractional Knapsack.
 */
struct FractionalKnapsackCandidateEvaluation {
    bool is_counterexample{false};
    Solution<FractionalKnapsackItem> greedy;
    Solution<FractionalKnapsackItem> optimal;
    ComparisonResult comparison;
    std::optional<DivergenceResult> divergence;
};

/**
 * @brief Canonical ordering comparator for Coin Change candidates.
 */
bool is_candidate_smaller(const CoinChangeCandidate& a, const CoinChangeCandidate& b);

/**
 * @brief Canonical ordering comparator for Activity Selection candidates.
 */
bool is_candidate_smaller(const ActivitySelectionCandidate& a, const ActivitySelectionCandidate& b);

/**
 * @brief Canonical ordering comparator for Knapsack candidates.
 */
bool is_candidate_smaller(const KnapsackCandidate& a, const KnapsackCandidate& b);

/**
 * @brief Failure severity comparator for Coin Change in WORST mode.
 */
bool is_severity_worse(
    const ComparisonResult& a_cmp,
    const CoinChangeCandidate& a_cand,
    const ComparisonResult& b_cmp,
    const CoinChangeCandidate& b_cand
);

/**
 * @brief Failure severity comparator for Knapsack in WORST mode.
 */
bool is_severity_worse(
    const ComparisonResult& a_cmp,
    const KnapsackCandidate& a_cand,
    const ComparisonResult& b_cmp,
    const KnapsackCandidate& b_cand
);

/**
 * @brief Executes the standard verification pipeline for a single Coin Change candidate.
 */
CandidateEvaluation evaluate_candidate(
    const CoinChangeProblem& problem,
    const CoinChangeCandidate& candidate
);

/**
 * @brief Executes the standard verification pipeline for an Activity Selection candidate.
 */
ActivityCandidateEvaluation evaluate_activity_candidate(
    const ActivitySelectionProblem& problem,
    const ActivitySelectionCandidate& candidate
);

/**
 * @brief Executes the standard verification pipeline for a 0/1 Knapsack candidate.
 */
KnapsackCandidateEvaluation evaluate_knapsack_candidate(
    const Knapsack01Problem& problem,
    const KnapsackCandidate& candidate
);

/**
 * @brief Executes the standard verification pipeline for a Fractional Knapsack candidate.
 */
FractionalKnapsackCandidateEvaluation evaluate_fractional_candidate(
    const FractionalKnapsackProblem& problem,
    const KnapsackCandidate& candidate
);

/**
 * @brief Abstract base class for counterexample search engines.
 */
class SearchEngine {
public:
    virtual ~SearchEngine() = default;

    /**
     * @brief Executes counterexample search within configured bounds and mode.
     */
    virtual SearchResult search(const SearchConfig& config) = 0;
};

} // namespace greedybreaker
