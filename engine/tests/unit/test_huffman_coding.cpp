#include <gtest/gtest.h>
#include "greedybreaker/problems/huffman_coding.hpp"
#include "greedybreaker/core/verification_result.hpp"

using namespace greedybreaker::problems;
using namespace greedybreaker::core;

TEST(HuffmanCodingTest, StandardHuffmanMatchesExactOptimum) {
    HuffmanCodingInstance inst;
    inst.symbols = {
        {1, "A", 45},
        {2, "B", 13},
        {3, "C", 12},
        {4, "D", 16},
        {5, "E", 9},
        {6, "F", 5}
    };

    ASSERT_TRUE(HuffmanCodingProblem::is_valid_instance(inst));

    auto greedy = HuffmanCodingProblem::solve_greedy(inst, "standard_huffman");
    auto oracle = HuffmanCodingProblem::solve_exact(inst);

    auto ver = verify(HuffmanCodingProblem::objective(), greedy, oracle);
    EXPECT_EQ(ver.status, VerificationStatus::OPTIMAL);
    EXPECT_FALSE(ver.is_counterexample);
    EXPECT_DOUBLE_EQ(greedy.objective_value, oracle.optimal_value);
}

TEST(HuffmanCodingTest, ShannonFanoHeuristicFailsAndIsDiscovered) {
    // Classic information-theoretic counterexample:
    // Frequencies: {36, 18, 16, 15, 15}
    // Shannon-Fano cost: 230
    // Huffman optimal cost: 228
    HuffmanCodingInstance inst;
    inst.symbols = {
        {1, "A", 36},
        {2, "B", 18},
        {3, "C", 16},
        {4, "D", 15},
        {5, "E", 15}
    };

    auto greedy = HuffmanCodingProblem::solve_greedy(inst, "shannon_fano");
    auto oracle = HuffmanCodingProblem::solve_exact(inst);

    auto ver = verify(HuffmanCodingProblem::objective(), greedy, oracle);
    EXPECT_EQ(ver.status, VerificationStatus::OPTIMALITY_FAILURE);
    EXPECT_TRUE(ver.is_counterexample);
    EXPECT_DOUBLE_EQ(greedy.objective_value, 230.0);
    EXPECT_DOUBLE_EQ(oracle.optimal_value, 228.0);
    EXPECT_DOUBLE_EQ(ver.absolute_gap, 2.0);
}
