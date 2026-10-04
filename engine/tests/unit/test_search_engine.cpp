#include <gtest/gtest.h>
#include "greedybreaker/search/search_engine.hpp"
#include "greedybreaker/search/search_space.hpp"
#include "greedybreaker/problems/coin_change.hpp"
#include "greedybreaker/problems/knapsack_01.hpp"

using namespace greedybreaker::search;
using namespace greedybreaker::core;

TEST(SearchEngineTest, CoinChangeExhaustiveDiscovery) {
    SearchConfig cfg;
    cfg.experiment_id = "test_coin_change_exhaustive";
    cfg.problem_type = "coin_change";
    cfg.strategy_id = "largest_denomination_first";
    cfg.mode = SearchMode::ANY;
    cfg.strategy = SearchStrategy::EXHAUSTIVE;
    cfg.max_candidates = 500;
    cfg.max_time_ms = 2000;

    CoinChangeSearchSpace space;
    space.denomination_count = {3, 3};
    space.denomination_range = {1, 10};
    space.target_range = {1, 15};
    cfg.search_space = space;

    auto res = SearchEngine::run(cfg);
    EXPECT_TRUE(res.status.ok());
    EXPECT_TRUE(res.has_counterexample);
    EXPECT_GT(res.instances_evaluated, 0);
    EXPECT_GT(res.counterexamples_found, 0);

    // Verify discovered instance is indeed a valid counterexample
    auto inst = res.best_instance.get<greedybreaker::problems::CoinChangeInstance>();
    auto greedy = greedybreaker::problems::CoinChangeProblem::solve_greedy(inst);
    auto oracle = greedybreaker::problems::CoinChangeProblem::solve_exact_dp(inst);
    auto ver = verify(ObjectiveDirection::MINIMIZE, greedy, oracle);
    EXPECT_TRUE(ver.is_counterexample);
}

TEST(SearchEngineTest, CoinChangeProfileMode) {
    SearchConfig cfg;
    cfg.experiment_id = "test_coin_profile";
    cfg.problem_type = "coin_change";
    cfg.mode = SearchMode::PROFILE;
    cfg.strategy = SearchStrategy::GUIDED;
    cfg.max_candidates = 50;

    CoinChangeSearchSpace space;
    space.denomination_count = {3, 3};
    space.denomination_range = {1, 15};
    space.target_range = {1, 30};
    cfg.search_space = space;

    auto res = SearchEngine::run(cfg);
    EXPECT_TRUE(res.status.ok());
    EXPECT_GT(res.instances_evaluated, 0);
    EXPECT_GT(res.counterexamples_found, 0);
    EXPECT_GT(res.failure_rate, 0.0);
    EXPECT_GT(res.average_gap, 0.0);
}

TEST(SearchEngineTest, Knapsack01DiscoversFailureWhileFractionalFindsZero) {
    SearchConfig cfg_01;
    cfg_01.experiment_id = "knapsack_01_search";
    cfg_01.problem_type = "knapsack_01";
    cfg_01.strategy_id = "value_density_descending";
    cfg_01.mode = SearchMode::ANY;
    cfg_01.strategy = SearchStrategy::GUIDED;
    cfg_01.max_candidates = 20;

    auto res_01 = SearchEngine::run(cfg_01);
    EXPECT_TRUE(res_01.status.ok());
    EXPECT_TRUE(res_01.has_counterexample);

    // Contrast with Fractional Knapsack under the exact same density strategy
    SearchConfig cfg_frac = cfg_01;
    cfg_frac.problem_type = "fractional_knapsack";
    auto res_frac = SearchEngine::run(cfg_frac);
    EXPECT_FALSE(res_frac.has_counterexample);
    EXPECT_EQ(res_frac.counterexamples_found, 0);
    EXPECT_EQ(res_frac.status.code(), ErrorCode::NO_COUNTEREXAMPLE_FOUND);
}

TEST(SearchEngineTest, ActivitySelectionInvarianceTest) {
    // Optimal strategy: Earliest Finish Time must produce 0 counterexamples
    SearchConfig cfg;
    cfg.experiment_id = "activity_invariance";
    cfg.problem_type = "activity_selection";
    cfg.strategy_id = "earliest_finish_time";
    cfg.mode = SearchMode::PROFILE;
    cfg.strategy = SearchStrategy::RANDOMIZED;
    cfg.random_seed = 12345;
    cfg.max_candidates = 100;

    auto res = SearchEngine::run(cfg);
    EXPECT_EQ(res.counterexamples_found, 0);
    EXPECT_FALSE(res.has_counterexample);
    EXPECT_EQ(res.status.code(), ErrorCode::NO_COUNTEREXAMPLE_FOUND);
}

TEST(SearchEngineTest, ReproducibilityWithIdenticalSeed) {
    SearchConfig cfg1;
    cfg1.experiment_id = "repro_test";
    cfg1.problem_type = "coin_change";
    cfg1.mode = SearchMode::ANY;
    cfg1.strategy = SearchStrategy::RANDOMIZED;
    cfg1.random_seed = 987654321ULL;
    cfg1.max_candidates = 100;

    SearchConfig cfg2 = cfg1;

    auto res1 = SearchEngine::run(cfg1);
    auto res2 = SearchEngine::run(cfg2);

    EXPECT_EQ(res1.instances_evaluated, res2.instances_evaluated);
    EXPECT_EQ(res1.counterexamples_found, res2.counterexamples_found);
    EXPECT_EQ(res1.has_counterexample, res2.has_counterexample);
    if (res1.has_counterexample) {
        EXPECT_EQ(res1.best_instance, res2.best_instance);
        EXPECT_DOUBLE_EQ(res1.best_verification.absolute_gap, res2.best_verification.absolute_gap);
    }
}
