#pragma once

#include "greedybreaker/search/search_engine.hpp"

namespace greedybreaker {

/**
 * @brief Backtracking search engine with incremental candidate construction and branch pruning.
 * 
 * Implemented pruning criteria:
 * 1. Single-coin / single-item pruning: |D|=1 can never fail greedy optimality.
 * 2. Target bound pruning: If smallest item exceeds capacity/target, subtree is pruned.
 * 3. Capacity pruning: When sum of all item weights <= capacity, greedy takes all items (provably optimal).
 * 4. GCD Reachability pruning: If target % gcd(d_1..d_k) != 0, target is unreachable.
 * 5. Non-overlapping activity pruning: If all activities are mutually disjoint, greedy takes all (provably optimal).
 */
class BacktrackingSearch : public SearchEngine {
public:
    SearchResult search(const SearchConfig& config) override;

private:
    bool backtrack_coin(
        const SearchConfig& config,
        const CoinChangeProblem& problem,
        int target,
        int target_k,
        int next_val,
        std::vector<int>& current_denoms,
        SearchResult& result
    );

    bool backtrack_knapsack(
        const SearchConfig& config,
        const Knapsack01Problem& problem,
        int capacity,
        int target_k,
        size_t next_item_idx,
        const std::vector<std::pair<int, int>>& item_pool,
        std::vector<KnapsackItem>& current_items,
        int current_weight_sum,
        SearchResult& result
    );

    bool backtrack_fractional(
        const SearchConfig& config,
        const FractionalKnapsackProblem& problem,
        int capacity,
        int target_k,
        size_t next_item_idx,
        const std::vector<std::pair<int, int>>& item_pool,
        std::vector<KnapsackItem>& current_items,
        int current_weight_sum,
        SearchResult& result
    );

    bool backtrack_activity(
        const SearchConfig& config,
        const ActivitySelectionProblem& problem,
        int target_k,
        size_t next_interval_idx,
        const std::vector<std::pair<int, int>>& interval_pool,
        std::vector<Activity>& current_activities,
        SearchResult& result
    );
};

} // namespace greedybreaker
