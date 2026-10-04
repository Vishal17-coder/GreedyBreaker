#include <gtest/gtest.h>
#include "greedybreaker/serialization/json_adapters.hpp"

using namespace greedybreaker::core;

TEST(SerializationTest, ObjectiveDirectionRoundtrip) {
    nlohmann::json j_min = ObjectiveDirection::MINIMIZE;
    EXPECT_EQ(j_min.get<std::string>(), "MINIMIZE");
    EXPECT_EQ(j_min.get<ObjectiveDirection>(), ObjectiveDirection::MINIMIZE);

    nlohmann::json j_max = ObjectiveDirection::MAXIMIZE;
    EXPECT_EQ(j_max.get<std::string>(), "MAXIMIZE");
    EXPECT_EQ(j_max.get<ObjectiveDirection>(), ObjectiveDirection::MAXIMIZE);
}

TEST(SerializationTest, DecisionTraceRoundtrip) {
    DecisionTrace trace;
    DecisionStep step0;
    step0.step_index = 0;
    step0.action_description = "Pick coin 4";
    step0.heuristic_rule = "Largest coin <= remaining target";
    step0.candidates_considered.push_back({"coin_4", 4.0, true, "Valid"});
    step0.candidates_considered.push_back({"coin_3", 3.0, true, "Valid"});
    step0.state_before = "Target: 6";
    step0.state_after = "Target: 2";
    step0.cumulative_objective = 1.0;
    trace.add_step(step0);

    nlohmann::json j = trace;
    EXPECT_TRUE(j.contains("steps"));
    EXPECT_EQ(j["total_steps"], 1);

    auto deserialized = j.get<DecisionTrace>();
    EXPECT_EQ(deserialized.size(), 1);
    EXPECT_EQ(deserialized.step_at(0).action_description, "Pick coin 4");
    EXPECT_EQ(deserialized.step_at(0).candidates_considered.size(), 2);
}

TEST(SerializationTest, VerificationResultRoundtrip) {
    VerificationResult vr;
    vr.status = VerificationStatus::OPTIMALITY_FAILURE;
    vr.is_counterexample = true;
    vr.greedy_value = 3.0;
    vr.optimal_value = 2.0;
    vr.absolute_gap = 1.0;
    vr.relative_gap = 1.5;
    vr.failure_percent = 50.0;
    vr.summary = "Greedy failed";

    nlohmann::json j = vr;
    EXPECT_EQ(j["status"], "OPTIMALITY_FAILURE");
    EXPECT_EQ(j["is_counterexample"], true);

    auto deserialized = j.get<VerificationResult>();
    EXPECT_EQ(deserialized.status, VerificationStatus::OPTIMALITY_FAILURE);
    EXPECT_TRUE(deserialized.is_counterexample);
    EXPECT_DOUBLE_EQ(deserialized.absolute_gap, 1.0);
}
