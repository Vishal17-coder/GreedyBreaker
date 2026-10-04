#include <gtest/gtest.h>
#include "greedybreaker/problems/knapsack_01.hpp"
#include "greedybreaker/core/verification_result.hpp"

using namespace greedybreaker::problems;
using namespace greedybreaker::core;

TEST(Knapsack01Test, CanonicalRatioFailureDiscovered) {
    // Canonical 0/1 Knapsack counterexample:
    // Item 1: v=60,  w=10 -> density 6.0
    // Item 2: v=100, w=20 -> density 5.0
    // Item 3: v=120, w=30 -> density 4.0
    // Capacity = 50
    Knapsack01Instance inst;
    inst.capacity = 50;
    inst.items = {
        {1, 60.0, 10.0},
        {2, 100.0, 20.0},
        {3, 120.0, 30.0}
    };

    ASSERT_TRUE(Knapsack01Problem::is_valid_instance(inst));

    auto greedy = Knapsack01Problem::solve_greedy(inst, "value_density_descending");
    EXPECT_TRUE(greedy.feasible);
    EXPECT_DOUBLE_EQ(greedy.objective_value, 160.0); // Item 1 (60) + Item 2 (100) = 160

    auto oracle = Knapsack01Problem::solve_exact_dp(inst);
    EXPECT_TRUE(oracle.is_exact());
    EXPECT_DOUBLE_EQ(oracle.optimal_value, 220.0); // Item 2 (100) + Item 3 (120) = 220

    auto ver = verify(Knapsack01Problem::objective(), greedy, oracle);
    EXPECT_EQ(ver.status, VerificationStatus::OPTIMALITY_FAILURE);
    EXPECT_TRUE(ver.is_counterexample);
    EXPECT_DOUBLE_EQ(ver.absolute_gap, 60.0);
    EXPECT_DOUBLE_EQ(ver.relative_gap, 220.0 / 160.0);
    EXPECT_NEAR(ver.failure_percent, (60.0 / 220.0) * 100.0, 1e-4);
}

TEST(Knapsack01Test, OptimalInstanceMatches) {
    Knapsack01Instance inst;
    inst.capacity = 30;
    inst.items = {
        {1, 100.0, 10.0},
        {2, 50.0, 10.0},
        {3, 20.0, 10.0}
    };

    auto greedy = Knapsack01Problem::solve_greedy(inst);
    auto oracle = Knapsack01Problem::solve_exact_dp(inst);

    auto ver = verify(Knapsack01Problem::objective(), greedy, oracle);
    EXPECT_EQ(ver.status, VerificationStatus::OPTIMAL);
    EXPECT_FALSE(ver.is_counterexample);
    EXPECT_DOUBLE_EQ(greedy.objective_value, 170.0);
    EXPECT_DOUBLE_EQ(oracle.optimal_value, 170.0);
}

TEST(Knapsack01Test, CrossValidationDPvsExhaustive) {
    Knapsack01Instance inst;
    inst.capacity = 16;
    inst.items = {
        {1, 10.0, 5.0},
        {2, 40.0, 4.0},
        {3, 30.0, 6.0},
        {4, 50.0, 3.0},
        {5, 35.0, 5.0},
        {6, 40.0, 7.0}
    };

    auto dp_res = Knapsack01Problem::solve_exact_dp(inst);
    auto exh_res = Knapsack01Problem::solve_exact_exhaustive(inst);

    EXPECT_DOUBLE_EQ(dp_res.optimal_value, exh_res.optimal_value);
    EXPECT_EQ(dp_res.items_selected, exh_res.items_selected);
}
