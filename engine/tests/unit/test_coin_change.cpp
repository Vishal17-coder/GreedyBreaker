#include <gtest/gtest.h>
#include "greedybreaker/problems/coin_change.hpp"
#include "greedybreaker/core/verification_result.hpp"

using namespace greedybreaker::problems;
using namespace greedybreaker::core;

TEST(CoinChangeTest, CanonicalFailureDiscovered) {
    // Canonical textbook counterexample: {1, 3, 4}, Target = 6
    CoinChangeInstance inst;
    inst.denominations = {1, 3, 4};
    inst.target = 6;

    ASSERT_TRUE(CoinChangeProblem::is_valid_instance(inst));

    auto greedy = CoinChangeProblem::solve_greedy(inst);
    EXPECT_TRUE(greedy.feasible);
    EXPECT_DOUBLE_EQ(greedy.objective_value, 3.0); // 4 + 1 + 1

    auto oracle = CoinChangeProblem::solve_exact_dp(inst);
    EXPECT_TRUE(oracle.is_exact());
    EXPECT_DOUBLE_EQ(oracle.optimal_value, 2.0); // 3 + 3

    auto ver = verify(CoinChangeProblem::objective(), greedy, oracle);
    EXPECT_EQ(ver.status, VerificationStatus::OPTIMALITY_FAILURE);
    EXPECT_TRUE(ver.is_counterexample);
    EXPECT_DOUBLE_EQ(ver.absolute_gap, 1.0);
    EXPECT_DOUBLE_EQ(ver.relative_gap, 1.5);
    EXPECT_DOUBLE_EQ(ver.failure_percent, 50.0);
}

TEST(CoinChangeTest, CanonicalOptimalInstance) {
    // US currency is canonical: greedy is optimal
    CoinChangeInstance inst;
    inst.denominations = {1, 5, 10, 25};
    inst.target = 30;

    auto greedy = CoinChangeProblem::solve_greedy(inst);
    auto oracle = CoinChangeProblem::solve_exact_dp(inst);

    auto ver = verify(CoinChangeProblem::objective(), greedy, oracle);
    EXPECT_EQ(ver.status, VerificationStatus::OPTIMAL);
    EXPECT_FALSE(ver.is_counterexample);
    EXPECT_DOUBLE_EQ(greedy.objective_value, 2.0);
    EXPECT_DOUBLE_EQ(oracle.optimal_value, 2.0);
}

TEST(CoinChangeTest, DecisionTraceValidation) {
    CoinChangeInstance inst;
    inst.denominations = {1, 3, 4};
    inst.target = 6;

    auto greedy = CoinChangeProblem::solve_greedy(inst);
    const auto& trace = greedy.trace;

    ASSERT_EQ(trace.size(), 3);
    EXPECT_EQ(trace.step_at(0).action_description, "Selected coin 4");
    EXPECT_EQ(trace.step_at(0).state_before, "Remaining target: 6");
    EXPECT_EQ(trace.step_at(0).state_after, "Remaining target: 2");

    EXPECT_EQ(trace.step_at(1).action_description, "Selected coin 1");
    EXPECT_EQ(trace.step_at(2).action_description, "Selected coin 1");
}

TEST(CoinChangeTest, OracleCrossValidationDPvsBFS) {
    std::vector<CoinChangeInstance> test_instances = {
        {{1, 3, 4}, 6},
        {{1, 3, 4}, 11},
        {{1, 4, 5, 10}, 12},
        {{1, 7, 10}, 14},
        {{1, 2, 5, 10, 20}, 43}
    };

    for (const auto& inst : test_instances) {
        auto dp_res = CoinChangeProblem::solve_exact_dp(inst);
        auto bfs_res = CoinChangeProblem::solve_exact_bfs(inst);

        EXPECT_DOUBLE_EQ(dp_res.optimal_value, bfs_res.optimal_value)
            << "Cross-validation mismatch for target " << inst.target;
    }
}
