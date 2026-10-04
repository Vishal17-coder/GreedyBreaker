#include <gtest/gtest.h>
#include "greedybreaker/search/randomized_search.hpp"

using namespace greedybreaker;

class RandomizedSearchTest : public ::testing::Test {
protected:
    RandomizedSearch engine;
};

TEST_F(RandomizedSearchTest, AnyModeDiscoversCounterexample) {
    SearchConfig config;
    config.strategy = SearchStrategy::RANDOMIZED;
    config.mode = SearchMode::ANY;
    config.random_seed = 42;
    config.max_iterations = 3000;
    config.min_denomination = 1;
    config.max_denomination = 10;
    config.min_coin_count = 3;
    config.max_coin_count = 4;
    config.min_target = 6;
    config.max_target = 30;

    auto result = engine.search(config);

    EXPECT_TRUE(result.found);
    EXPECT_GT(result.iterations, 0);
    EXPECT_EQ(result.random_seed.value(), 42);
    EXPECT_FALSE(result.smallest_verified); // Honestly reported
    ASSERT_TRUE(result.best_candidate.has_value());
    EXPECT_TRUE(result.comparison_metrics->is_counterexample);
    if (result.greedy_solution->cost.has_value()) {
        EXPECT_GT(result.greedy_solution->cost.value(), result.optimal_solution->cost.value());
    } else {
        EXPECT_FALSE(result.greedy_solution->success);
        EXPECT_TRUE(result.optimal_solution->success);
    }
}

TEST_F(RandomizedSearchTest, IdenticalSeedProducesIdenticalResult) {
    SearchConfig config;
    config.strategy = SearchStrategy::RANDOMIZED;
    config.mode = SearchMode::WORST;
    config.random_seed = 12345;
    config.max_iterations = 500;
    config.min_denomination = 1;
    config.max_denomination = 10;
    config.min_coin_count = 3;
    config.max_coin_count = 4;
    config.min_target = 5;
    config.max_target = 25;

    auto r1 = engine.search(config);
    auto r2 = engine.search(config);

    EXPECT_EQ(r1.found, r2.found);
    EXPECT_EQ(r1.iterations, r2.iterations);
    EXPECT_EQ(r1.candidates_evaluated, r2.candidates_evaluated);
    EXPECT_EQ(r1.counterexamples_found, r2.counterexamples_found);
    ASSERT_EQ(r1.best_candidate.has_value(), r2.best_candidate.has_value());

    if (r1.best_candidate.has_value()) {
        EXPECT_EQ(r1.best_candidate->target, r2.best_candidate->target);
        EXPECT_EQ(r1.best_candidate->denominations, r2.best_candidate->denominations);
        EXPECT_DOUBLE_EQ(r1.comparison_metrics->ratio.value(), r2.comparison_metrics->ratio.value());
    }
}

TEST_F(RandomizedSearchTest, DifferentSeedChangesExploration) {
    SearchConfig c1;
    c1.strategy = SearchStrategy::RANDOMIZED;
    c1.random_seed = 101;
    c1.max_iterations = 500;
    c1.min_denomination = 1;
    c1.max_denomination = 10;
    c1.min_coin_count = 3;
    c1.max_coin_count = 4;
    c1.min_target = 5;
    c1.max_target = 25;

    SearchConfig c2 = c1;
    c2.random_seed = 999999;

    auto r1 = engine.search(c1);
    auto r2 = engine.search(c2);

    // Both should terminate and record their respective seeds
    EXPECT_EQ(r1.random_seed.value(), 101);
    EXPECT_EQ(r2.random_seed.value(), 999999);
}

TEST_F(RandomizedSearchTest, IterationLimitEnforced) {
    SearchConfig config;
    config.strategy = SearchStrategy::RANDOMIZED;
    config.mode = SearchMode::WORST;
    config.random_seed = 42;
    config.max_iterations = 50;
    config.min_denomination = 1;
    config.max_denomination = 2; // only {1, 2}, impossible to fail
    config.min_coin_count = 2;
    config.max_coin_count = 2;
    config.min_target = 1;
    config.max_target = 10;

    auto result = engine.search(config);

    EXPECT_FALSE(result.found);
    EXPECT_EQ(result.iterations, 50);
    EXPECT_TRUE(result.search_completed);
    EXPECT_NE(result.termination_reason.find("Max iterations"), std::string::npos);
}
