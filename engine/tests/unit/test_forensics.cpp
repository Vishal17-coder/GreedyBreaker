#include <gtest/gtest.h>
#include "greedybreaker/forensics/forensics.hpp"
#include "greedybreaker/problems/coin_change.hpp"
#include "greedybreaker/problems/knapsack_01.hpp"

using namespace greedybreaker::forensics;
using namespace greedybreaker::problems;
using namespace greedybreaker::core;

TEST(ForensicsTest, CoinChangeDivergenceIdentified) {
    CoinChangeInstance inst;
    inst.denominations = {1, 3, 4};
    inst.target = 6;

    auto greedy = CoinChangeProblem::solve_greedy(inst);
    auto oracle = CoinChangeProblem::solve_exact_dp(inst);
    auto ver = verify(CoinChangeProblem::objective(), greedy, oracle);

    nlohmann::json inst_json = inst;
    auto report = ForensicAnalyzer::analyze("coin_change", inst_json, greedy, oracle, ver, "Minimal");

    EXPECT_EQ(report.divergence_step_index, 0); // First step selected coin 4!
    EXPECT_NE(report.critical_greedy_action.find("4"), std::string::npos);
    EXPECT_NE(report.optimal_alternative_action.find("3"), std::string::npos);
    EXPECT_EQ(report.evidence_confidence, "HIGH_CONFIDENCE_EVIDENCE");
    EXPECT_FALSE(report.causal_consequence.empty());
}

TEST(ForensicsTest, Knapsack01DivergenceIdentified) {
    Knapsack01Instance inst;
    inst.capacity = 50;
    inst.items = {
        {1, 60.0, 10.0},
        {2, 100.0, 20.0},
        {3, 120.0, 30.0}
    };

    auto greedy = Knapsack01Problem::solve_greedy(inst);
    auto oracle = Knapsack01Problem::solve_exact_dp(inst);
    auto ver = verify(Knapsack01Problem::objective(), greedy, oracle);

    nlohmann::json inst_json = inst;
    auto report = ForensicAnalyzer::analyze("knapsack_01", inst_json, greedy, oracle, ver, "Minimal");

    EXPECT_GE(report.divergence_step_index, 0);
    EXPECT_NE(report.critical_greedy_action.find("1"), std::string::npos);
    EXPECT_EQ(report.evidence_confidence, "HIGH_CONFIDENCE_EVIDENCE");
}
