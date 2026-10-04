#include <gtest/gtest.h>
#include "greedybreaker/core/verification_result.hpp"

using namespace greedybreaker::core;

TEST(VerificationResultTest, MinimizationOptimalMatch) {
    GreedyResult greedy;
    greedy.feasible = true;
    greedy.objective_value = 2.0;

    OracleResult oracle;
    oracle.confidence = OracleConfidence::EXACT;
    oracle.optimal_value = 2.0;

    auto result = verify(ObjectiveDirection::MINIMIZE, greedy, oracle);
    EXPECT_EQ(result.status, VerificationStatus::OPTIMAL);
    EXPECT_FALSE(result.is_counterexample);
    EXPECT_DOUBLE_EQ(result.absolute_gap, 0.0);
    EXPECT_DOUBLE_EQ(result.relative_gap, 1.0);
    EXPECT_DOUBLE_EQ(result.failure_percent, 0.0);
}

TEST(VerificationResultTest, MinimizationOptimalityFailureExact) {
    // Canonical Coin Change: Greedy uses 3 coins (4 + 1 + 1), Optimal uses 2 coins (3 + 3)
    GreedyResult greedy;
    greedy.feasible = true;
    greedy.objective_value = 3.0;

    OracleResult oracle;
    oracle.confidence = OracleConfidence::EXACT;
    oracle.optimal_value = 2.0;

    auto result = verify(ObjectiveDirection::MINIMIZE, greedy, oracle);
    EXPECT_EQ(result.status, VerificationStatus::OPTIMALITY_FAILURE);
    EXPECT_TRUE(result.is_counterexample);
    EXPECT_DOUBLE_EQ(result.absolute_gap, 1.0);
    EXPECT_DOUBLE_EQ(result.relative_gap, 1.5);
    EXPECT_DOUBLE_EQ(result.failure_percent, 50.0); // 50% overhead
}

TEST(VerificationResultTest, MinimizationNonExactOracleDoesNotCertifyCounterexample) {
    // If oracle confidence is APPROXIMATION, must not declare verified counterexample
    GreedyResult greedy;
    greedy.feasible = true;
    greedy.objective_value = 3.0;

    OracleResult oracle;
    oracle.confidence = OracleConfidence::APPROXIMATION;
    oracle.optimal_value = 2.0;

    auto result = verify(ObjectiveDirection::MINIMIZE, greedy, oracle);
    EXPECT_EQ(result.status, VerificationStatus::OPTIMALITY_FAILURE);
    EXPECT_FALSE(result.is_counterexample); // MUST BE FALSE because oracle not exact
}

TEST(VerificationResultTest, MaximizationOptimalMatch) {
    GreedyResult greedy;
    greedy.feasible = true;
    greedy.objective_value = 50.0;

    OracleResult oracle;
    oracle.confidence = OracleConfidence::EXACT;
    oracle.optimal_value = 50.0;

    auto result = verify(ObjectiveDirection::MAXIMIZE, greedy, oracle);
    EXPECT_EQ(result.status, VerificationStatus::OPTIMAL);
    EXPECT_FALSE(result.is_counterexample);
    EXPECT_DOUBLE_EQ(result.absolute_gap, 0.0);
}

TEST(VerificationResultTest, MaximizationOptimalityFailureExact) {
    // 0/1 Knapsack: Greedy gets 160 value, Optimal gets 220 value
    GreedyResult greedy;
    greedy.feasible = true;
    greedy.objective_value = 160.0;

    OracleResult oracle;
    oracle.confidence = OracleConfidence::EXACT;
    oracle.optimal_value = 220.0;

    auto result = verify(ObjectiveDirection::MAXIMIZE, greedy, oracle);
    EXPECT_EQ(result.status, VerificationStatus::OPTIMALITY_FAILURE);
    EXPECT_TRUE(result.is_counterexample);
    EXPECT_DOUBLE_EQ(result.absolute_gap, 60.0);
    EXPECT_DOUBLE_EQ(result.relative_gap, 220.0 / 160.0);
    EXPECT_NEAR(result.failure_percent, (60.0 / 220.0) * 100.0, 1e-5);
}

TEST(VerificationResultTest, FeasibilityFailure) {
    GreedyResult greedy;
    greedy.feasible = false; // Greedy could not form target
    greedy.objective_value = 0.0;

    OracleResult oracle;
    oracle.confidence = OracleConfidence::EXACT;
    oracle.optimal_value = 2.0;

    auto result = verify(ObjectiveDirection::MINIMIZE, greedy, oracle);
    EXPECT_EQ(result.status, VerificationStatus::FEASIBILITY_FAILURE);
    EXPECT_TRUE(result.is_counterexample);
}
