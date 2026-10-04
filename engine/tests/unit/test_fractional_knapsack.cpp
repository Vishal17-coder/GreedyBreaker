#include <gtest/gtest.h>
#include "greedybreaker/problems/fractional_knapsack.hpp"
#include "greedybreaker/core/verification_result.hpp"

using namespace greedybreaker::problems;
using namespace greedybreaker::core;

TEST(FractionalKnapsackTest, ValueDensityIsOptimalAndContrasts01) {
    // Exact same instance that failed in 0/1 Knapsack:
    // Item 1: v=60,  w=10 -> density 6.0
    // Item 2: v=100, w=20 -> density 5.0
    // Item 3: v=120, w=30 -> density 4.0
    // Capacity = 50
    // In Fractional: take 100% Item 1 (60), 100% Item 2 (100), and 20/30 of Item 3 (80) = 240.0!
    FractionalKnapsackInstance inst;
    inst.capacity = 50;
    inst.items = {
        {1, 60.0, 10.0},
        {2, 100.0, 20.0},
        {3, 120.0, 30.0}
    };

    ASSERT_TRUE(FractionalKnapsackProblem::is_valid_instance(inst));

    auto greedy = FractionalKnapsackProblem::solve_greedy(inst, "value_density_descending");
    auto oracle = FractionalKnapsackProblem::solve_exact_lp(inst);

    auto ver = verify(FractionalKnapsackProblem::objective(), greedy, oracle);
    EXPECT_EQ(ver.status, VerificationStatus::OPTIMAL);
    EXPECT_FALSE(ver.is_counterexample);
    EXPECT_DOUBLE_EQ(greedy.objective_value, 240.0);
    EXPECT_DOUBLE_EQ(oracle.optimal_value, 240.0);
}

TEST(FractionalKnapsackTest, MinWeightHeuristicFailsAndIsDiscovered) {
    // Item 1: v=1, w=1 (density 1.0)
    // Item 2: v=100, w=10 (density 10.0)
    // Capacity = 10
    // Min weight picks Item 1 (w=1, v=1), then 9/10 of Item 2 (w=9, v=90) -> Total = 91.0
    // Optimal picks 10/10 of Item 2 (w=10, v=100) -> Total = 100.0
    FractionalKnapsackInstance inst;
    inst.capacity = 10;
    inst.items = {
        {1, 1.0, 1.0},
        {2, 100.0, 10.0}
    };

    auto greedy = FractionalKnapsackProblem::solve_greedy(inst, "min_weight_first");
    auto oracle = FractionalKnapsackProblem::solve_exact_lp(inst);

    auto ver = verify(FractionalKnapsackProblem::objective(), greedy, oracle);
    EXPECT_EQ(ver.status, VerificationStatus::OPTIMALITY_FAILURE);
    EXPECT_TRUE(ver.is_counterexample);
    EXPECT_DOUBLE_EQ(greedy.objective_value, 91.0);
    EXPECT_DOUBLE_EQ(oracle.optimal_value, 100.0);
    EXPECT_DOUBLE_EQ(ver.absolute_gap, 9.0);
    EXPECT_DOUBLE_EQ(ver.failure_percent, 9.0);
}
