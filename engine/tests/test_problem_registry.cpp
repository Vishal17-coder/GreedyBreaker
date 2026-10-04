#include <gtest/gtest.h>
#include "greedybreaker/core/registry.hpp"
#include "greedybreaker/core/metrics.hpp"
#include "greedybreaker/search/exhaustive_search.hpp"

using namespace greedybreaker;

TEST(ProblemRegistryTest, RegistrySlugAndStringMapping) {
    EXPECT_EQ(to_string(ProblemType::COIN_CHANGE), "Coin Change");
    EXPECT_EQ(to_string(ProblemType::ACTIVITY_SELECTION), "Activity Selection");
    EXPECT_EQ(to_string(ProblemType::KNAPSACK_01), "0/1 Knapsack");
    EXPECT_EQ(to_string(ProblemType::KNAPSACK_FRACTIONAL), "Fractional Knapsack");

    EXPECT_EQ(to_slug(ProblemType::COIN_CHANGE), "coin-change");
    EXPECT_EQ(to_slug(ProblemType::ACTIVITY_SELECTION), "activity-selection");
    EXPECT_EQ(to_slug(ProblemType::KNAPSACK_01), "knapsack-01");
    EXPECT_EQ(to_slug(ProblemType::KNAPSACK_FRACTIONAL), "knapsack-fractional");

    EXPECT_EQ(parse_problem_type("coin-change"), ProblemType::COIN_CHANGE);
    EXPECT_EQ(parse_problem_type("activity-selection"), ProblemType::ACTIVITY_SELECTION);
    EXPECT_EQ(parse_problem_type("knapsack-01"), ProblemType::KNAPSACK_01);
    EXPECT_EQ(parse_problem_type("knapsack-fractional"), ProblemType::KNAPSACK_FRACTIONAL);
}

TEST(GenericMetricsTest, MinimizationVsMaximizationFormulation) {
    // Minimization: greedy 3, optimal 2 -> gap = +1, ratio = 1.5x
    auto min_res = calculate_comparison(ProblemObjective::Minimize, true, true, 3.0, 2.0);
    EXPECT_TRUE(min_res.is_counterexample);
    EXPECT_DOUBLE_EQ(min_res.absolute_gap.value(), 1.0);
    EXPECT_DOUBLE_EQ(min_res.ratio.value(), 1.5);

    // Maximization: greedy 160, optimal 220 -> gap = 60, ratio = 220/160 = 1.375x
    auto max_res = calculate_comparison(ProblemObjective::Maximize, true, true, 160.0, 220.0);
    EXPECT_TRUE(max_res.is_counterexample);
    EXPECT_DOUBLE_EQ(max_res.absolute_gap.value(), 60.0);
    EXPECT_DOUBLE_EQ(max_res.ratio.value(), 220.0 / 160.0);
}

TEST(GenericFrameworkTest, CrossProblemSearchEngineReusability) {
    ExhaustiveSearch engine;

    // 1. Coin Change
    SearchConfig coin_cfg;
    coin_cfg.problem_type = ProblemType::COIN_CHANGE;
    coin_cfg.min_target = 6;
    coin_cfg.max_target = 6;
    coin_cfg.min_denomination = 1;
    coin_cfg.max_denomination = 4;
    coin_cfg.min_coin_count = 3;
    coin_cfg.max_coin_count = 3;
    auto coin_res = engine.search(coin_cfg);
    EXPECT_TRUE(coin_res.found);

    // 2. 0/1 Knapsack
    SearchConfig knap_cfg;
    knap_cfg.problem_type = ProblemType::KNAPSACK_01;
    knap_cfg.min_capacity = 5;
    knap_cfg.max_capacity = 5;
    auto knap_res = engine.search(knap_cfg);
    EXPECT_TRUE(knap_res.found);

    // 3. Activity Selection
    SearchConfig act_cfg;
    act_cfg.problem_type = ProblemType::ACTIVITY_SELECTION;
    act_cfg.min_time = 0;
    act_cfg.max_time = 5;
    act_cfg.min_activities = 3;
    act_cfg.max_activities = 3;
    auto act_res = engine.search(act_cfg);
    EXPECT_FALSE(act_res.found);
    EXPECT_EQ(act_res.counterexamples_found, 0);
}
