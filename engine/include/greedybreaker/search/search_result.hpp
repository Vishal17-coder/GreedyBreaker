#pragma once

#include "greedybreaker/search/search_types.hpp"
#include "greedybreaker/core/registry.hpp"
#include "greedybreaker/core/metrics.hpp"
#include "greedybreaker/core/trace.hpp"
#include "greedybreaker/problems/coin_change.hpp"
#include "greedybreaker/problems/activity_selection.hpp"
#include "greedybreaker/problems/knapsack.hpp"
#include <optional>
#include <string>
#include <cstddef>
#include <cstdint>

namespace greedybreaker {

/**
 * @brief Complete telemetry and verified counterexample outcome from a search execution.
 */
struct SearchResult {
    bool found{false};
    SearchMode mode{SearchMode::ANY};
    SearchStrategy strategy{SearchStrategy::EXHAUSTIVE};
    ProblemType problem_type{ProblemType::COIN_CHANGE};

    // Telemetry
    size_t candidates_generated{0};
    size_t candidates_evaluated{0};
    size_t counterexamples_found{0};
    size_t nodes_explored{0};
    size_t branches_pruned{0};

    // Shared / Coin Change counterexample details (retained for backward compatibility)
    std::optional<CoinChangeCandidate> best_candidate;
    std::optional<Solution<int>> greedy_solution;
    std::optional<Solution<int>> optimal_solution;
    std::optional<ComparisonResult> comparison_metrics;
    std::optional<DivergenceResult> first_divergence;

    // Activity Selection specific payloads
    std::optional<ActivitySelectionCandidate> best_activity_candidate;
    std::optional<Solution<Activity>> greedy_activity_solution;
    std::optional<Solution<Activity>> optimal_activity_solution;

    // Knapsack specific payloads
    std::optional<KnapsackCandidate> best_knapsack_candidate;
    std::optional<Solution<KnapsackItem>> greedy_knapsack_solution;
    std::optional<Solution<KnapsackItem>> optimal_knapsack_solution;
    std::optional<Solution<FractionalKnapsackItem>> greedy_fractional_solution;
    std::optional<Solution<FractionalKnapsackItem>> optimal_fractional_solution;

    // Canonical human-readable summary of the discovered candidate instance
    std::string candidate_summary;

    // Search completion status
    bool search_completed{false};
    bool search_limit_reached{false};
    std::optional<uint32_t> random_seed;
    size_t iterations{0};
    double duration_ms{0.0};
    std::string termination_reason;
    std::string search_space_description;

    // Minimality flag: true ONLY if search ordering and exhaustiveness establish minimality within bounds
    bool smallest_verified{false};
};

} // namespace greedybreaker
