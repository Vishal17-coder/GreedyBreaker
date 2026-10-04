#include <gtest/gtest.h>
#include "greedybreaker/core/metrics.hpp"
#include <cmath>

using namespace greedybreaker;

TEST(MetricsTest, MinimizationCounterexampleSeverity) {
    // Greedy uses 3 coins, Optimal uses 2 coins
    auto result = calculate_comparison(
        ProblemObjective::Minimize,
        true,
        true,
        3.0,
        2.0
    );

    EXPECT_TRUE(result.is_counterexample);
    ASSERT_TRUE(result.greedy_cost.has_value());
    ASSERT_TRUE(result.optimal_cost.has_value());
    EXPECT_DOUBLE_EQ(result.greedy_cost.value(), 3.0);
    EXPECT_DOUBLE_EQ(result.optimal_cost.value(), 2.0);

    ASSERT_TRUE(result.absolute_gap.has_value());
    EXPECT_DOUBLE_EQ(result.absolute_gap.value(), 1.0);

    ASSERT_TRUE(result.ratio.has_value());
    EXPECT_DOUBLE_EQ(result.ratio.value(), 1.50);

    ASSERT_TRUE(result.overhead_percentage.has_value());
    EXPECT_DOUBLE_EQ(result.overhead_percentage.value(), 50.0);

    EXPECT_NE(result.interpretation.find("COUNTEREXAMPLE CONFIRMED"), std::string::npos);
}

TEST(MetricsTest, MinimizationOptimalMatch) {
    auto result = calculate_comparison(
        ProblemObjective::Minimize,
        true,
        true,
        3.0,
        3.0
    );

    EXPECT_FALSE(result.is_counterexample);
    ASSERT_TRUE(result.absolute_gap.has_value());
    EXPECT_DOUBLE_EQ(result.absolute_gap.value(), 0.0);
    ASSERT_TRUE(result.ratio.has_value());
    EXPECT_DOUBLE_EQ(result.ratio.value(), 1.0);
    ASSERT_TRUE(result.overhead_percentage.has_value());
    EXPECT_DOUBLE_EQ(result.overhead_percentage.value(), 0.0);
    EXPECT_NE(result.interpretation.find("OPTIMAL MATCH"), std::string::npos);
}

TEST(MetricsTest, MinimizationGreedyDeadEnd) {
    auto result = calculate_comparison(
        ProblemObjective::Minimize,
        false,
        true,
        std::nullopt,
        2.0
    );

    EXPECT_TRUE(result.is_counterexample);
    EXPECT_FALSE(result.greedy_solved);
    EXPECT_TRUE(result.optimal_solved);
    ASSERT_TRUE(result.absolute_gap.has_value());
    EXPECT_TRUE(std::isinf(result.absolute_gap.value()));
    ASSERT_TRUE(result.ratio.has_value());
    EXPECT_TRUE(std::isinf(result.ratio.value()));
    ASSERT_TRUE(result.overhead_percentage.has_value());
    EXPECT_TRUE(std::isinf(result.overhead_percentage.value()));
    EXPECT_NE(result.interpretation.find("CRITICAL FAILURE"), std::string::npos);
}

TEST(MetricsTest, UnsolvableInstanceNeitherSolves) {
    auto result = calculate_comparison(
        ProblemObjective::Minimize,
        false,
        false,
        std::nullopt,
        std::nullopt
    );

    EXPECT_FALSE(result.is_counterexample);
    ASSERT_TRUE(result.absolute_gap.has_value());
    EXPECT_DOUBLE_EQ(result.absolute_gap.value(), 0.0);
    ASSERT_TRUE(result.ratio.has_value());
    EXPECT_DOUBLE_EQ(result.ratio.value(), 1.0);
    ASSERT_TRUE(result.overhead_percentage.has_value());
    EXPECT_DOUBLE_EQ(result.overhead_percentage.value(), 0.0);
    EXPECT_NE(result.interpretation.find("unsolvable"), std::string::npos);
}

TEST(MetricsTest, MaximizationCounterexampleSeverity) {
    // Knapsack: Greedy value 80, Optimal value 100
    auto result = calculate_comparison(
        ProblemObjective::Maximize,
        true,
        true,
        80.0,
        100.0
    );

    EXPECT_TRUE(result.is_counterexample);
    ASSERT_TRUE(result.absolute_gap.has_value());
    EXPECT_DOUBLE_EQ(result.absolute_gap.value(), 20.0);
    ASSERT_TRUE(result.ratio.has_value());
    EXPECT_DOUBLE_EQ(result.ratio.value(), 1.25);
    ASSERT_TRUE(result.overhead_percentage.has_value());
    EXPECT_DOUBLE_EQ(result.overhead_percentage.value(), 20.0);
}

TEST(MetricsTest, MaximizationBothZeroCostMatches) {
    auto result = calculate_comparison(
        ProblemObjective::Maximize,
        true,
        true,
        0.0,
        0.0
    );

    EXPECT_FALSE(result.is_counterexample);
    ASSERT_TRUE(result.absolute_gap.has_value());
    EXPECT_DOUBLE_EQ(result.absolute_gap.value(), 0.0);
    ASSERT_TRUE(result.ratio.has_value());
    EXPECT_DOUBLE_EQ(result.ratio.value(), 1.0);
    ASSERT_TRUE(result.overhead_percentage.has_value());
    EXPECT_DOUBLE_EQ(result.overhead_percentage.value(), 0.0);
}

TEST(MetricsTest, MaximizationGreedyZeroOptimalPositive) {
    auto result = calculate_comparison(
        ProblemObjective::Maximize,
        true,
        true,
        0.0,
        50.0
    );

    EXPECT_TRUE(result.is_counterexample);
    ASSERT_TRUE(result.absolute_gap.has_value());
    EXPECT_DOUBLE_EQ(result.absolute_gap.value(), 50.0);
    ASSERT_TRUE(result.ratio.has_value());
    EXPECT_TRUE(std::isinf(result.ratio.value()));
    ASSERT_TRUE(result.overhead_percentage.has_value());
    EXPECT_DOUBLE_EQ(result.overhead_percentage.value(), 100.0);
}

TEST(MetricsTest, InvalidStateThrowsException) {
    EXPECT_THROW(
        calculate_comparison(
            ProblemObjective::Minimize,
            true,
            false,
            3.0,
            std::nullopt
        ),
        std::runtime_error
    );
}
