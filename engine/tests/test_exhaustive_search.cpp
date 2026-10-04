#include <gtest/gtest.h>
#include "greedybreaker/search/exhaustive_search.hpp"

using namespace greedybreaker;

class ExhaustiveSearchTest : public ::testing::Test {
protected:
    ExhaustiveSearch engine;
};

TEST_F(ExhaustiveSearchTest, AnyModeDiscoversCounterexample) {
    SearchConfig config;
    config.strategy = SearchStrategy::EXHAUSTIVE;
    config.mode = SearchMode::ANY;
    config.min_denomination = 1;
    config.max_denomination = 6;
    config.min_coin_count = 3;
    config.max_coin_count = 3;
    config.min_target = 1;
    config.max_target = 10;

    auto result = engine.search(config);

    EXPECT_TRUE(result.found);
    EXPECT_GT(result.candidates_evaluated, 0);
    EXPECT_GT(result.counterexamples_found, 0);
    ASSERT_TRUE(result.best_candidate.has_value());
    ASSERT_TRUE(result.greedy_solution.has_value());
    ASSERT_TRUE(result.optimal_solution.has_value());
    ASSERT_TRUE(result.comparison_metrics.has_value());

    EXPECT_TRUE(result.comparison_metrics->is_counterexample);
    if (result.greedy_solution->cost.has_value()) {
        EXPECT_GT(result.greedy_solution->cost.value(), result.optimal_solution->cost.value());
    } else {
        EXPECT_FALSE(result.greedy_solution->success);
        EXPECT_TRUE(result.optimal_solution->success);
    }
    ASSERT_TRUE(result.first_divergence.has_value());
    EXPECT_TRUE(result.first_divergence->has_diverged);
}

TEST_F(ExhaustiveSearchTest, SmallestModeFindsCanonicalSmallest) {
    SearchConfig config;
    config.strategy = SearchStrategy::EXHAUSTIVE;
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

    // Verified smallest under (target, k, lexicographical) is target 6, coins {1, 3, 4}
    EXPECT_EQ(result.best_candidate->target, 6);
    std::vector<int> expected_coins = {1, 3, 4};
    EXPECT_EQ(result.best_candidate->denominations, expected_coins);

    EXPECT_DOUBLE_EQ(result.greedy_solution->cost.value(), 3.0);
    EXPECT_DOUBLE_EQ(result.optimal_solution->cost.value(), 2.0);
    EXPECT_DOUBLE_EQ(result.comparison_metrics->ratio.value(), 1.50);
}

TEST_F(ExhaustiveSearchTest, WorstModeMaximizesSeverity) {
    SearchConfig config;
    config.strategy = SearchStrategy::EXHAUSTIVE;
    config.mode = SearchMode::WORST;
    config.min_denomination = 1;
    config.max_denomination = 9;
    config.min_coin_count = 3;
    config.max_coin_count = 3;
    config.min_target = 6;
    config.max_target = 12;

    auto result = engine.search(config);

    EXPECT_TRUE(result.found);
    EXPECT_TRUE(result.search_completed);
    EXPECT_FALSE(result.search_limit_reached);
    ASSERT_TRUE(result.best_candidate.has_value());
    ASSERT_TRUE(result.comparison_metrics.has_value());

    // In this space, {1, 6, 9}, target 12 yields greedy=4, optimal=2, ratio=2.0x, gap=2
    EXPECT_GE(result.comparison_metrics->ratio.value(), 1.50);
    EXPECT_GE(result.comparison_metrics->absolute_gap.value(), 1.0);
}

TEST_F(ExhaustiveSearchTest, DeterministicReproduction) {
    SearchConfig config;
    config.min_denomination = 1;
    config.max_denomination = 6;
    config.min_coin_count = 2;
    config.max_coin_count = 3;
    config.min_target = 1;
    config.max_target = 10;
    config.mode = SearchMode::ANY;

    auto r1 = engine.search(config);
    auto r2 = engine.search(config);

    EXPECT_EQ(r1.found, r2.found);
    EXPECT_EQ(r1.candidates_evaluated, r2.candidates_evaluated);
    EXPECT_EQ(r1.best_candidate->target, r2.best_candidate->target);
    EXPECT_EQ(r1.best_candidate->denominations, r2.best_candidate->denominations);
}

TEST_F(ExhaustiveSearchTest, NoCounterexampleWhenSpaceCanonical) {
    SearchConfig config;
    config.min_denomination = 1;
    config.max_denomination = 2; // only {1, 2} possible
    config.min_coin_count = 2;
    config.max_coin_count = 2;
    config.min_target = 1;
    config.max_target = 10;

    auto result = engine.search(config);

    // {1, 2} is canonical (greedy is always optimal)
    EXPECT_FALSE(result.found);
    EXPECT_EQ(result.counterexamples_found, 0);
    EXPECT_TRUE(result.search_completed);
}
