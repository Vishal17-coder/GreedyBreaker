#include <gtest/gtest.h>
#include "greedybreaker/core/trace.hpp"

using namespace greedybreaker;

TEST(TraceTest, TraceStepFields) {
    TraceStep step;
    step.step_number = 1;
    step.state_description = "Remaining target: 6";
    step.remaining_target = 6;
    step.available_choices = {1, 3, 4};
    step.selected_choice = 4;
    step.outcome_description = "Selected coin 4. Remaining reduced to 2.";
    step.remaining_after = 2;
    step.accumulated_count = 1;
    step.rationale = "Chose largest coin 4 <= 6.";

    EXPECT_EQ(step.step_number, 1);
    EXPECT_EQ(step.remaining_target, 6);
    EXPECT_EQ(step.selected_choice.value(), 4);
    EXPECT_EQ(step.remaining_after, 2);
    EXPECT_EQ(step.accumulated_count, 1);
    EXPECT_EQ(step.available_choices.size(), 3);
}

TEST(TraceTest, AlgorithmTraceFields) {
    AlgorithmTrace trace;
    trace.algorithm_name = "Greedy";
    trace.strategy_name = "Largest Denomination First";
    trace.success = true;
    trace.final_solution = {4, 1, 1};
    trace.final_cost = 3.0;
    trace.execution_notes = {"Completed successfully"};

    EXPECT_EQ(trace.algorithm_name, "Greedy");
    EXPECT_TRUE(trace.success);
    ASSERT_TRUE(trace.final_cost.has_value());
    EXPECT_DOUBLE_EQ(trace.final_cost.value(), 3.0);
    EXPECT_EQ(trace.final_solution.size(), 3);
}

TEST(TraceTest, DivergenceResultFields) {
    DivergenceResult div;
    div.has_diverged = true;
    div.divergence_step = 1;
    div.state_remaining = 6;
    div.greedy_choice = 4;
    div.optimal_choices = {3};
    div.greedy_projected_cost = 3.0;
    div.optimal_projected_cost = 2.0;
    div.explanation = "Greedy selected 4 instead of 3.";

    EXPECT_TRUE(div.has_diverged);
    ASSERT_TRUE(div.divergence_step.has_value());
    EXPECT_EQ(div.divergence_step.value(), 1);
    ASSERT_TRUE(div.greedy_choice.has_value());
    EXPECT_EQ(div.greedy_choice.value(), 4);
    ASSERT_EQ(div.optimal_choices.size(), 1);
    EXPECT_EQ(div.optimal_choices[0], 3);
}
