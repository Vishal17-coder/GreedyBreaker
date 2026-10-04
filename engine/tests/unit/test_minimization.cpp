#include <gtest/gtest.h>
#include "greedybreaker/minimization/minimizer.hpp"
#include "greedybreaker/problems/coin_change.hpp"
#include "greedybreaker/problems/knapsack_01.hpp"

using namespace greedybreaker::minimization;
using namespace greedybreaker::problems;

TEST(MinimizationTest, CoinChangeReducesRedundantDenominations) {
    // Oversized counterexample containing unnecessary denomination 2:
    // Denominations: {1, 2, 3, 4}, Target = 6
    // Greedy: 4 + 2 = 2 coins (Wait, 4 + 2 = 2 coins is optimal!)
    // Let's use: {1, 2, 3, 5, 6}, Target = 9
    // Greedy picks 6 + 2 + 1 = 3 coins.
    // Optimal picks 6 + 3 = 2 coins or 5 + 4 ...
    // Or {1, 2, 3, 4, 7}, Target = 6:
    // Greedy on {1, 2, 3, 4, 7} for Target 6:
    // 4 <= 6: picks 4, rem 2. Picks 2. Total = 2.
    // Let's construct a verified counterexample with extra coin:
    // Base: {1, 3, 4}, Target = 6 (Greedy: 4 + 1 + 1 = 3; Optimal: 3 + 3 = 2).
    // Now add coin 10: {1, 3, 4, 10}, Target = 6.
    // Coin 10 is redundant (never used for Target 6).
    CoinChangeInstance inst;
    inst.denominations = {1, 3, 4, 10};
    inst.target = 6;

    auto min_res = CounterexampleMinimizer::minimize_coin_change(inst);
    EXPECT_GT(min_res.reduction_steps, 0);
    EXPECT_TRUE(min_res.final_verification.is_counterexample);

    auto final_inst = min_res.minimal_instance.get<CoinChangeInstance>();
    // Coin 10 must have been pruned!
    EXPECT_EQ(final_inst.denominations.size(), 3);
    EXPECT_EQ(final_inst.denominations, std::vector<int64_t>({1, 3, 4}));
    EXPECT_EQ(final_inst.target, 6);
}

TEST(MinimizationTest, Knapsack01ReducesNonEssentialItems) {
    // Core failing items: (60, 10), (100, 20), (120, 30), Capacity = 50
    // Add extra useless item: (1, 15) with very low density
    Knapsack01Instance inst;
    inst.capacity = 50;
    inst.items = {
        {1, 60.0, 10.0},
        {2, 100.0, 20.0},
        {3, 120.0, 30.0},
        {4, 1.0, 15.0} // Useless low-value item
    };

    auto min_res = CounterexampleMinimizer::minimize_knapsack_01(inst);
    EXPECT_GT(min_res.reduction_steps, 0);
    EXPECT_TRUE(min_res.final_verification.is_counterexample);

    auto final_inst = min_res.minimal_instance.get<Knapsack01Instance>();
    EXPECT_LT(final_inst.items.size(), inst.items.size());
}
