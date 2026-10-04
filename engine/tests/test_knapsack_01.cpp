#include <gtest/gtest.h>
#include "greedybreaker/problems/knapsack.hpp"
#include "greedybreaker/search/exhaustive_search.hpp"
#include "greedybreaker/search/backtracking_search.hpp"
#include "greedybreaker/search/randomized_search.hpp"
#include "greedybreaker/search/heuristic_search.hpp"

using namespace greedybreaker;

class Knapsack01Test : public ::testing::Test {
protected:
    Knapsack01Problem problem;
};

TEST_F(Knapsack01Test, ValidCandidatePasses) {
    KnapsackCandidate candidate{
        .items = {
            KnapsackItem{.id = 1, .value = 60, .weight = 10},
            KnapsackItem{.id = 2, .value = 100, .weight = 20}
        },
        .capacity = 50
    };
    EXPECT_NO_THROW(problem.validate_candidate(candidate));
}

TEST_F(Knapsack01Test, EmptyItemsThrows) {
    KnapsackCandidate candidate{.items = {}, .capacity = 50};
    EXPECT_THROW(problem.validate_candidate(candidate), InvalidCandidateException);
}

TEST_F(Knapsack01Test, ZeroOrNegativeCapacityThrows) {
    KnapsackCandidate zero_cap{
        .items = {KnapsackItem{.id = 1, .value = 10, .weight = 5}},
        .capacity = 0
    };
    EXPECT_THROW(problem.validate_candidate(zero_cap), InvalidCandidateException);

    KnapsackCandidate neg_cap{
        .items = {KnapsackItem{.id = 1, .value = 10, .weight = 5}},
        .capacity = -5
    };
    EXPECT_THROW(problem.validate_candidate(neg_cap), InvalidCandidateException);
}

TEST_F(Knapsack01Test, NonPositiveWeightThrows) {
    KnapsackCandidate candidate{
        .items = {KnapsackItem{.id = 1, .value = 10, .weight = 0}},
        .capacity = 50
    };
    EXPECT_THROW(problem.validate_candidate(candidate), InvalidCandidateException);
}

TEST_F(Knapsack01Test, NegativeValueThrows) {
    KnapsackCandidate candidate{
        .items = {KnapsackItem{.id = 1, .value = -5, .weight = 10}},
        .capacity = 50
    };
    EXPECT_THROW(problem.validate_candidate(candidate), InvalidCandidateException);
}

TEST_F(Knapsack01Test, DuplicateIdsThrows) {
    KnapsackCandidate candidate{
        .items = {
            KnapsackItem{.id = 1, .value = 10, .weight = 5},
            KnapsackItem{.id = 1, .value = 20, .weight = 8}
        },
        .capacity = 50
    };
    EXPECT_THROW(problem.validate_candidate(candidate), InvalidCandidateException);
}

TEST_F(Knapsack01Test, FamousCounterexampleDiscovered) {
    // Canonical DAA counterexample:
    // W = 50
    // Item 1: v = 60,  w = 10 (ratio 6.0)
    // Item 2: v = 100, w = 20 (ratio 5.0)
    // Item 3: v = 120, w = 30 (ratio 4.0)
    KnapsackCandidate candidate{
        .items = {
            KnapsackItem{.id = 1, .value = 60, .weight = 10},
            KnapsackItem{.id = 2, .value = 100, .weight = 20},
            KnapsackItem{.id = 3, .value = 120, .weight = 30}
        },
        .capacity = 50
    };

    auto greedy_sol = problem.greedy_solution(candidate);
    auto optimal_sol = problem.optimal_solution(candidate);

    ASSERT_TRUE(greedy_sol.success);
    ASSERT_TRUE(optimal_sol.success);

    // Greedy takes Item 1 (wt 10) then Item 2 (wt 20) -> wt 30, remaining 20. Item 3 (wt 30) does not fit!
    // Greedy total value = 60 + 100 = 160
    EXPECT_EQ(greedy_sol.cost.value(), 160.0);

    // Optimal takes Item 2 and Item 3: total weight = 20 + 30 = 50 <= 50
    // Optimal total value = 100 + 120 = 220
    EXPECT_EQ(optimal_sol.cost.value(), 220.0);

    auto cmp = problem.compare_solutions(greedy_sol, optimal_sol);
    EXPECT_TRUE(cmp.is_counterexample);
    EXPECT_DOUBLE_EQ(cmp.absolute_gap.value(), 60.0);
    EXPECT_DOUBLE_EQ(cmp.ratio.value(), 220.0 / 160.0);

    // First divergence analysis
    auto div = problem.analyze_divergence(candidate, greedy_sol, optimal_sol);
    EXPECT_TRUE(div.has_diverged);
    EXPECT_EQ(div.divergence_step.value(), 1);
    EXPECT_EQ(div.greedy_choice.value(), 1); // Greedy chose Item 1, which optimal skipped
}

TEST_F(Knapsack01Test, SearchIntegrationExhaustiveAnyMode) {
    ExhaustiveSearch engine;
    SearchConfig config;
    config.problem_type = ProblemType::KNAPSACK_01;
    config.mode = SearchMode::ANY;

    SearchResult result = engine.search(config);
    EXPECT_TRUE(result.search_completed);
    EXPECT_TRUE(result.found);
    EXPECT_GT(result.counterexamples_found, 0);
    EXPECT_TRUE(result.best_knapsack_candidate.has_value());
    EXPECT_TRUE(result.comparison_metrics.has_value());
    EXPECT_TRUE(result.comparison_metrics.value().is_counterexample);
}

TEST_F(Knapsack01Test, SearchIntegrationExhaustiveSmallestMode) {
    ExhaustiveSearch engine;
    SearchConfig config;
    config.problem_type = ProblemType::KNAPSACK_01;
    config.mode = SearchMode::SMALLEST;

    SearchResult result = engine.search(config);
    EXPECT_TRUE(result.search_completed);
    EXPECT_TRUE(result.found);
    EXPECT_TRUE(result.smallest_verified);
    EXPECT_EQ(result.best_knapsack_candidate.value().capacity, config.min_capacity);
}

TEST_F(Knapsack01Test, SearchIntegrationBacktrackingPruning) {
    BacktrackingSearch engine;
    SearchConfig config;
    config.problem_type = ProblemType::KNAPSACK_01;
    config.mode = SearchMode::ANY;

    SearchResult result = engine.search(config);
    EXPECT_TRUE(result.search_completed);
    EXPECT_TRUE(result.found);
    EXPECT_GT(result.branches_pruned, 0);
}

TEST_F(Knapsack01Test, SearchIntegrationRandomizedSeedReproducibility) {
    RandomizedSearch engine;
    SearchConfig config;
    config.problem_type = ProblemType::KNAPSACK_01;
    config.mode = SearchMode::ANY;
    config.random_seed = 12345;
    config.max_iterations = 2000;

    SearchResult res1 = engine.search(config);
    SearchResult res2 = engine.search(config);

    EXPECT_EQ(res1.found, res2.found);
    EXPECT_EQ(res1.iterations, res2.iterations);
    if (res1.found && res2.found) {
        EXPECT_EQ(res1.best_knapsack_candidate.value().capacity, res2.best_knapsack_candidate.value().capacity);
    }
}

TEST_F(Knapsack01Test, SingleItemFitsMatches) {
    KnapsackCandidate candidate{
        .items = {KnapsackItem{.id = 1, .value = 25, .weight = 10}},
        .capacity = 15
    };
    auto g = problem.greedy_solution(candidate);
    auto opt = problem.optimal_solution(candidate);
    EXPECT_DOUBLE_EQ(g.cost.value(), 25.0);
    EXPECT_DOUBLE_EQ(opt.cost.value(), 25.0);
    auto cmp = problem.compare_solutions(g, opt);
    EXPECT_FALSE(cmp.is_counterexample);
}

TEST_F(Knapsack01Test, SingleItemExceedsCapacityBothTakeZero) {
    KnapsackCandidate candidate{
        .items = {KnapsackItem{.id = 1, .value = 25, .weight = 20}},
        .capacity = 10
    };
    auto g = problem.greedy_solution(candidate);
    auto opt = problem.optimal_solution(candidate);
    EXPECT_DOUBLE_EQ(g.cost.value(), 0.0);
    EXPECT_DOUBLE_EQ(opt.cost.value(), 0.0);
    auto cmp = problem.compare_solutions(g, opt);
    EXPECT_FALSE(cmp.is_counterexample);
}

TEST_F(Knapsack01Test, AllItemsFitGreedyOptimalMatch) {
    KnapsackCandidate candidate{
        .items = {
            KnapsackItem{.id = 1, .value = 10, .weight = 2},
            KnapsackItem{.id = 2, .value = 20, .weight = 3}
        },
        .capacity = 10
    };
    auto g = problem.greedy_solution(candidate);
    auto opt = problem.optimal_solution(candidate);
    EXPECT_DOUBLE_EQ(g.cost.value(), 30.0);
    EXPECT_DOUBLE_EQ(opt.cost.value(), 30.0);
    auto cmp = problem.compare_solutions(g, opt);
    EXPECT_FALSE(cmp.is_counterexample);
}

TEST_F(Knapsack01Test, TieRatiosHigherValuePickedFirst) {
    // Both items have ratio 2.0 (10/5 and 20/10)
    // Tie-break rule: higher ratio -> higher value -> lower weight -> lower ID.
    // Item 2 has higher value (20 > 10), so Item 2 should be considered first.
    KnapsackCandidate candidate{
        .items = {
            KnapsackItem{.id = 1, .value = 10, .weight = 5},
            KnapsackItem{.id = 2, .value = 20, .weight = 10}
        },
        .capacity = 10
    };
    auto g = problem.greedy_solution(candidate);
    ASSERT_EQ(g.items.size(), 1);
    EXPECT_EQ(g.items[0].id, 2); // Item 2 selected first
    EXPECT_DOUBLE_EQ(g.cost.value(), 20.0);
}

TEST_F(Knapsack01Test, SearchIntegrationWorstModeFindsHighSeverity) {
    ExhaustiveSearch engine;
    SearchConfig config;
    config.problem_type = ProblemType::KNAPSACK_01;
    config.mode = SearchMode::WORST;
    config.min_capacity = 5;
    config.max_capacity = 6;
    config.min_items = 3;
    config.max_items = 3;
    config.min_weight = 1;
    config.max_weight = 4;
    config.min_value = 1;
    config.max_value = 6;

    SearchResult result = engine.search(config);
    EXPECT_TRUE(result.search_completed);
    EXPECT_TRUE(result.found);
    EXPECT_TRUE(result.comparison_metrics.has_value());
    EXPECT_GT(result.comparison_metrics.value().absolute_gap.value(), 0.0);
    EXPECT_GT(result.comparison_metrics.value().ratio.value(), 1.0);
}
