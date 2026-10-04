#pragma once

#include "greedybreaker/search/search_config.hpp"
#include "greedybreaker/problems/coin_change.hpp"
#include "greedybreaker/problems/activity_selection.hpp"
#include "greedybreaker/problems/knapsack.hpp"
#include <functional>

namespace greedybreaker {

/**
 * @brief Streams Coin Change candidates in deterministic canonical order.
 * @return false if consumer requested early termination.
 */
bool stream_coin_change_candidates(
    const SearchConfig& config,
    const std::function<bool(const CoinChangeCandidate&)>& consumer
);

/**
 * @brief Streams Activity Selection candidates in deterministic canonical order.
 * @return false if consumer requested early termination.
 */
bool stream_activity_candidates(
    const SearchConfig& config,
    const std::function<bool(const ActivitySelectionCandidate&)>& consumer
);

/**
 * @brief Streams Knapsack candidates in deterministic canonical order.
 * @return false if consumer requested early termination.
 */
bool stream_knapsack_candidates(
    const SearchConfig& config,
    const std::function<bool(const KnapsackCandidate&)>& consumer
);

} // namespace greedybreaker
