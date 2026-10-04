#include <gtest/gtest.h>
#include "greedybreaker/problems/coin_change.hpp"
#include <numeric>

using namespace greedybreaker;

class CoinChangeTest : public ::testing::Test {
protected:
    CoinChangeProblem problem;
};

// ==========================================
// 1. Validation Tests
// ==========================================

TEST_F(CoinChangeTest, ValidCandidatePasses) {
    CoinChangeCandidate c{{1, 2, 5}, 11};
    EXPECT_NO_THROW(problem.validate_candidate(c));
}

TEST_F(CoinChangeTest, EmptyDenominationsThrows) {
    CoinChangeCandidate c{{}, 10};
    EXPECT_THROW(problem.validate_candidate(c), InvalidCandidateException);
}

TEST_F(CoinChangeTest, ZeroOrNegativeTargetThrows) {
    CoinChangeCandidate c1{{1, 5}, 0};
    EXPECT_THROW(problem.validate_candidate(c1), InvalidCandidateException);

    CoinChangeCandidate c2{{1, 5}, -10};
    EXPECT_THROW(problem.validate_candidate(c2), InvalidCandidateException);
}

TEST_F(CoinChangeTest, ZeroOrNegativeDenominationThrows) {
    CoinChangeCandidate c1{{0, 1, 5}, 10};
    EXPECT_THROW(problem.validate_candidate(c1), InvalidCandidateException);

    CoinChangeCandidate c2{{-2, 1, 5}, 10};
    EXPECT_THROW(problem.validate_candidate(c2), InvalidCandidateException);
}

TEST_F(CoinChangeTest, DuplicateDenominationsThrows) {
    CoinChangeCandidate c{{1, 3, 3, 4}, 6};
    EXPECT_THROW(problem.validate_candidate(c), InvalidCandidateException);
}

// ==========================================
// 2. Canonical Cases Where Greedy is Optimal
// ==========================================

TEST_F(CoinChangeTest, USCoinSystemOptimal) {
    std::vector<int> targets = {1, 6, 15, 30, 42, 67, 99};
    for (int t : targets) {
        CoinChangeCandidate c{{1, 5, 10, 25}, t};
        auto greedy = problem.greedy_solution(c);
        auto optimal = problem.optimal_solution(c);
        auto cmp = problem.compare_solutions(greedy, optimal);

        EXPECT_TRUE(greedy.success);
        EXPECT_TRUE(optimal.success);
        ASSERT_TRUE(greedy.cost.has_value());
        ASSERT_TRUE(optimal.cost.has_value());
        EXPECT_DOUBLE_EQ(greedy.cost.value(), optimal.cost.value());
        EXPECT_FALSE(cmp.is_counterexample);
        EXPECT_DOUBLE_EQ(cmp.absolute_gap.value(), 0.0);
        EXPECT_DOUBLE_EQ(cmp.ratio.value(), 1.0);
    }
}

TEST_F(CoinChangeTest, PowersOfTwoOptimal) {
    CoinChangeCandidate c{{1, 2, 4, 8, 16}, 27};
    auto greedy = problem.greedy_solution(c);
    auto optimal = problem.optimal_solution(c);
    auto cmp = problem.compare_solutions(greedy, optimal);

    EXPECT_TRUE(greedy.success);
    EXPECT_TRUE(optimal.success);
    EXPECT_DOUBLE_EQ(greedy.cost.value(), optimal.cost.value());
    EXPECT_FALSE(cmp.is_counterexample);
}

TEST_F(CoinChangeTest, SingleDenominationOptimal) {
    CoinChangeCandidate c{{1}, 10};
    auto greedy = problem.greedy_solution(c);
    auto optimal = problem.optimal_solution(c);
    auto cmp = problem.compare_solutions(greedy, optimal);

    EXPECT_DOUBLE_EQ(greedy.cost.value(), 10.0);
    EXPECT_DOUBLE_EQ(optimal.cost.value(), 10.0);
    EXPECT_FALSE(cmp.is_counterexample);
}

// ==========================================
// 3. Known Counterexamples
// ==========================================

TEST_F(CoinChangeTest, FamousCounterexample1_3_4_Target6) {
    CoinChangeCandidate c{{1, 3, 4}, 6};
    auto greedy = problem.greedy_solution(c);
    auto optimal = problem.optimal_solution(c);
    auto cmp = problem.compare_solutions(greedy, optimal);
    auto div = problem.analyze_divergence(c, greedy, optimal);

    // Greedy: 4 + 1 + 1 (3 coins)
    EXPECT_TRUE(greedy.success);
    ASSERT_TRUE(greedy.cost.has_value());
    EXPECT_DOUBLE_EQ(greedy.cost.value(), 3.0);
    ASSERT_EQ(greedy.items.size(), 3);
    EXPECT_EQ(greedy.items[0], 4);
    EXPECT_EQ(greedy.items[1], 1);
    EXPECT_EQ(greedy.items[2], 1);

    // Optimal: 3 + 3 (2 coins)
    EXPECT_TRUE(optimal.success);
    ASSERT_TRUE(optimal.cost.has_value());
    EXPECT_DOUBLE_EQ(optimal.cost.value(), 2.0);
    ASSERT_EQ(optimal.items.size(), 2);
    EXPECT_EQ(optimal.items[0], 3);
    EXPECT_EQ(optimal.items[1], 3);

    // Severity Metrics
    EXPECT_TRUE(cmp.is_counterexample);
    ASSERT_TRUE(cmp.absolute_gap.has_value());
    EXPECT_DOUBLE_EQ(cmp.absolute_gap.value(), 1.0);
    ASSERT_TRUE(cmp.ratio.has_value());
    EXPECT_DOUBLE_EQ(cmp.ratio.value(), 1.50);
    ASSERT_TRUE(cmp.overhead_percentage.has_value());
    EXPECT_DOUBLE_EQ(cmp.overhead_percentage.value(), 50.0);

    // Divergence Analysis
    EXPECT_TRUE(div.has_diverged);
    ASSERT_TRUE(div.divergence_step.has_value());
    EXPECT_EQ(div.divergence_step.value(), 1);
    EXPECT_EQ(div.state_remaining, 6);
    ASSERT_TRUE(div.greedy_choice.has_value());
    EXPECT_EQ(div.greedy_choice.value(), 4);
    ASSERT_EQ(div.optimal_choices.size(), 1);
    EXPECT_EQ(div.optimal_choices[0], 3);
}

TEST_F(CoinChangeTest, Counterexample1_4_5_6_Target8) {
    CoinChangeCandidate c{{1, 4, 5, 6}, 8};
    auto greedy = problem.greedy_solution(c);
    auto optimal = problem.optimal_solution(c);
    auto cmp = problem.compare_solutions(greedy, optimal);
    auto div = problem.analyze_divergence(c, greedy, optimal);

    // Greedy: 6 + 1 + 1 (3 coins)
    // Optimal: 4 + 4 (2 coins)
    EXPECT_DOUBLE_EQ(greedy.cost.value(), 3.0);
    EXPECT_DOUBLE_EQ(optimal.cost.value(), 2.0);
    EXPECT_TRUE(cmp.is_counterexample);
    EXPECT_DOUBLE_EQ(cmp.absolute_gap.value(), 1.0);
    EXPECT_DOUBLE_EQ(cmp.ratio.value(), 1.50);

    EXPECT_TRUE(div.has_diverged);
    EXPECT_EQ(div.divergence_step.value(), 1);
    EXPECT_EQ(div.greedy_choice.value(), 6);
    ASSERT_EQ(div.optimal_choices.size(), 1);
    EXPECT_EQ(div.optimal_choices[0], 4);
}

TEST_F(CoinChangeTest, Counterexample1_6_9_Target12) {
    CoinChangeCandidate c{{1, 6, 9}, 12};
    auto greedy = problem.greedy_solution(c);
    auto optimal = problem.optimal_solution(c);
    auto cmp = problem.compare_solutions(greedy, optimal);

    // Greedy: 9 + 1 + 1 + 1 (4 coins)
    // Optimal: 6 + 6 (2 coins)
    EXPECT_DOUBLE_EQ(greedy.cost.value(), 4.0);
    EXPECT_DOUBLE_EQ(optimal.cost.value(), 2.0);
    EXPECT_TRUE(cmp.is_counterexample);
    EXPECT_DOUBLE_EQ(cmp.absolute_gap.value(), 2.0);
    EXPECT_DOUBLE_EQ(cmp.ratio.value(), 2.0);
    EXPECT_DOUBLE_EQ(cmp.overhead_percentage.value(), 100.0);
}

TEST_F(CoinChangeTest, Counterexample1_5_6_9_Target11) {
    CoinChangeCandidate c{{1, 5, 6, 9}, 11};
    auto greedy = problem.greedy_solution(c);
    auto optimal = problem.optimal_solution(c);
    auto cmp = problem.compare_solutions(greedy, optimal);

    // Greedy: 9 + 1 + 1 (3 coins)
    // Optimal: 6 + 5 (2 coins)
    EXPECT_DOUBLE_EQ(greedy.cost.value(), 3.0);
    EXPECT_DOUBLE_EQ(optimal.cost.value(), 2.0);
    EXPECT_TRUE(cmp.is_counterexample);
}

TEST_F(CoinChangeTest, Counterexample1_8_20_Target24) {
    CoinChangeCandidate c{{1, 8, 20}, 24};
    auto greedy = problem.greedy_solution(c);
    auto optimal = problem.optimal_solution(c);
    auto cmp = problem.compare_solutions(greedy, optimal);

    // Greedy: 20 + 1 + 1 + 1 + 1 (5 coins)
    // Optimal: 8 + 8 + 8 (3 coins)
    EXPECT_DOUBLE_EQ(greedy.cost.value(), 5.0);
    EXPECT_DOUBLE_EQ(optimal.cost.value(), 3.0);
    EXPECT_TRUE(cmp.is_counterexample);
    EXPECT_DOUBLE_EQ(cmp.absolute_gap.value(), 2.0);
}

// ==========================================
// 4. Divergence at Step Two
// ==========================================

TEST_F(CoinChangeTest, DivergenceAtStepTwo) {
    // Denominations {1, 3, 4, 10}, target = 16
    // Step 1: Target 16. Greedy chooses 10 (remainder 6).
    // Is 10 optimal? Yes! 10 + 3 + 3 = 16 (3 coins), matching dp[16]=3.
    // Step 2: Remaining 6. Greedy chooses 4 (remainder 2 -> 1, 1). Total 4 coins.
    // Optimal requires 3, leaving 3 -> needs 3. Total 3 coins.
    CoinChangeCandidate c{{1, 3, 4, 10}, 16};
    auto greedy = problem.greedy_solution(c);
    auto optimal = problem.optimal_solution(c);
    auto div = problem.analyze_divergence(c, greedy, optimal);

    EXPECT_DOUBLE_EQ(greedy.cost.value(), 4.0);
    EXPECT_DOUBLE_EQ(optimal.cost.value(), 3.0);
    EXPECT_TRUE(div.has_diverged);
    ASSERT_TRUE(div.divergence_step.has_value());
    EXPECT_EQ(div.divergence_step.value(), 2);
    EXPECT_EQ(div.state_remaining, 6);
    EXPECT_EQ(div.greedy_choice.value(), 4);
    ASSERT_EQ(div.optimal_choices.size(), 1);
    EXPECT_EQ(div.optimal_choices[0], 3);
}

// ==========================================
// 5. Dead Ends and Solvability
// ==========================================

TEST_F(CoinChangeTest, GreedyDeadEndWhileOptimalSucceeds) {
    // Denominations {3, 4}, target = 6
    // Greedy: takes 4, remainder 2 -> dead end!
    // Optimal: takes 3 + 3 = 6 (2 coins) -> success!
    CoinChangeCandidate c{{3, 4}, 6};
    auto greedy = problem.greedy_solution(c);
    auto optimal = problem.optimal_solution(c);
    auto cmp = problem.compare_solutions(greedy, optimal);
    auto div = problem.analyze_divergence(c, greedy, optimal);

    EXPECT_FALSE(greedy.success);
    EXPECT_TRUE(optimal.success);
    EXPECT_DOUBLE_EQ(optimal.cost.value(), 2.0);

    EXPECT_TRUE(cmp.is_counterexample);
    EXPECT_FALSE(cmp.greedy_solved);
    EXPECT_TRUE(cmp.optimal_solved);

    EXPECT_TRUE(div.has_diverged);
    EXPECT_EQ(div.divergence_step.value(), 1);
    EXPECT_EQ(div.greedy_choice.value(), 4);
    ASSERT_EQ(div.optimal_choices.size(), 1);
    EXPECT_EQ(div.optimal_choices[0], 3);
}

TEST_F(CoinChangeTest, UnsolvableInstanceBothFail) {
    CoinChangeCandidate c{{3, 5}, 7};
    auto greedy = problem.greedy_solution(c);
    auto optimal = problem.optimal_solution(c);
    auto cmp = problem.compare_solutions(greedy, optimal);

    EXPECT_FALSE(greedy.success);
    EXPECT_FALSE(optimal.success);
    EXPECT_FALSE(cmp.is_counterexample);
}

// ==========================================
// 6. Trace Integrity and Reconstruction
// ==========================================

TEST_F(CoinChangeTest, TraceIntegrityAndReconstruction) {
    CoinChangeCandidate c{{1, 3, 4}, 6};
    auto greedy = problem.greedy_solution(c);
    auto optimal = problem.optimal_solution(c);

    // Sum of reconstructed coins must match target
    int greedy_sum = std::accumulate(greedy.items.begin(), greedy.items.end(), 0);
    int optimal_sum = std::accumulate(optimal.items.begin(), optimal.items.end(), 0);
    EXPECT_EQ(greedy_sum, 6);
    EXPECT_EQ(optimal_sum, 6);

    // Trace step indexing and rationale
    EXPECT_EQ(greedy.trace.steps.size(), 3);
    for (size_t i = 0; i < greedy.trace.steps.size(); ++i) {
        EXPECT_EQ(greedy.trace.steps[i].step_number, static_cast<int>(i + 1));
        EXPECT_FALSE(greedy.trace.steps[i].rationale.empty());
    }

    EXPECT_EQ(optimal.trace.steps.size(), 2);
    for (size_t i = 0; i < optimal.trace.steps.size(); ++i) {
        EXPECT_EQ(optimal.trace.steps[i].step_number, static_cast<int>(i + 1));
        EXPECT_FALSE(optimal.trace.steps[i].rationale.empty());
    }
}

// ==========================================
// 7. Complexity Metadata
// ==========================================

TEST_F(CoinChangeTest, ComplexityInfoMetadata) {
    auto report = problem.complexity_info();
    EXPECT_EQ(report.problem_name, "Coin Change");
    ASSERT_EQ(report.solvers.size(), 2);

    EXPECT_NE(report.solvers[0].time_worst.find("O("), std::string::npos);
    EXPECT_NE(report.solvers[1].time_worst.find("O("), std::string::npos);
    EXPECT_FALSE(report.solvers[0].explanation.empty());
    EXPECT_FALSE(report.solvers[1].explanation.empty());
}
