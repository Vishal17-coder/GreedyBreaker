#pragma once

#include "greedybreaker/search/search_engine.hpp"

namespace greedybreaker {

/**
 * @brief Deterministic exhaustive search enumerating all candidates in canonical order.
 * 
 * Enumeration sequence:
 * 1. Target ascending [min_target .. max_target]
 * 2. Coin count ascending [min_coin_count .. max_coin_count]
 * 3. Denominations lexicographically ascending
 */
class ExhaustiveSearch : public SearchEngine {
public:
    SearchResult search(const SearchConfig& config) override;
};

} // namespace greedybreaker
