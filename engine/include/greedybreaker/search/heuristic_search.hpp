#pragma once

#include "greedybreaker/search/search_engine.hpp"

namespace greedybreaker {

/**
 * @brief Deterministic heuristic search prioritizing non-canonical remainder traps.
 * 
 * Explores candidate configurations based on the Kozen-Zaks theorem for greedy change-making failure:
 * Identifies denomination pairs (a, b) where a < b < 2*a with target T = 2*a.
 * Under this structure, an optimal solver achieves cost 2 (a + a = 2*a), but greedy greedily
 * selects b (since b > a), leaving an inefficient remainder (2*a - b) that requires extra coins.
 */
class HeuristicSearch : public SearchEngine {
public:
    SearchResult search(const SearchConfig& config) override;
};

} // namespace greedybreaker
