#include <gtest/gtest.h>
#include "greedybreaker/core/json_serialization.hpp"
#include "greedybreaker/search/exhaustive_search.hpp"

using namespace greedybreaker;

class JsonCliTest : public ::testing::Test {};

TEST_F(JsonCliTest, SearchConfigRoundTrip) {
    SearchConfig config;
    config.problem_type = ProblemType::KNAPSACK_01;
    config.strategy = SearchStrategy::BACKTRACKING;
    config.mode = SearchMode::WORST;
    config.min_capacity = 8;
    config.max_capacity = 15;
    config.min_items = 4;
    config.max_items = 6;
    config.random_seed = 999;
    config.max_candidates = 25000;

    nlohmann::json j = to_json(config);
    EXPECT_EQ(j["problem_type"], "knapsack-01");
    EXPECT_EQ(j["strategy"], "BACKTRACKING");
    EXPECT_EQ(j["mode"], "WORST");
    EXPECT_EQ(j["min_capacity"], 8);
    EXPECT_EQ(j["max_capacity"], 15);
    EXPECT_EQ(j["random_seed"], 999);
    EXPECT_EQ(j["max_candidates"], 25000);

    SearchConfig reconstructed = search_config_from_json(j);
    EXPECT_EQ(reconstructed.problem_type, ProblemType::KNAPSACK_01);
    EXPECT_EQ(reconstructed.strategy, SearchStrategy::BACKTRACKING);
    EXPECT_EQ(reconstructed.mode, SearchMode::WORST);
    EXPECT_EQ(reconstructed.min_capacity, 8);
    EXPECT_EQ(reconstructed.max_capacity, 15);
    EXPECT_EQ(reconstructed.min_items, 4);
    EXPECT_EQ(reconstructed.max_items, 6);
    EXPECT_EQ(reconstructed.random_seed, 999);
    EXPECT_EQ(reconstructed.max_candidates, 25000);
}

TEST_F(JsonCliTest, ProblemRegistryMetadataSerialization) {
    nlohmann::json reg = problem_registry_to_json();
    ASSERT_TRUE(reg.is_array());
    EXPECT_EQ(reg.size(), 4);

    std::vector<std::string> expected_ids = {
        "coin_change", "activity_selection", "knapsack_01", "knapsack_fractional"
    };

    for (size_t i = 0; i < reg.size(); ++i) {
        EXPECT_EQ(reg[i]["id"], expected_ids[i]);
        EXPECT_TRUE(reg[i].contains("name"));
        EXPECT_TRUE(reg[i].contains("objective"));
        EXPECT_TRUE(reg[i].contains("description"));
        EXPECT_TRUE(reg[i].contains("greedy_strategy"));
        EXPECT_TRUE(reg[i].contains("oracle_strategy"));
        EXPECT_TRUE(reg[i].contains("can_fail"));
        EXPECT_TRUE(reg[i].contains("default_config"));
        EXPECT_TRUE(reg[i].contains("solvers"));
        EXPECT_GT(reg[i]["solvers"].size(), 0);
    }
}

TEST_F(JsonCliTest, VerificationCoinChangeCounterexample) {
    nlohmann::json req = {
        {"problem", "coin_change"},
        {"candidate", {
            {"denominations", {1, 3, 4}},
            {"target", 6}
        }}
    };

    nlohmann::json res = verify_candidate_json(req);
    EXPECT_EQ(res["status"], "success");
    EXPECT_EQ(res["problem"], "coin-change");
    EXPECT_TRUE(res["counterexample_found"].get<bool>());

    EXPECT_EQ(res["greedy_solution"]["cost"], 3.0);
    EXPECT_EQ(res["optimal_solution"]["cost"], 2.0);

    EXPECT_EQ(res["metrics"]["absolute_gap"], 1.0);
    EXPECT_DOUBLE_EQ(res["metrics"]["ratio"].get<double>(), 1.5);
    EXPECT_DOUBLE_EQ(res["metrics"]["overhead_percentage"].get<double>(), 50.0);

    EXPECT_TRUE(res["divergence"]["has_diverged"].get<bool>());
    EXPECT_EQ(res["divergence"]["divergence_step"], 1);
    EXPECT_EQ(res["divergence"]["greedy_choice"], 4);
}

TEST_F(JsonCliTest, VerificationActivitySelectionAlwaysOptimal) {
    nlohmann::json req = {
        {"problem", "activity_selection"},
        {"candidate", {
            {"activities", {
                {{"id", 1}, {"start", 1}, {"finish", 4}},
                {{"id", 2}, {"start", 3}, {"finish", 5}},
                {{"id", 3}, {"start", 0}, {"finish", 6}},
                {{"id", 4}, {"start", 5}, {"finish", 7}}
            }}
        }}
    };

    nlohmann::json res = verify_candidate_json(req);
    EXPECT_EQ(res["status"], "success");
    EXPECT_FALSE(res["counterexample_found"].get<bool>());
    EXPECT_EQ(res["greedy_solution"]["cost"], 2.0);
    EXPECT_EQ(res["optimal_solution"]["cost"], 2.0);
    EXPECT_EQ(res["metrics"]["absolute_gap"], 0.0);
    EXPECT_DOUBLE_EQ(res["metrics"]["ratio"].get<double>(), 1.0);
}

TEST_F(JsonCliTest, VerificationKnapsack01Counterexample) {
    nlohmann::json req = {
        {"problem", "knapsack_01"},
        {"candidate", {
            {"capacity", 5},
            {"items", {
                {{"id", 1}, {"value", 20}, {"weight", 1}},
                {{"id", 2}, {"value", 19}, {"weight", 1}},
                {{"id", 3}, {"value", 20}, {"weight", 4}}
            }}
        }}
    };

    nlohmann::json res = verify_candidate_json(req);
    EXPECT_EQ(res["status"], "success");
    EXPECT_TRUE(res["counterexample_found"].get<bool>());
    EXPECT_EQ(res["greedy_solution"]["cost"], 39.0);
    EXPECT_EQ(res["optimal_solution"]["cost"], 40.0);
    EXPECT_EQ(res["metrics"]["absolute_gap"], 1.0);
}

TEST_F(JsonCliTest, VerificationFractionalKnapsackMatches) {
    nlohmann::json req = {
        {"problem", "knapsack_fractional"},
        {"candidate", {
            {"capacity", 15},
            {"items", {
                {{"id", 1}, {"value", 60}, {"weight", 10}},
                {{"id", 2}, {"value", 100}, {"weight", 20}}
            }}
        }}
    };

    nlohmann::json res = verify_candidate_json(req);
    EXPECT_EQ(res["status"], "success");
    EXPECT_FALSE(res["counterexample_found"].get<bool>());
    EXPECT_DOUBLE_EQ(res["greedy_solution"]["cost"].get<double>(), 85.0);
    EXPECT_DOUBLE_EQ(res["optimal_solution"]["cost"].get<double>(), 85.0);
}

TEST_F(JsonCliTest, SearchResultCoinChangeToJson) {
    ExhaustiveSearch engine;
    SearchConfig config;
    config.problem_type = ProblemType::COIN_CHANGE;
    config.strategy = SearchStrategy::EXHAUSTIVE;
    config.mode = SearchMode::SMALLEST;
    config.min_denomination = 1;
    config.max_denomination = 6;
    config.min_coin_count = 2;
    config.max_coin_count = 3;
    config.min_target = 1;
    config.max_target = 10;

    SearchResult result = engine.search(config);
    nlohmann::json j = to_json(result, config);

    EXPECT_EQ(j["status"], "success");
    EXPECT_EQ(j["problem"], "coin-change");
    EXPECT_TRUE(j["counterexample_found"].get<bool>());
    EXPECT_TRUE(j["minimality"]["verified"].get<bool>());
    EXPECT_TRUE(j.contains("search_statistics"));
    EXPECT_GT(j["search_statistics"]["candidates_evaluated"].get<size_t>(), 0);
    EXPECT_TRUE(j.contains("candidate"));
    EXPECT_TRUE(j.contains("greedy_solution"));
    EXPECT_TRUE(j.contains("optimal_solution"));
    EXPECT_TRUE(j.contains("metrics"));
    EXPECT_TRUE(j.contains("divergence"));
    EXPECT_EQ(j["error"], nullptr);
}

TEST_F(JsonCliTest, SearchResultNoCounterexampleToJson) {
    ExhaustiveSearch engine;
    SearchConfig config;
    config.problem_type = ProblemType::ACTIVITY_SELECTION;
    config.strategy = SearchStrategy::EXHAUSTIVE;
    config.mode = SearchMode::ANY;
    config.min_activities = 3;
    config.max_activities = 3;
    config.min_time = 0;
    config.max_time = 5;

    SearchResult result = engine.search(config);
    nlohmann::json j = to_json(result, config);

    EXPECT_EQ(j["status"], "success");
    EXPECT_EQ(j["problem"], "activity-selection");
    EXPECT_FALSE(j["counterexample_found"].get<bool>());
    EXPECT_EQ(j["message"], "No counterexample was found within the configured search space.");
    EXPECT_FALSE(j["minimality"]["verified"].get<bool>());
    EXPECT_EQ(j["candidate"], nullptr);
    EXPECT_EQ(j["greedy_solution"], nullptr);
    EXPECT_EQ(j["optimal_solution"], nullptr);
    EXPECT_EQ(j["metrics"], nullptr);
    EXPECT_EQ(j["divergence"], nullptr);
    EXPECT_EQ(j["error"], nullptr);
}

TEST_F(JsonCliTest, ErrorJsonStructure) {
    nlohmann::json err = make_error_json("INVALID_BOUNDS", "Min target exceeds max target.");
    EXPECT_EQ(err["status"], "error");
    EXPECT_EQ(err["error"]["code"], "INVALID_BOUNDS");
    EXPECT_EQ(err["error"]["message"], "Min target exceeds max target.");
}
