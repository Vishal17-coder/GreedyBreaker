#pragma once

#include "greedybreaker/search/search_types.hpp"
#include "greedybreaker/core/types.hpp"
#include "greedybreaker/core/registry.hpp"
#include <functional>
#include <cstdint>
#include <cstddef>
#include <string>

namespace greedybreaker {

/**
 * @brief Configuration parameters and safety limits for counterexample search.
 */
struct SearchConfig {
    ProblemType problem_type{ProblemType::COIN_CHANGE};
    SearchStrategy strategy{SearchStrategy::EXHAUSTIVE};
    SearchMode mode{SearchMode::ANY};

    // Coin Change search bounds
    int min_denomination{1};
    int max_denomination{10};
    int min_coin_count{2};
    int max_coin_count{4};
    int min_target{1};
    int max_target{30};
    bool require_unit_coin{true}; ///< If true, ensures denomination 1 is included (solvability invariant)

    // Activity Selection search bounds
    int min_activities{3};
    int max_activities{3};
    int min_time{0};
    int max_time{8};
    int min_duration{1};

    // Knapsack (0/1 and Fractional) search bounds
    int min_items{3};
    int max_items{5};
    int min_weight{1};
    int max_weight{10};
    int min_value{1};
    int max_value{20};
    int min_capacity{5};
    int max_capacity{20};

    // Randomized search parameters
    uint32_t random_seed{42};
    size_t max_iterations{10000};

    // Global safety limits
    size_t max_candidates{50000};

    // Cooperative cancellation hook
    std::function<bool()> should_cancel{nullptr};

    /**
     * @brief Validates configuration parameters and boundaries.
     * 
     * @throws InvalidCandidateException if bounds are inverted or invalid.
     */
    void validate() const;

    /**
     * @brief Generates a human-readable summary of the configured search space.
     */
    std::string describe_space() const;
};

} // namespace greedybreaker
