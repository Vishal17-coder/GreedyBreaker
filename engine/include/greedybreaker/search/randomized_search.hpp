#pragma once

#include "greedybreaker/search/search_engine.hpp"

namespace greedybreaker {

/**
 * @brief Pseudo-random candidate search engine using std::mt19937.
 * 
 * Generates valid candidate instances within configured bounds using a deterministic
 * PRNG seed, ensuring complete reproducibility across runs.
 */
class RandomizedSearch : public SearchEngine {
public:
    SearchResult search(const SearchConfig& config) override;
};

} // namespace greedybreaker
