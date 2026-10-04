#include <gtest/gtest.h>
#include "greedybreaker/search/backtracking_search.hpp"

using namespace greedybreaker;

class BacktrackingSearchTest : public ::testing::Test {
protected:
    BacktrackingSearch engine;
};

TEST_F(BacktrackingSearchTest, AnyModeDiscoversCounterexample) {
    SearchConfig config;
    config.strategy = SearchStrategy::BACKTRACKING;
    config.mode = SearchMode::ANY;
    config.min_denomination = 1;
    config.max_denomination = 6;
    config.min_coin_count = 3;
    config.max_coin_count = 3;
    config.min_target = 1;
    config.max_target = 10;

    auto result = engine.search(config);

    EXPECT_TRUE(result.found);
    EXPECT_GT(result.nodes_explored, 0);
    EXPECT_GT(result.branches_pruned, 0);
    ASSERT_TRUE(result.best_candidate.has_value());
    EXPECT_TRUE(result.comparison_metrics->is_counterexample);
    if (result.greedy_solution->cost.has_value()) {
        EXPECT_GT(result.greedy_solution->cost.value(), result.optimal_solution->cost.value());
    } else {
        EXPECT_FALSE(result.greedy_solution->success);
        EXPECT_TRUE(result.optimal_solution->success);
    }
}

TEST_F(BacktrackingSearchTest, SmallestModeFindsCanonicalSmallest) {
    SearchConfig config;
    config.strategy = SearchStrategy::BACKTRACKING;
    config.mode = SearchMode::SMALLEST;
    config.min_denomination = 1;
    config.max_denomination = 6;
    config.min_coin_count = 3;
    config.max_coin_count = 3;
    config.min_target = 1;
    config.max_target = 15;

    auto result = engine.search(config);

    EXPECT_TRUE(result.found);
    EXPECT_TRUE(result.smallest_verified);
    ASSERT_TRUE(result.best_candidate.has_value());
    EXPECT_EQ(result.best_candidate->target, 6);
    std::vector<int> expected_coins = {1, 3, 4};
    EXPECT_EQ(result.best_candidate->denominations, expected_coins);
    EXPECT_DOUBLE_EQ(result.greedy_solution->cost.value(), 3.0);
    EXPECT_DOUBLE_EQ(result.optimal_solution->cost.value(), 2.0);
}

TEST_F(BacktrackingSearchTest, PruningActiveDuringExploration) {
    SearchConfig config;
    config.strategy = SearchStrategy::BACKTRACKING;
    config.mode = SearchMode::WORST;
    config.min_denomination = 1;
    config.max_denomination = 8;
    config.min_coin_count = 2;
    config.max_coin_count = 3;
    config.min_target = 1;
    config.max_target = 10;

    auto result = engine.search(config);

    EXPECT_TRUE(result.found);
    EXPECT_GT(result.branches_pruned, 0);
    EXPECT_GT(result.nodes_explored, result.candidates_evaluated);
}

TEST_F(BacktrackingSearchTest, SafetyLimitEnforced) {
    SearchConfig config;
    config.strategy = SearchStrategy::BACKTRACKING;
    config.min_denomination = 1;
    config.max_denomination = 10;
    config.min_coin_count = 2;
    config.max_coin_count = 4;
    config.min_target = 1;
    config.max_target = 30;
    config.max_candidates = 8;
    config.mode = SearchMode::WORST;

    auto result = engine.search(config);

    EXPECT_TRUE(result.search_limit_reached);
    EXPECT_LE(result.candidates_evaluated, 8);
}
