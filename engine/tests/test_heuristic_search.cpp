#include <gtest/gtest.h>
#include "greedybreaker/search/heuristic_search.hpp"

using namespace greedybreaker;

class HeuristicSearchTest : public ::testing::Test {
protected:
    HeuristicSearch engine;
};

TEST_F(HeuristicSearchTest, AnyModeDiscoversCounterexampleRapidly) {
    SearchConfig config;
    config.strategy = SearchStrategy::HEURISTIC;
    config.mode = SearchMode::ANY;
    config.min_denomination = 1;
    config.max_denomination = 10;
    config.min_coin_count = 3;
    config.max_coin_count = 3;
    config.min_target = 1;
    config.max_target = 30;

    auto result = engine.search(config);

    EXPECT_TRUE(result.found);
    // Heuristic tests Kozen-Zaks double-coin traps first, finding failure in very few trials
    EXPECT_LE(result.candidates_evaluated, 50);
    EXPECT_FALSE(result.smallest_verified); // Honestly reported
    ASSERT_TRUE(result.best_candidate.has_value());
    ASSERT_TRUE(result.comparison_metrics.has_value());
    EXPECT_TRUE(result.comparison_metrics->is_counterexample);
    ASSERT_TRUE(result.first_divergence.has_value());
    EXPECT_TRUE(result.first_divergence->has_diverged);
}

TEST_F(HeuristicSearchTest, DeterministicReproduction) {
    SearchConfig config;
    config.strategy = SearchStrategy::HEURISTIC;
    config.mode = SearchMode::WORST;
    config.min_denomination = 1;
    config.max_denomination = 8;
    config.min_coin_count = 3;
    config.max_coin_count = 3;
    config.min_target = 1;
    config.max_target = 20;

    auto r1 = engine.search(config);
    auto r2 = engine.search(config);

    EXPECT_EQ(r1.found, r2.found);
    EXPECT_EQ(r1.candidates_evaluated, r2.candidates_evaluated);
    ASSERT_EQ(r1.best_candidate.has_value(), r2.best_candidate.has_value());
    EXPECT_EQ(r1.best_candidate->target, r2.best_candidate->target);
    EXPECT_EQ(r1.best_candidate->denominations, r2.best_candidate->denominations);
    EXPECT_DOUBLE_EQ(r1.comparison_metrics->ratio.value(), r2.comparison_metrics->ratio.value());
}

TEST_F(HeuristicSearchTest, VerificationPipelineFullIntegrity) {
    SearchConfig config;
    config.strategy = SearchStrategy::HEURISTIC;
    config.mode = SearchMode::ANY;
    config.min_denomination = 1;
    config.max_denomination = 10;
    config.min_coin_count = 3;
    config.max_coin_count = 3;
    config.min_target = 5;
    config.max_target = 20;

    auto result = engine.search(config);

    EXPECT_TRUE(result.found);
    ASSERT_TRUE(result.greedy_solution.has_value());
    ASSERT_TRUE(result.optimal_solution.has_value());
    ASSERT_TRUE(result.comparison_metrics.has_value());
    ASSERT_TRUE(result.first_divergence.has_value());

    EXPECT_GT(result.greedy_solution->cost.value(), result.optimal_solution->cost.value());
    EXPECT_GT(result.comparison_metrics->absolute_gap.value(), 0.0);
    EXPECT_GT(result.comparison_metrics->ratio.value(), 1.0);
    EXPECT_FALSE(result.first_divergence->explanation.empty());
}
