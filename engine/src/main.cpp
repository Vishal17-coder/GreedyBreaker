#include "greedybreaker/search/exhaustive_search.hpp"
#include "greedybreaker/search/backtracking_search.hpp"
#include "greedybreaker/search/randomized_search.hpp"
#include "greedybreaker/search/heuristic_search.hpp"
#include "greedybreaker/core/registry.hpp"
#include <iostream>
#include <iomanip>
#include <memory>
#include <string>

int main(int argc, char* argv[]) {
    using namespace greedybreaker;

    std::cout << "====================================================\n";
    std::cout << "           GREEDY BREAKER — SEARCH ENGINE           \n";
    std::cout << "  Automated Counterexample Discovery for DAA Core   \n";
    std::cout << "====================================================\n\n";

    SearchConfig config;
    config.strategy = SearchStrategy::EXHAUSTIVE;
    config.mode = SearchMode::SMALLEST;
    config.max_candidates = 50000;

    int arg_offset = 1;

    // Check if first arg specifies problem type
    if (argc >= 2) {
        std::string first_arg = argv[1];
        if (first_arg == "coin-change" || first_arg == "coin_change" ||
            first_arg == "activity-selection" || first_arg == "activity_selection" ||
            first_arg == "knapsack-01" || first_arg == "knapsack_01" ||
            first_arg == "knapsack-fractional" || first_arg == "knapsack_fractional") {
            config.problem_type = parse_problem_type(first_arg);
            arg_offset = 2;
        }
    }

    // Parse strategy
    if (argc >= arg_offset + 1) {
        std::string strat = argv[arg_offset];
        if (strat == "backtracking") config.strategy = SearchStrategy::BACKTRACKING;
        else if (strat == "randomized") config.strategy = SearchStrategy::RANDOMIZED;
        else if (strat == "heuristic") config.strategy = SearchStrategy::HEURISTIC;
        else config.strategy = SearchStrategy::EXHAUSTIVE;
    }

    // Parse mode
    if (argc >= arg_offset + 2) {
        std::string m = argv[arg_offset + 1];
        if (m == "any") config.mode = SearchMode::ANY;
        else if (m == "worst") config.mode = SearchMode::WORST;
        else config.mode = SearchMode::SMALLEST;
    }

    std::cout << "Problem: " << to_string(config.problem_type) << "\n";
    std::cout << "Strategy: " << to_string(config.strategy) << "\n";
    std::cout << "Mode: " << to_string(config.mode) << "\n\n";

    std::cout << "Search Bounds:\n";
    if (config.problem_type == ProblemType::COIN_CHANGE) {
        std::cout << "  Denominations: [" << config.min_denomination << ".." << config.max_denomination << "]\n";
        std::cout << "  Coin Count: [" << config.min_coin_count << ".." << config.max_coin_count << "]\n";
        std::cout << "  Target Range: [" << config.min_target << ".." << config.max_target << "]\n";
    } else if (config.problem_type == ProblemType::ACTIVITY_SELECTION) {
        std::cout << "  Activities Count: [" << config.min_activities << ".." << config.max_activities << "]\n";
        std::cout << "  Time Span: [" << config.min_time << ".." << config.max_time << "]\n";
        std::cout << "  Min Duration: " << config.min_duration << "\n";
    } else {
        std::cout << "  Items Count: [" << config.min_items << ".." << config.max_items << "]\n";
        std::cout << "  Weight: [" << config.min_weight << ".." << config.max_weight << "]\n";
        std::cout << "  Value: [" << config.min_value << ".." << config.max_value << "]\n";
        std::cout << "  Capacity: [" << config.min_capacity << ".." << config.max_capacity << "]\n";
    }
    std::cout << "  Max Safety Limit: " << config.max_candidates << " candidates\n";
    if (config.strategy == SearchStrategy::RANDOMIZED) {
        std::cout << "  PRNG Seed: " << config.random_seed << " | Max Iterations: " << config.max_iterations << "\n";
    }
    std::cout << "\nSearching for counterexamples...\n\n";

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

    std::cout << "Search Statistics:\n";
    std::cout << "  Candidates Generated: " << result.candidates_generated << "\n";
    std::cout << "  Candidates Evaluated: " << result.candidates_evaluated << "\n";
    if (config.strategy == SearchStrategy::BACKTRACKING) {
        std::cout << "  Nodes Explored: " << result.nodes_explored << "\n";
        std::cout << "  Branches Pruned: " << result.branches_pruned << "\n";
    }
    if (config.strategy == SearchStrategy::RANDOMIZED) {
        std::cout << "  Iterations: " << result.iterations << "\n";
    }
    std::cout << "  Counterexamples Discovered: " << result.counterexamples_found << "\n";
    std::cout << "  Execution Time: " << std::fixed << std::setprecision(3) << result.duration_ms << " ms\n";
    std::cout << "  Termination Reason: " << result.termination_reason << "\n\n";

    if (!result.found) {
        std::cout << "----------------------------------------------------\n";
        std::cout << "No counterexample discovered within configured bounds.\n";
        if (config.problem_type == ProblemType::ACTIVITY_SELECTION) {
            std::cout << "Empirical verification: Earliest-Finish-Time greedy found zero counterexamples.\n";
            std::cout << "Note: Zero counterexamples within bounded search space does not constitute universal proof.\n";
        } else if (config.problem_type == ProblemType::KNAPSACK_FRACTIONAL) {
            std::cout << "Empirical verification: Fractional Knapsack continuous ratio greedy found zero counterexamples.\n";
            std::cout << "Note: Zero counterexamples within bounded search space does not constitute universal proof.\n";
        }
        std::cout << "----------------------------------------------------\n";
        return 0;
    }

    std::cout << "----------------------------------------------------\n";
    std::cout << "           COUNTEREXAMPLE DISCOVERED                \n";
    std::cout << "----------------------------------------------------\n\n";

    if (config.problem_type == ProblemType::COIN_CHANGE && result.best_candidate.has_value()) {
        const auto& cand = result.best_candidate.value();
        std::cout << "Coins: ";
        for (size_t i = 0; i < cand.denominations.size(); ++i) {
            if (i > 0) std::cout << " ";
            std::cout << cand.denominations[i];
        }
        std::cout << "\nTarget: " << cand.target << "\n\n";

        const auto& greedy = result.greedy_solution.value();
        const auto& optimal = result.optimal_solution.value();
        const auto& metrics = result.comparison_metrics.value();
        const auto& div = result.first_divergence.value();

        std::cout << "Greedy:\n  ";
        for (size_t i = 0; i < greedy.items.size(); ++i) {
            if (i > 0) std::cout << " + ";
            std::cout << greedy.items[i];
        }
        std::cout << "\n  Cost: " << static_cast<int>(greedy.cost.value()) << " coins\n\n";

        std::cout << "Optimal:\n  ";
        for (size_t i = 0; i < optimal.items.size(); ++i) {
            if (i > 0) std::cout << " + ";
            std::cout << optimal.items[i];
        }
        std::cout << "\n  Cost: " << static_cast<int>(optimal.cost.value()) << " coins\n\n";

        std::cout << "Failure Severity Metrics:\n";
        std::cout << "  Absolute Gap: +" << static_cast<int>(metrics.absolute_gap.value()) << " coin(s)\n";
        std::cout << "  Competitiveness Ratio: " << std::fixed << std::setprecision(2) << metrics.ratio.value() << "x\n";
        std::cout << "  Overhead: " << metrics.overhead_percentage.value() << "%\n\n";

        std::cout << "First Divergence Analysis:\n";
        if (div.has_diverged) {
            std::cout << "  Divergence Step: " << div.divergence_step.value() << "\n";
            std::cout << "  State at Divergence: remaining target " << div.state_remaining << "\n";
            std::cout << "  Greedy Choice: " << (div.greedy_choice.has_value() ? std::to_string(div.greedy_choice.value()) : "none") << "\n";
            std::cout << "  Optimal-Compatible Choice(s): {";
            for (size_t i = 0; i < div.optimal_choices.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << div.optimal_choices[i];
            }
            std::cout << "}\n\n";
            std::cout << "Forensic Rationale:\n  " << div.explanation << "\n\n";
        }
    } else if (config.problem_type == ProblemType::KNAPSACK_01 && result.best_knapsack_candidate.has_value()) {
        const auto& cand = result.best_knapsack_candidate.value();
        std::cout << "Problem: 0/1 Knapsack\n";
        std::cout << "Capacity W: " << cand.capacity << "\n\n";

        std::cout << "Items:\n";
        for (const auto& item : cand.items) {
            std::cout << "  Item " << item.id << ": value=" << item.value
                      << ", weight=" << item.weight
                      << ", ratio=" << std::fixed << std::setprecision(2) << item.ratio() << "\n";
        }
        std::cout << "\n";

        const auto& greedy = result.greedy_knapsack_solution.value();
        const auto& optimal = result.optimal_knapsack_solution.value();
        const auto& metrics = result.comparison_metrics.value();
        const auto& div = result.first_divergence.value();

        std::cout << "Greedy:\n  Items Selected: [";
        int greedy_weight = 0;
        for (size_t i = 0; i < greedy.items.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << "Item " << greedy.items[i].id;
            greedy_weight += greedy.items[i].weight;
        }
        std::cout << "]\n";
        std::cout << "  Greedy Value: " << static_cast<int>(greedy.cost.value()) << "\n";
        std::cout << "  Total Weight: " << greedy_weight << " / " << cand.capacity << "\n\n";

        std::cout << "Optimal:\n  Items Selected: [";
        int opt_weight = 0;
        for (size_t i = 0; i < optimal.items.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << "Item " << optimal.items[i].id;
            opt_weight += optimal.items[i].weight;
        }
        std::cout << "]\n";
        std::cout << "  Optimal Value: " << static_cast<int>(optimal.cost.value()) << "\n";
        std::cout << "  Total Weight: " << opt_weight << " / " << cand.capacity << "\n\n";

        std::cout << "Counterexample: YES\n\n";

        std::cout << "Failure Severity Metrics:\n";
        std::cout << "  Absolute Gap: " << static_cast<int>(metrics.absolute_gap.value()) << " value units\n";
        std::cout << "  Ratio (Optimal / Greedy): " << std::fixed << std::setprecision(2) << metrics.ratio.value() << "x\n";
        std::cout << "  Deficit Percentage: " << metrics.overhead_percentage.value() << "%\n\n";

        std::cout << "First Divergence Analysis:\n";
        if (div.has_diverged) {
            std::cout << "  Divergence Step: " << div.divergence_step.value() << "\n";
            std::cout << "  Forensic Rationale: " << div.explanation << "\n\n";
        }
    }

    std::cout << "Minimality Verification:\n";
    if (result.smallest_verified) {
        std::cout << "  [VERIFIED] Smallest counterexample within the configured search bounds and canonical ordering.\n";
    } else {
        std::cout << "  [UNVERIFIED] Not proven minimal (mode=" << to_string(config.mode)
                  << ", strategy=" << to_string(config.strategy) << ").\n";
    }

    std::cout << "\n====================================================\n";
    return 0;
}
