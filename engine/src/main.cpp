#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <nlohmann/json.hpp>

#include "greedybreaker/core/error.hpp"
#include "greedybreaker/core/types.hpp"
#include "greedybreaker/search/search_engine.hpp"
#include "greedybreaker/minimization/minimizer.hpp"
#include "greedybreaker/forensics/forensics.hpp"

using namespace greedybreaker;

int main(int argc, char* argv[]) {
    // Enable fast I/O
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    if (argc >= 2 && (std::string(argv[1]) == "--version" || std::string(argv[1]) == "-v")) {
        nlohmann::json v_info{
            {"engine", "GreedyBreaker Core"},
            {"version", "2.0.0"},
            {"standard", "C++20"},
            {"problems", {"coin_change", "knapsack_01", "activity_selection", "fractional_knapsack", "huffman_coding"}},
            {"modes", {"ANY", "SMALLEST", "WORST", "PROFILE"}},
            {"strategies", {"EXHAUSTIVE", "RANDOMIZED", "GUIDED", "HYBRID"}}
        };
        std::cout << v_info.dump(2) << std::endl;
        return 0;
    }

    std::string input_content;
    if (argc >= 3 && std::string(argv[1]) == "--config") {
        std::ifstream file(argv[2]);
        if (!file.is_open()) {
            nlohmann::json err{
                {"status_code", static_cast<int>(core::ErrorCode::INVALID_CONFIGURATION)},
                {"error", "Failed to open config file: " + std::string(argv[2])}
            };
            std::cerr << err.dump(2) << std::endl;
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        input_content = buffer.str();
    } else {
        // Read from standard input
        std::stringstream buffer;
        buffer << std::cin.rdbuf();
        input_content = buffer.str();
    }

    if (input_content.empty()) {
        std::cout << "GreedyBreaker Engine v2.0.0 (Usage: greedybreaker_cli --config <path.json> or pass JSON via stdin)\n";
        return 0;
    }

    try {
        nlohmann::json req_json = nlohmann::json::parse(input_content);
        search::SearchConfig config = req_json.get<search::SearchConfig>();

        // 1. Run Search Engine
        search::SearchResult search_res = search::SearchEngine::run(config);

        nlohmann::json final_output;
        final_output["search_result"] = search_res;

        // 2. If counterexample found, run Minimization and Forensic Analysis
        if (search_res.has_counterexample) {
            auto min_res = minimization::CounterexampleMinimizer::minimize(
                config.problem_type,
                search_res.best_instance,
                config.strategy_id
            );
            final_output["minimization_result"] = min_res;

            auto forensic_rep = forensics::ForensicAnalyzer::analyze(
                config.problem_type,
                min_res.minimal_instance,
                search_res.best_greedy,
                search_res.best_oracle,
                search_res.best_verification,
                min_res.minimality_status
            );
            final_output["forensic_report"] = forensic_rep;
        }

        std::cout << final_output.dump(2) << std::endl;
        return 0;
    } catch (const std::exception& e) {
        nlohmann::json err{
            {"status_code", static_cast<int>(core::ErrorCode::INTERNAL_ERROR)},
            {"error", std::string("Engine exception: ") + e.what()}
        };
        std::cerr << err.dump(2) << std::endl;
        return 1;
    }
}
