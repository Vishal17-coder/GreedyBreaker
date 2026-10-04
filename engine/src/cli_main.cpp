#include "greedybreaker/core/json_serialization.hpp"
#include "greedybreaker/search/exhaustive_search.hpp"
#include "greedybreaker/search/backtracking_search.hpp"
#include "greedybreaker/search/randomized_search.hpp"
#include "greedybreaker/search/heuristic_search.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <string>

using namespace greedybreaker;

void print_usage(const char* prog_name) {
    std::cout << "GreedyBreaker Engine CLI — Automated Counterexample Discovery\n\n";
    std::cout << "Usage:\n";
    std::cout << "  " << prog_name << " [options]\n\n";
    std::cout << "Modes:\n";
    std::cout << "  --list-problems               Print metadata of all registered problems\n";
    std::cout << "  --config-json <json_str>      Run search using SearchConfig JSON string\n";
    std::cout << "  --config-file <path>          Run search using SearchConfig JSON file\n";
    std::cout << "  --verify-json <json_str>      Directly verify a candidate instance from JSON\n";
    std::cout << "  --verify-file <path>          Directly verify a candidate instance from file\n\n";
    std::cout << "Search Options:\n";
    std::cout << "  --problem <name>              coin-change | activity-selection | knapsack-01 | knapsack-fractional\n";
    std::cout << "  --strategy <name>             exhaustive | backtracking | randomized | heuristic\n";
    std::cout << "  --mode <name>                 any | smallest | worst\n";
    std::cout << "  --max-candidates <N>          Maximum candidates to evaluate (safety bound)\n";
    std::cout << "  --seed <N>                    PRNG seed for randomized/heuristic search\n";
    std::cout << "  --max-iterations <N>          Maximum iterations for randomized/heuristic search\n\n";
    std::cout << "Output Format:\n";
    std::cout << "  --json                        Format output as structured, machine-readable JSON\n";
    std::cout << "  --pretty                      Pretty-print JSON output\n";
    std::cout << "  --help, -h                    Show this help message\n";
}

int main(int argc, char* argv[]) {
    bool json_output = false;
    bool pretty_json = false;
    bool list_problems = false;

    std::string config_json_str;
    std::string config_file_path;
    std::string verify_json_str;
    std::string verify_file_path;

    SearchConfig config;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        } else if (arg == "--json") {
            json_output = true;
        } else if (arg == "--pretty") {
            pretty_json = true;
        } else if (arg == "--list-problems") {
            list_problems = true;
        } else if (arg == "--config-json" && i + 1 < argc) {
            config_json_str = argv[++i];
        } else if (arg == "--config-file" && i + 1 < argc) {
            config_file_path = argv[++i];
        } else if (arg == "--config-stdin") {
            std::string line;
            std::ostringstream ss;
            while (std::getline(std::cin, line)) {
                ss << line << "\n";
            }
            config_json_str = ss.str();
        } else if (arg == "--verify-json" && i + 1 < argc) {
            verify_json_str = argv[++i];
        } else if (arg == "--verify-file" && i + 1 < argc) {
            verify_file_path = argv[++i];
        } else if (arg == "--verify-stdin") {
            std::string line;
            std::ostringstream ss;
            while (std::getline(std::cin, line)) {
                ss << line << "\n";
            }
            verify_json_str = ss.str();
        } else if (arg == "--problem" && i + 1 < argc) {
            config.problem_type = parse_problem_type(argv[++i]);
        } else if (arg == "--strategy" && i + 1 < argc) {
            std::string s = argv[++i];
            if (s == "backtracking") config.strategy = SearchStrategy::BACKTRACKING;
            else if (s == "randomized") config.strategy = SearchStrategy::RANDOMIZED;
            else if (s == "heuristic") config.strategy = SearchStrategy::HEURISTIC;
            else config.strategy = SearchStrategy::EXHAUSTIVE;
        } else if (arg == "--mode" && i + 1 < argc) {
            std::string m = argv[++i];
            if (m == "smallest") config.mode = SearchMode::SMALLEST;
            else if (m == "worst") config.mode = SearchMode::WORST;
            else config.mode = SearchMode::ANY;
        } else if (arg == "--max-candidates" && i + 1 < argc) {
            config.max_candidates = std::stoull(argv[++i]);
        } else if (arg == "--seed" && i + 1 < argc) {
            config.random_seed = static_cast<uint32_t>(std::stoul(argv[++i]));
        } else if (arg == "--max-iterations" && i + 1 < argc) {
            config.max_iterations = std::stoull(argv[++i]);
        } else if (arg == "--min-denomination" && i + 1 < argc) {
            config.min_denomination = std::stoi(argv[++i]);
        } else if (arg == "--max-denomination" && i + 1 < argc) {
            config.max_denomination = std::stoi(argv[++i]);
        } else if (arg == "--min-coins" && i + 1 < argc) {
            config.min_coin_count = std::stoi(argv[++i]);
        } else if (arg == "--max-coins" && i + 1 < argc) {
            config.max_coin_count = std::stoi(argv[++i]);
        } else if (arg == "--min-target" && i + 1 < argc) {
            config.min_target = std::stoi(argv[++i]);
        } else if (arg == "--max-target" && i + 1 < argc) {
            config.max_target = std::stoi(argv[++i]);
        } else if (arg == "--min-capacity" && i + 1 < argc) {
            config.min_capacity = std::stoi(argv[++i]);
        } else if (arg == "--max-capacity" && i + 1 < argc) {
            config.max_capacity = std::stoi(argv[++i]);
        } else if (arg == "--min-items" && i + 1 < argc) {
            config.min_items = std::stoi(argv[++i]);
        } else if (arg == "--max-items" && i + 1 < argc) {
            config.max_items = std::stoi(argv[++i]);
        } else if (arg == "--min-weight" && i + 1 < argc) {
            config.min_weight = std::stoi(argv[++i]);
        } else if (arg == "--max-weight" && i + 1 < argc) {
            config.max_weight = std::stoi(argv[++i]);
        } else if (arg == "--min-value" && i + 1 < argc) {
            config.min_value = std::stoi(argv[++i]);
        } else if (arg == "--max-value" && i + 1 < argc) {
            config.max_value = std::stoi(argv[++i]);
        } else if (arg == "--min-activities" && i + 1 < argc) {
            config.min_activities = std::stoi(argv[++i]);
        } else if (arg == "--max-activities" && i + 1 < argc) {
            config.max_activities = std::stoi(argv[++i]);
        } else if (arg == "--min-time" && i + 1 < argc) {
            config.min_time = std::stoi(argv[++i]);
        } else if (arg == "--max-time" && i + 1 < argc) {
            config.max_time = std::stoi(argv[++i]);
        } else if (arg == "--min-duration" && i + 1 < argc) {
            config.min_duration = std::stoi(argv[++i]);
        }
    }

    auto output_json = [&](const nlohmann::json& j) {
        if (pretty_json) {
            std::cout << j.dump(2) << "\n";
        } else {
            std::cout << j.dump() << "\n";
        }
    };

    // 1. List problems
    if (list_problems) {
        nlohmann::json problems = problem_registry_to_json();
        output_json(problems);
        return 0;
    }

    // 2. Direct candidate verification
    if (!verify_json_str.empty() || !verify_file_path.empty()) {
        try {
            nlohmann::json v_req;
            if (!verify_file_path.empty()) {
                std::ifstream f(verify_file_path);
                if (!f.is_open()) {
                    auto err = make_error_json("FILE_NOT_FOUND", "Could not open verification file: " + verify_file_path);
                    output_json(err);
                    return 1;
                }
                f >> v_req;
            } else {
                v_req = nlohmann::json::parse(verify_json_str);
            }

            auto res = verify_candidate_json(v_req);
            output_json(res);
            return (res.value("status", "") == "error") ? 1 : 0;
        } catch (const std::exception& ex) {
            auto err = make_error_json("VERIFICATION_ERROR", ex.what());
            output_json(err);
            return 1;
        }
    }

    // 3. Counterexample search
    try {
        if (!config_file_path.empty()) {
            std::ifstream f(config_file_path);
            if (!f.is_open()) {
                auto err = make_error_json("FILE_NOT_FOUND", "Could not open config file: " + config_file_path);
                output_json(err);
                return 1;
            }
            nlohmann::json cfg_j;
            f >> cfg_j;
            config = search_config_from_json(cfg_j);
        } else if (!config_json_str.empty()) {
            nlohmann::json cfg_j = nlohmann::json::parse(config_json_str);
            config = search_config_from_json(cfg_j);
        }

        // Validate configuration bounds
        config.validate();

        std::unique_ptr<SearchEngine> engine;
        switch (config.strategy) {
            case SearchStrategy::EXHAUSTIVE:
                engine = std::make_unique<ExhaustiveSearch>();
                break;
            case SearchStrategy::BACKTRACKING:
                engine = std::make_unique<BacktrackingSearch>();
                break;
            case SearchStrategy::RANDOMIZED:
                engine = std::make_unique<RandomizedSearch>();
                break;
            case SearchStrategy::HEURISTIC:
                engine = std::make_unique<HeuristicSearch>();
                break;
        }

        SearchResult result = engine->search(config);

        if (json_output || !config_json_str.empty() || !config_file_path.empty()) {
            nlohmann::json res_j = to_json(result, config);
            output_json(res_j);
        } else {
            // Human-readable summary if not requested as JSON
            std::cout << "Problem: " << to_string(config.problem_type) << "\n";
            std::cout << "Strategy: " << to_string(config.strategy) << " | Mode: " << to_string(config.mode) << "\n";
            std::cout << "Candidates Evaluated: " << result.candidates_evaluated << "\n";
            std::cout << "Counterexamples Found: " << result.counterexamples_found << "\n";
            std::cout << "Execution Time: " << result.duration_ms << " ms\n";
            std::cout << "Termination: " << result.termination_reason << "\n";
            if (result.found) {
                std::cout << "Counterexample Discovered: YES\n";
                if (!result.candidate_summary.empty()) {
                    std::cout << "Instance: " << result.candidate_summary << "\n";
                }
            } else {
                std::cout << "No counterexample discovered within configured bounds.\n";
            }
        }
        return 0;

    } catch (const std::exception& ex) {
        if (json_output || !config_json_str.empty() || !config_file_path.empty()) {
            auto err = make_error_json("SEARCH_ERROR", ex.what());
            output_json(err);
        } else {
            std::cerr << "Error: " << ex.what() << "\n";
        }
        return 1;
    }
}
