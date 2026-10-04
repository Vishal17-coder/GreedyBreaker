#include <gtest/gtest.h>
#include "greedybreaker/problems/knapsack.hpp"
#include "greedybreaker/search/exhaustive_search.hpp"

using namespace greedybreaker;

class FractionalKnapsackTest : public ::testing::Test {
protected:
    FractionalKnapsackProblem problem;
};

TEST_F(FractionalKnapsackTest, ValidCandidatePasses) {
    KnapsackCandidate candidate{
        .items = {
            KnapsackItem{.id = 1, .value = 60, .weight = 10},
            KnapsackItem{.id = 2, .value = 100, .weight = 20}
        },
        .capacity = 50
    };
    EXPECT_NO_THROW(problem.validate_candidate(candidate));
}

TEST_F(FractionalKnapsackTest, PartialItemSelectionSaturatesCapacity) {
    // Capacity = 10
    // Item 1: v = 20, w = 5  (ratio 4.0)
    // Item 2: v = 30, w = 10 (ratio 3.0)
    // Greedy should take 100% of Item 1 (v=20, w=5)
    // Then 50% of Item 2 (5/10 * 30 = 15, w=5)
    // Total value = 20 + 15 = 35.0
    KnapsackCandidate candidate{
        .items = {
            KnapsackItem{.id = 1, .value = 20, .weight = 5},
            KnapsackItem{.id = 2, .value = 30, .weight = 10}
        },
        .capacity = 10
    };

    auto sol = problem.greedy_solution(candidate);
    ASSERT_TRUE(sol.success);
    ASSERT_EQ(sol.items.size(), 2);

    EXPECT_DOUBLE_EQ(sol.items[0].fraction, 1.0);
    EXPECT_DOUBLE_EQ(sol.items[0].value_taken, 20.0);
    EXPECT_DOUBLE_EQ(sol.items[0].weight_taken, 5.0);

    EXPECT_DOUBLE_EQ(sol.items[1].fraction, 0.5);
    EXPECT_DOUBLE_EQ(sol.items[1].value_taken, 15.0);
    EXPECT_DOUBLE_EQ(sol.items[1].weight_taken, 5.0);

    EXPECT_DOUBLE_EQ(sol.cost.value(), 35.0);
}

TEST_F(FractionalKnapsackTest, Known01CounterexampleSucceedsInFractional) {
    // Under 0/1 Knapsack, this instance failed with gap 60 (Greedy 160 vs Optimal 220).
    // Under Fractional Knapsack, Greedy takes:
    // Item 1: v=60, w=10 (full, rem 40)
    // Item 2: v=100, w=20 (full, rem 20)
    // Item 3: 20/30 * 120 = 80 (fractional, rem 0)
    // Total value = 60 + 100 + 80 = 240.0 (Global Optimum!)
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

    EXPECT_DOUBLE_EQ(greedy_sol.cost.value(), 240.0);
    EXPECT_DOUBLE_EQ(optimal_sol.cost.value(), 240.0);

    auto cmp = problem.compare_solutions(greedy_sol, optimal_sol);
    EXPECT_FALSE(cmp.is_counterexample);
    EXPECT_DOUBLE_EQ(cmp.absolute_gap.value(), 0.0);
    EXPECT_DOUBLE_EQ(cmp.ratio.value(), 1.0);
}

TEST_F(FractionalKnapsackTest, ExhaustiveSearchFindsZeroCounterexamples) {
    ExhaustiveSearch engine;
    SearchConfig config;
    config.problem_type = ProblemType::KNAPSACK_FRACTIONAL;
    config.min_items = 3;
    config.max_items = 3;
    config.min_weight = 1;
    config.max_weight = 3;
    config.min_value = 1;
    config.max_value = 5;
    config.min_capacity = 4;
    config.max_capacity = 5;

    SearchResult result = engine.search(config);
    EXPECT_TRUE(result.search_completed);
    EXPECT_FALSE(result.found);
    EXPECT_EQ(result.counterexamples_found, 0);
}
