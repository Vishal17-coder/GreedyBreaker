#include <gtest/gtest.h>
#include "greedybreaker/problems/activity_selection.hpp"
#include "greedybreaker/core/verification_result.hpp"

using namespace greedybreaker::problems;
using namespace greedybreaker::core;

TEST(ActivitySelectionTest, EarliestFinishTimeIsProvablyOptimal) {
    // Canonical instance where Earliest Finish Time must match DP
    ActivitySelectionInstance inst;
    inst.activities = {
        {1, 1, 4},
        {2, 3, 5},
        {3, 0, 6},
        {4, 5, 7},
        {5, 3, 9},
        {6, 5, 9},
        {7, 6, 10},
        {8, 8, 11},
        {9, 8, 12},
        {10, 2, 14},
        {11, 12, 16}
    };

    ASSERT_TRUE(ActivitySelectionProblem::is_valid_instance(inst));

    auto greedy = ActivitySelectionProblem::solve_greedy(inst, "earliest_finish_time");
    auto oracle = ActivitySelectionProblem::solve_exact_dp(inst);

    auto ver = verify(ActivitySelectionProblem::objective(), greedy, oracle);
    EXPECT_EQ(ver.status, VerificationStatus::OPTIMAL);
    EXPECT_FALSE(ver.is_counterexample);
    EXPECT_DOUBLE_EQ(greedy.objective_value, 4.0);
    EXPECT_DOUBLE_EQ(oracle.optimal_value, 4.0);
}

TEST(ActivitySelectionTest, ShortestIntervalFailsAndIsDiscovered) {
    // Textbook failure instance for Shortest Interval First:
    // Act 1: [1, 5] (duration 4)
    // Act 2: [4, 7] (duration 3 - shortest!)
    // Act 3: [6, 10] (duration 4)
    ActivitySelectionInstance inst;
    inst.activities = {
        {1, 1, 5},
        {2, 4, 7},
        {3, 6, 10}
    };

    auto greedy = ActivitySelectionProblem::solve_greedy(inst, "shortest_interval");
    EXPECT_TRUE(greedy.feasible);
    EXPECT_DOUBLE_EQ(greedy.objective_value, 1.0); // Only Act 2 scheduled

    auto oracle = ActivitySelectionProblem::solve_exact_dp(inst);
    EXPECT_DOUBLE_EQ(oracle.optimal_value, 2.0); // Act 1 + Act 3 scheduled

    auto ver = verify(ActivitySelectionProblem::objective(), greedy, oracle);
    EXPECT_EQ(ver.status, VerificationStatus::OPTIMALITY_FAILURE);
    EXPECT_TRUE(ver.is_counterexample);
    EXPECT_DOUBLE_EQ(ver.absolute_gap, 1.0);
    EXPECT_DOUBLE_EQ(ver.failure_percent, 50.0);
}

TEST(ActivitySelectionTest, CrossValidationDPvsExhaustive) {
    ActivitySelectionInstance inst;
    inst.activities = {
        {1, 1, 3},
        {2, 2, 5},
        {3, 4, 7},
        {4, 1, 8},
        {5, 6, 9},
        {6, 8, 10}
    };

    auto dp_res = ActivitySelectionProblem::solve_exact_dp(inst);
    auto exh_res = ActivitySelectionProblem::solve_exact_exhaustive(inst);

    EXPECT_DOUBLE_EQ(dp_res.optimal_value, exh_res.optimal_value);
    EXPECT_EQ(dp_res.items_selected, exh_res.items_selected);
}
