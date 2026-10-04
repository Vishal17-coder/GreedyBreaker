#include "greedybreaker/search/search_config.hpp"
#include "greedybreaker/search/search_result.hpp"
#include "greedybreaker/serialization/json_adapters.hpp"

namespace greedybreaker::search {

void to_json(nlohmann::json& j, const SearchConfig& cfg) {
    j = nlohmann::json{
        {"experiment_id", cfg.experiment_id},
        {"problem_type", cfg.problem_type},
        {"strategy_id", cfg.strategy_id},
        {"oracle_id", cfg.oracle_id},
        {"mode", std::string(to_string(cfg.mode))},
        {"strategy", std::string(to_string(cfg.strategy))},
        {"random_seed", cfg.random_seed},
        {"max_candidates", cfg.max_candidates},
        {"max_time_ms", cfg.max_time_ms},
        {"search_space", cfg.search_space}
    };
}

void from_json(const nlohmann::json& j, SearchConfig& cfg) {
    cfg.experiment_id = j.value("experiment_id", "exp_default");
    cfg.problem_type = j.value("problem_type", "coin_change");
    cfg.strategy_id = j.value("strategy_id", "default");
    cfg.oracle_id = j.value("oracle_id", "default");

    std::string m = j.value("mode", "ANY");
    if (m == "SMALLEST") cfg.mode = SearchMode::SMALLEST;
    else if (m == "WORST") cfg.mode = SearchMode::WORST;
    else if (m == "PROFILE") cfg.mode = SearchMode::PROFILE;
    else cfg.mode = SearchMode::ANY;

    std::string s = j.value("strategy", "EXHAUSTIVE");
    if (s == "RANDOMIZED") cfg.strategy = SearchStrategy::RANDOMIZED;
    else if (s == "GUIDED") cfg.strategy = SearchStrategy::GUIDED;
    else if (s == "HYBRID") cfg.strategy = SearchStrategy::HYBRID;
    else cfg.strategy = SearchStrategy::EXHAUSTIVE;

    cfg.random_seed = j.value("random_seed", uint64_t{42});
    cfg.max_candidates = j.value("max_candidates", uint64_t{10'000});
    cfg.max_time_ms = j.value("max_time_ms", uint64_t{5'000});
    if (j.contains("search_space")) {
        cfg.search_space = j.at("search_space");
    }
}

void to_json(nlohmann::json& j, const SearchResult& r) {
    j = nlohmann::json{
        {"status_code", static_cast<int>(r.status.code())},
        {"status_message", r.status.message()},
        {"experiment_id", r.experiment_id},
        {"problem_type", r.problem_type},
        {"strategy_id", r.strategy_id},
        {"oracle_id", r.oracle_id},
        {"mode", std::string(to_string(r.mode))},
        {"strategy", std::string(to_string(r.strategy))},
        {"coverage", std::string(to_string(r.coverage))},
        {"estimated_space_size", r.estimated_space_size},
        {"instances_evaluated", r.instances_evaluated},
        {"counterexamples_found", r.counterexamples_found},
        {"failure_rate", r.failure_rate},
        {"average_gap", r.average_gap},
        {"worst_gap", r.worst_gap},
        {"elapsed_time_ms", r.elapsed_time_ms},
        {"coverage_ratio", r.coverage_ratio},
        {"has_counterexample", r.has_counterexample},
        {"reproducibility_token", r.reproducibility_token}
    };
    if (r.has_counterexample) {
        j["best_instance"] = r.best_instance;
        j["best_greedy"] = r.best_greedy;
        j["best_oracle"] = r.best_oracle;
        j["best_verification"] = r.best_verification;
    }
}

void from_json(const nlohmann::json& j, SearchResult& r) {
    r.experiment_id = j.value("experiment_id", "");
    r.problem_type = j.value("problem_type", "");
    r.strategy_id = j.value("strategy_id", "");
    r.oracle_id = j.value("oracle_id", "");
    r.estimated_space_size = j.value("estimated_space_size", uint64_t{0});
    r.instances_evaluated = j.value("instances_evaluated", uint64_t{0});
    r.counterexamples_found = j.value("counterexamples_found", uint64_t{0});
    r.failure_rate = j.value("failure_rate", 0.0);
    r.average_gap = j.value("average_gap", 0.0);
    r.worst_gap = j.value("worst_gap", 0.0);
    r.elapsed_time_ms = j.value("elapsed_time_ms", 0.0);
    r.coverage_ratio = j.value("coverage_ratio", 0.0);
    r.has_counterexample = j.value("has_counterexample", false);
    if (r.has_counterexample) {
        if (j.contains("best_instance")) r.best_instance = j.at("best_instance");
        if (j.contains("best_greedy")) r.best_greedy = j.at("best_greedy").get<core::GreedyResult>();
        if (j.contains("best_oracle")) r.best_oracle = j.at("best_oracle").get<core::OracleResult>();
        if (j.contains("best_verification")) r.best_verification = j.at("best_verification").get<core::VerificationResult>();
    }
    if (j.contains("reproducibility_token")) {
        r.reproducibility_token = j.at("reproducibility_token");
    }
}

} // namespace greedybreaker::search
