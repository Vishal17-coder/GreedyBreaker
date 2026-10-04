#include "greedybreaker/core/json_serialization.hpp"
#include <algorithm>
#include <cctype>

namespace greedybreaker {

static std::string to_upper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
    return s;
}

nlohmann::json to_json(const SearchConfig& config) {
    nlohmann::json j;
    j["problem_type"] = to_slug(config.problem_type);
    j["strategy"] = to_string(config.strategy);
    j["mode"] = to_string(config.mode);

    // Coin change bounds
    j["min_denomination"] = config.min_denomination;
    j["max_denomination"] = config.max_denomination;
    j["min_coin_count"] = config.min_coin_count;
    j["max_coin_count"] = config.max_coin_count;
    j["min_target"] = config.min_target;
    j["max_target"] = config.max_target;
    j["require_unit_coin"] = config.require_unit_coin;

    // Activity selection bounds
    j["min_activities"] = config.min_activities;
    j["max_activities"] = config.max_activities;
    j["min_time"] = config.min_time;
    j["max_time"] = config.max_time;
    j["min_duration"] = config.min_duration;

    // Knapsack bounds
    j["min_items"] = config.min_items;
    j["max_items"] = config.max_items;
    j["min_weight"] = config.min_weight;
    j["max_weight"] = config.max_weight;
    j["min_value"] = config.min_value;
    j["max_value"] = config.max_value;
    j["min_capacity"] = config.min_capacity;
    j["max_capacity"] = config.max_capacity;

    // Randomized / safety bounds
    j["random_seed"] = config.random_seed;
    j["max_iterations"] = config.max_iterations;
    j["max_candidates"] = config.max_candidates;

    return j;
}

SearchConfig search_config_from_json(const nlohmann::json& j) {
    SearchConfig config;

    if (j.contains("problem_type")) {
        config.problem_type = parse_problem_type(j["problem_type"].get<std::string>());
    } else if (j.contains("problem")) {
        config.problem_type = parse_problem_type(j["problem"].get<std::string>());
    }

    if (j.contains("strategy")) {
        std::string s = to_upper(j["strategy"].get<std::string>());
        if (s == "BACKTRACKING") config.strategy = SearchStrategy::BACKTRACKING;
        else if (s == "RANDOMIZED") config.strategy = SearchStrategy::RANDOMIZED;
        else if (s == "HEURISTIC") config.strategy = SearchStrategy::HEURISTIC;
        else config.strategy = SearchStrategy::EXHAUSTIVE;
    }

    if (j.contains("mode")) {
        std::string m = to_upper(j["mode"].get<std::string>());
        if (m == "SMALLEST") config.mode = SearchMode::SMALLEST;
        else if (m == "WORST") config.mode = SearchMode::WORST;
        else config.mode = SearchMode::ANY;
    }

    // Coin change parameters
    if (j.contains("min_denomination")) config.min_denomination = j["min_denomination"].get<int>();
    if (j.contains("max_denomination")) config.max_denomination = j["max_denomination"].get<int>();
    if (j.contains("min_coin_count")) config.min_coin_count = j["min_coin_count"].get<int>();
    if (j.contains("max_coin_count")) config.max_coin_count = j["max_coin_count"].get<int>();
    if (j.contains("min_target")) config.min_target = j["min_target"].get<int>();
    if (j.contains("max_target")) config.max_target = j["max_target"].get<int>();
    if (j.contains("require_unit_coin")) config.require_unit_coin = j["require_unit_coin"].get<bool>();

    // Activity selection parameters
    if (j.contains("min_activities")) config.min_activities = j["min_activities"].get<int>();
    if (j.contains("max_activities")) config.max_activities = j["max_activities"].get<int>();
    if (j.contains("min_time")) config.min_time = j["min_time"].get<int>();
    if (j.contains("max_time")) config.max_time = j["max_time"].get<int>();
    if (j.contains("min_duration")) config.min_duration = j["min_duration"].get<int>();

    // Knapsack parameters
    if (j.contains("min_items")) config.min_items = j["min_items"].get<int>();
    if (j.contains("max_items")) config.max_items = j["max_items"].get<int>();
    if (j.contains("min_weight")) config.min_weight = j["min_weight"].get<int>();
    if (j.contains("max_weight")) config.max_weight = j["max_weight"].get<int>();
    if (j.contains("min_value")) config.min_value = j["min_value"].get<int>();
    if (j.contains("max_value")) config.max_value = j["max_value"].get<int>();
    if (j.contains("min_capacity")) config.min_capacity = j["min_capacity"].get<int>();
    if (j.contains("max_capacity")) config.max_capacity = j["max_capacity"].get<int>();

    // Safety and randomized parameters
    if (j.contains("random_seed")) config.random_seed = j["random_seed"].get<uint32_t>();
    if (j.contains("max_iterations")) config.max_iterations = j["max_iterations"].get<size_t>();
    if (j.contains("max_candidates")) config.max_candidates = j["max_candidates"].get<size_t>();

    return config;
}

nlohmann::json to_json(const TraceStep& step) {
    nlohmann::json j;
    j["step_number"] = step.step_number;
    j["state_description"] = step.state_description;
    j["remaining_target"] = step.remaining_target;
    j["available_choices"] = step.available_choices;
    if (step.selected_choice.has_value()) {
        j["selected_choice"] = step.selected_choice.value();
    } else {
        j["selected_choice"] = nullptr;
    }
    j["outcome_description"] = step.outcome_description;
    j["remaining_after"] = step.remaining_after;
    j["accumulated_count"] = step.accumulated_count;
    j["rationale"] = step.rationale;
    return j;
}

nlohmann::json to_json(const AlgorithmTrace& trace) {
    nlohmann::json j;
    j["algorithm_name"] = trace.algorithm_name;
    j["strategy_name"] = trace.strategy_name;
    j["success"] = trace.success;
    j["steps"] = nlohmann::json::array();
    for (const auto& step : trace.steps) {
        j["steps"].push_back(to_json(step));
    }
    j["final_solution"] = trace.final_solution;
    if (trace.final_cost.has_value()) {
        j["final_cost"] = trace.final_cost.value();
    } else {
        j["final_cost"] = nullptr;
    }
    j["execution_notes"] = trace.execution_notes;
    return j;
}

nlohmann::json to_json(const ComparisonResult& cmp) {
    nlohmann::json j;
    j["is_counterexample"] = cmp.is_counterexample;
    j["objective"] = (cmp.objective == ProblemObjective::Minimize ? "MINIMIZATION" : "MAXIMIZATION");
    j["greedy_solved"] = cmp.greedy_solved;
    j["optimal_solved"] = cmp.optimal_solved;

    if (cmp.greedy_cost.has_value()) j["greedy_cost"] = cmp.greedy_cost.value();
    else j["greedy_cost"] = nullptr;

    if (cmp.optimal_cost.has_value()) j["optimal_cost"] = cmp.optimal_cost.value();
    else j["optimal_cost"] = nullptr;

    if (cmp.absolute_gap.has_value()) j["absolute_gap"] = cmp.absolute_gap.value();
    else j["absolute_gap"] = nullptr;

    if (cmp.ratio.has_value()) j["ratio"] = cmp.ratio.value();
    else j["ratio"] = nullptr;

    if (cmp.overhead_percentage.has_value()) j["overhead_percentage"] = cmp.overhead_percentage.value();
    else j["overhead_percentage"] = nullptr;

    j["interpretation"] = cmp.interpretation;
    return j;
}

nlohmann::json to_json(const DivergenceResult& div) {
    nlohmann::json j;
    j["has_diverged"] = div.has_diverged;
    if (div.divergence_step.has_value()) {
        j["divergence_step"] = div.divergence_step.value();
    } else {
        j["divergence_step"] = nullptr;
    }
    j["state_remaining"] = div.state_remaining;
    if (div.greedy_choice.has_value()) {
        j["greedy_choice"] = div.greedy_choice.value();
    } else {
        j["greedy_choice"] = nullptr;
    }
    j["optimal_choices"] = div.optimal_choices;
    if (div.greedy_projected_cost.has_value()) {
        j["greedy_projected_cost"] = div.greedy_projected_cost.value();
    } else {
        j["greedy_projected_cost"] = nullptr;
    }
    if (div.optimal_projected_cost.has_value()) {
        j["optimal_projected_cost"] = div.optimal_projected_cost.value();
    } else {
        j["optimal_projected_cost"] = nullptr;
    }
    j["explanation"] = div.explanation;
    return j;
}

nlohmann::json to_json(const CoinChangeCandidate& cand) {
    nlohmann::json j;
    j["denominations"] = cand.denominations;
    j["target"] = cand.target;
    return j;
}

nlohmann::json to_json(const ActivitySelectionCandidate& cand) {
    nlohmann::json j;
    j["activities"] = nlohmann::json::array();
    for (const auto& act : cand.activities) {
        nlohmann::json act_j;
        act_j["id"] = act.id;
        act_j["start"] = act.start;
        act_j["finish"] = act.finish;
        act_j["duration"] = act.finish - act.start;
        j["activities"].push_back(act_j);
    }
    return j;
}

nlohmann::json to_json(const KnapsackCandidate& cand) {
    nlohmann::json j;
    j["capacity"] = cand.capacity;
    j["items"] = nlohmann::json::array();
    for (const auto& item : cand.items) {
        nlohmann::json item_j;
        item_j["id"] = item.id;
        item_j["value"] = item.value;
        item_j["weight"] = item.weight;
        item_j["ratio"] = item.ratio();
        j["items"].push_back(item_j);
    }
    return j;
}

nlohmann::json to_json(const SearchResult& result, const SearchConfig& config) {
    nlohmann::json j;
    j["status"] = "success";
    j["problem"] = to_slug(config.problem_type);
    j["search_config"] = to_json(config);

    // Search statistics
    nlohmann::json stats;
    stats["candidates_generated"] = result.candidates_generated;
    stats["candidates_evaluated"] = result.candidates_evaluated;
    stats["counterexamples_found"] = result.counterexamples_found;
    stats["nodes_explored"] = result.nodes_explored;
    stats["branches_pruned"] = result.branches_pruned;
    stats["iterations"] = result.iterations;
    stats["duration_ms"] = result.duration_ms;
    stats["search_completed"] = result.search_completed;
    stats["search_limit_reached"] = result.search_limit_reached;
    stats["termination_reason"] = result.termination_reason;
    j["search_statistics"] = stats;

    j["counterexample_found"] = result.found;

    nlohmann::json minimality;
    minimality["verified"] = result.smallest_verified;
    if (result.found) {
        if (result.smallest_verified) {
            minimality["description"] = "Smallest verified counterexample within the configured search bounds and canonical ordering.";
        } else {
            minimality["description"] = "Counterexample discovered, but minimality was not verified for this strategy/mode.";
        }
    } else {
        minimality["description"] = "No counterexample discovered.";
    }
    j["minimality"] = minimality;

    if (!result.found) {
        j["message"] = "No counterexample was found within the configured search space.";
        j["candidate"] = nullptr;
        j["greedy_solution"] = nullptr;
        j["optimal_solution"] = nullptr;
        j["metrics"] = nullptr;
        j["divergence"] = nullptr;
        j["error"] = nullptr;
        return j;
    }

    j["message"] = "Counterexample discovered!";
    j["candidate_summary"] = result.candidate_summary;

    // Polymorphic candidate and solution payload
    if (config.problem_type == ProblemType::COIN_CHANGE && result.best_candidate.has_value()) {
        j["candidate"] = to_json(result.best_candidate.value());
        
        nlohmann::json greedy_j;
        const auto& g = result.greedy_solution.value();
        greedy_j["success"] = g.success;
        greedy_j["cost"] = g.cost.value_or(0.0);
        greedy_j["items"] = g.items;
        greedy_j["trace"] = to_json(g.trace);
        j["greedy_solution"] = greedy_j;

        nlohmann::json optimal_j;
        const auto& opt = result.optimal_solution.value();
        optimal_j["success"] = opt.success;
        optimal_j["cost"] = opt.cost.value_or(0.0);
        optimal_j["items"] = opt.items;
        optimal_j["trace"] = to_json(opt.trace);
        j["optimal_solution"] = optimal_j;

    } else if (config.problem_type == ProblemType::ACTIVITY_SELECTION && result.best_activity_candidate.has_value()) {
        j["candidate"] = to_json(result.best_activity_candidate.value());

        nlohmann::json greedy_j;
        const auto& g = result.greedy_activity_solution.value();
        greedy_j["success"] = g.success;
        greedy_j["cost"] = g.cost.value_or(0.0);
        greedy_j["items"] = nlohmann::json::array();
        for (const auto& act : g.items) {
            nlohmann::json item_j;
            item_j["id"] = act.id;
            item_j["start"] = act.start;
            item_j["finish"] = act.finish;
            greedy_j["items"].push_back(item_j);
        }
        greedy_j["trace"] = to_json(g.trace);
        j["greedy_solution"] = greedy_j;

        nlohmann::json optimal_j;
        const auto& opt = result.optimal_activity_solution.value();
        optimal_j["success"] = opt.success;
        optimal_j["cost"] = opt.cost.value_or(0.0);
        optimal_j["items"] = nlohmann::json::array();
        for (const auto& act : opt.items) {
            nlohmann::json item_j;
            item_j["id"] = act.id;
            item_j["start"] = act.start;
            item_j["finish"] = act.finish;
            optimal_j["items"].push_back(item_j);
        }
        optimal_j["trace"] = to_json(opt.trace);
        j["optimal_solution"] = optimal_j;

    } else if (config.problem_type == ProblemType::KNAPSACK_01 && result.best_knapsack_candidate.has_value()) {
        j["candidate"] = to_json(result.best_knapsack_candidate.value());

        nlohmann::json greedy_j;
        const auto& g = result.greedy_knapsack_solution.value();
        greedy_j["success"] = g.success;
        greedy_j["cost"] = g.cost.value_or(0.0);
        greedy_j["items"] = nlohmann::json::array();
        int g_weight = 0;
        for (const auto& item : g.items) {
            nlohmann::json item_j;
            item_j["id"] = item.id;
            item_j["value"] = item.value;
            item_j["weight"] = item.weight;
            item_j["ratio"] = item.ratio();
            greedy_j["items"].push_back(item_j);
            g_weight += item.weight;
        }
        greedy_j["total_weight"] = g_weight;
        greedy_j["trace"] = to_json(g.trace);
        j["greedy_solution"] = greedy_j;

        nlohmann::json optimal_j;
        const auto& opt = result.optimal_knapsack_solution.value();
        optimal_j["success"] = opt.success;
        optimal_j["cost"] = opt.cost.value_or(0.0);
        optimal_j["items"] = nlohmann::json::array();
        int opt_weight = 0;
        for (const auto& item : opt.items) {
            nlohmann::json item_j;
            item_j["id"] = item.id;
            item_j["value"] = item.value;
            item_j["weight"] = item.weight;
            item_j["ratio"] = item.ratio();
            optimal_j["items"].push_back(item_j);
            opt_weight += item.weight;
        }
        optimal_j["total_weight"] = opt_weight;
        optimal_j["trace"] = to_json(opt.trace);
        j["optimal_solution"] = optimal_j;

    } else if (config.problem_type == ProblemType::KNAPSACK_FRACTIONAL && result.best_knapsack_candidate.has_value()) {
        j["candidate"] = to_json(result.best_knapsack_candidate.value());

        nlohmann::json greedy_j;
        const auto& g = result.greedy_fractional_solution.value();
        greedy_j["success"] = g.success;
        greedy_j["cost"] = g.cost.value_or(0.0);
        greedy_j["items"] = nlohmann::json::array();
        for (const auto& item : g.items) {
            nlohmann::json item_j;
            item_j["id"] = item.id;
            item_j["value"] = item.value;
            item_j["weight"] = item.weight;
            item_j["fraction"] = item.fraction;
            item_j["value_taken"] = item.value_taken;
            item_j["weight_taken"] = item.weight_taken;
            greedy_j["items"].push_back(item_j);
        }
        greedy_j["trace"] = to_json(g.trace);
        j["greedy_solution"] = greedy_j;

        nlohmann::json optimal_j;
        const auto& opt = result.optimal_fractional_solution.value();
        optimal_j["success"] = opt.success;
        optimal_j["cost"] = opt.cost.value_or(0.0);
        optimal_j["items"] = nlohmann::json::array();
        for (const auto& item : opt.items) {
            nlohmann::json item_j;
            item_j["id"] = item.id;
            item_j["value"] = item.value;
            item_j["weight"] = item.weight;
            item_j["fraction"] = item.fraction;
            item_j["value_taken"] = item.value_taken;
            item_j["weight_taken"] = item.weight_taken;
            optimal_j["items"].push_back(item_j);
        }
        optimal_j["trace"] = to_json(opt.trace);
        j["optimal_solution"] = optimal_j;
    }

    if (result.comparison_metrics.has_value()) {
        j["metrics"] = to_json(result.comparison_metrics.value());
    } else {
        j["metrics"] = nullptr;
    }

    if (result.first_divergence.has_value()) {
        j["divergence"] = to_json(result.first_divergence.value());
    } else {
        j["divergence"] = nullptr;
    }

    j["error"] = nullptr;
    return j;
}

nlohmann::json make_error_json(const std::string& code, const std::string& message) {
    nlohmann::json j;
    j["status"] = "error";
    j["error"] = {
        {"code", code},
        {"message", message}
    };
    return j;
}

nlohmann::json problem_registry_to_json() {
    nlohmann::json j = nlohmann::json::array();

    // 1. Coin Change
    {
        CoinChangeProblem prob;
        auto comp = prob.complexity_info();
        nlohmann::json p;
        p["id"] = "coin_change";
        p["name"] = "Coin Change";
        p["objective"] = "MINIMIZATION";
        p["description"] = "Make exact change for a target amount using the minimum total number of coins.";
        p["greedy_strategy"] = "Largest denomination first (greedy cashier heuristic)";
        p["oracle_strategy"] = "Exact 1D dynamic programming with path reconstruction";
        p["can_fail"] = true;
        p["default_config"] = {
            {"min_denomination", 1},
            {"max_denomination", 10},
            {"min_coin_count", 2},
            {"max_coin_count", 4},
            {"min_target", 1},
            {"max_target", 30},
            {"require_unit_coin", true}
        };
        p["solvers"] = nlohmann::json::array();
        for (const auto& s : comp.solvers) {
            nlohmann::json s_j;
            s_j["algorithm_name"] = s.algorithm_name;
            s_j["time_best"] = s.time_best;
            s_j["time_average"] = s.time_average;
            s_j["time_worst"] = s.time_worst;
            s_j["space_worst"] = s.space_worst;
            s_j["explanation"] = s.explanation;
            p["solvers"].push_back(s_j);
        }
        j.push_back(p);
    }

    // 2. Activity Selection
    {
        ActivitySelectionProblem prob;
        auto comp = prob.complexity_info();
        nlohmann::json p;
        p["id"] = "activity_selection";
        p["name"] = "Activity Selection";
        p["objective"] = "MAXIMIZATION";
        p["description"] = "Select the maximum number of mutually compatible, non-overlapping activities sharing a common resource.";
        p["greedy_strategy"] = "Earliest finish time first";
        p["oracle_strategy"] = "Exact interval scheduling dynamic programming";
        p["can_fail"] = false;
        p["default_config"] = {
            {"min_activities", 3},
            {"max_activities", 5},
            {"min_time", 0},
            {"max_time", 10},
            {"min_duration", 1}
        };
        p["solvers"] = nlohmann::json::array();
        for (const auto& s : comp.solvers) {
            nlohmann::json s_j;
            s_j["algorithm_name"] = s.algorithm_name;
            s_j["time_best"] = s.time_best;
            s_j["time_average"] = s.time_average;
            s_j["time_worst"] = s.time_worst;
            s_j["space_worst"] = s.space_worst;
            s_j["explanation"] = s.explanation;
            p["solvers"].push_back(s_j);
        }
        j.push_back(p);
    }

    // 3. 0/1 Knapsack
    {
        Knapsack01Problem prob;
        auto comp = prob.complexity_info();
        nlohmann::json p;
        p["id"] = "knapsack_01";
        p["name"] = "0/1 Knapsack";
        p["objective"] = "MAXIMIZATION";
        p["description"] = "Select a subset of discrete items to maximize total value without exceeding capacity W.";
        p["greedy_strategy"] = "Highest value-to-weight ratio first";
        p["oracle_strategy"] = "Exact 2D dynamic programming table with binary reconstruction";
        p["can_fail"] = true;
        p["default_config"] = {
            {"min_items", 3},
            {"max_items", 5},
            {"min_weight", 1},
            {"max_weight", 10},
            {"min_value", 1},
            {"max_value", 20},
            {"min_capacity", 5},
            {"max_capacity", 20}
        };
        p["solvers"] = nlohmann::json::array();
        for (const auto& s : comp.solvers) {
            nlohmann::json s_j;
            s_j["algorithm_name"] = s.algorithm_name;
            s_j["time_best"] = s.time_best;
            s_j["time_average"] = s.time_average;
            s_j["time_worst"] = s.time_worst;
            s_j["space_worst"] = s.space_worst;
            s_j["explanation"] = s.explanation;
            p["solvers"].push_back(s_j);
        }
        j.push_back(p);
    }

    // 4. Fractional Knapsack
    {
        FractionalKnapsackProblem prob;
        auto comp = prob.complexity_info();
        nlohmann::json p;
        p["id"] = "knapsack_fractional";
        p["name"] = "Fractional Knapsack";
        p["objective"] = "MAXIMIZATION";
        p["description"] = "Select fractions of items to maximize total value within capacity W (continuous relaxation).";
        p["greedy_strategy"] = "Highest value-to-weight ratio with continuous fraction on critical item";
        p["oracle_strategy"] = "Exact continuous optimal solution (proven equivalent to greedy)";
        p["can_fail"] = false;
        p["default_config"] = {
            {"min_items", 3},
            {"max_items", 5},
            {"min_weight", 1},
            {"max_weight", 10},
            {"min_value", 1},
            {"max_value", 20},
            {"min_capacity", 5},
            {"max_capacity", 20}
        };
        p["solvers"] = nlohmann::json::array();
        for (const auto& s : comp.solvers) {
            nlohmann::json s_j;
            s_j["algorithm_name"] = s.algorithm_name;
            s_j["time_best"] = s.time_best;
            s_j["time_average"] = s.time_average;
            s_j["time_worst"] = s.time_worst;
            s_j["space_worst"] = s.space_worst;
            s_j["explanation"] = s.explanation;
            p["solvers"].push_back(s_j);
        }
        j.push_back(p);
    }

    return j;
}

nlohmann::json verify_candidate_json(const nlohmann::json& input_json) {
    if (!input_json.contains("problem") || !input_json.contains("candidate")) {
        return make_error_json("INVALID_REQUEST", "Verification request must contain 'problem' and 'candidate' objects.");
    }

    ProblemType pt = parse_problem_type(input_json["problem"].get<std::string>());
    const auto& c_json = input_json["candidate"];

    nlohmann::json response;
    response["status"] = "success";
    response["problem"] = to_slug(pt);

    if (pt == ProblemType::COIN_CHANGE) {
        CoinChangeProblem problem;
        CoinChangeCandidate candidate;
        if (!c_json.contains("denominations") || !c_json.contains("target")) {
            return make_error_json("INVALID_CANDIDATE", "Coin Change candidate requires 'denominations' array and 'target' int.");
        }
        candidate.denominations = c_json["denominations"].get<std::vector<int>>();
        candidate.target = c_json["target"].get<int>();

        problem.validate_candidate(candidate);
        auto greedy = problem.greedy_solution(candidate);
        auto optimal = problem.optimal_solution(candidate);
        auto metrics = problem.compare_solutions(greedy, optimal);
        auto divergence = problem.analyze_divergence(candidate, greedy, optimal);

        response["counterexample_found"] = metrics.is_counterexample;
        response["candidate"] = to_json(candidate);

        nlohmann::json g_j;
        g_j["success"] = greedy.success;
        g_j["cost"] = greedy.cost.value_or(0.0);
        g_j["items"] = greedy.items;
        g_j["trace"] = to_json(greedy.trace);
        response["greedy_solution"] = g_j;

        nlohmann::json o_j;
        o_j["success"] = optimal.success;
        o_j["cost"] = optimal.cost.value_or(0.0);
        o_j["items"] = optimal.items;
        o_j["trace"] = to_json(optimal.trace);
        response["optimal_solution"] = o_j;

        response["metrics"] = to_json(metrics);
        response["divergence"] = to_json(divergence);

    } else if (pt == ProblemType::ACTIVITY_SELECTION) {
        ActivitySelectionProblem problem;
        ActivitySelectionCandidate candidate;
        if (!c_json.contains("activities")) {
            return make_error_json("INVALID_CANDIDATE", "Activity Selection candidate requires 'activities' array.");
        }
        for (const auto& a : c_json["activities"]) {
            candidate.activities.push_back({
                .id = a["id"].get<int>(),
                .start = a["start"].get<int>(),
                .finish = a["finish"].get<int>()
            });
        }

        problem.validate_candidate(candidate);
        auto greedy = problem.greedy_solution(candidate);
        auto optimal = problem.optimal_solution(candidate);
        auto metrics = problem.compare_solutions(greedy, optimal);
        auto divergence = problem.analyze_divergence(candidate, greedy, optimal);

        response["counterexample_found"] = metrics.is_counterexample;
        response["candidate"] = to_json(candidate);

        nlohmann::json g_j;
        g_j["success"] = greedy.success;
        g_j["cost"] = greedy.cost.value_or(0.0);
        g_j["items"] = nlohmann::json::array();
        for (const auto& act : greedy.items) {
            nlohmann::json item_j;
            item_j["id"] = act.id;
            item_j["start"] = act.start;
            item_j["finish"] = act.finish;
            g_j["items"].push_back(item_j);
        }
        g_j["trace"] = to_json(greedy.trace);
        response["greedy_solution"] = g_j;

        nlohmann::json o_j;
        o_j["success"] = optimal.success;
        o_j["cost"] = optimal.cost.value_or(0.0);
        o_j["items"] = nlohmann::json::array();
        for (const auto& act : optimal.items) {
            nlohmann::json item_j;
            item_j["id"] = act.id;
            item_j["start"] = act.start;
            item_j["finish"] = act.finish;
            o_j["items"].push_back(item_j);
        }
        o_j["trace"] = to_json(optimal.trace);
        response["optimal_solution"] = o_j;

        response["metrics"] = to_json(metrics);
        response["divergence"] = to_json(divergence);

    } else if (pt == ProblemType::KNAPSACK_01) {
        Knapsack01Problem problem;
        KnapsackCandidate candidate;
        if (!c_json.contains("capacity") || !c_json.contains("items")) {
            return make_error_json("INVALID_CANDIDATE", "Knapsack candidate requires 'capacity' and 'items' array.");
        }
        candidate.capacity = c_json["capacity"].get<int>();
        for (const auto& it : c_json["items"]) {
            candidate.items.push_back({
                .id = it["id"].get<int>(),
                .value = it["value"].get<int>(),
                .weight = it["weight"].get<int>()
            });
        }

        problem.validate_candidate(candidate);
        auto greedy = problem.greedy_solution(candidate);
        auto optimal = problem.optimal_solution(candidate);
        auto metrics = problem.compare_solutions(greedy, optimal);
        auto divergence = problem.analyze_divergence(candidate, greedy, optimal);

        response["counterexample_found"] = metrics.is_counterexample;
        response["candidate"] = to_json(candidate);

        nlohmann::json g_j;
        g_j["success"] = greedy.success;
        g_j["cost"] = greedy.cost.value_or(0.0);
        g_j["items"] = nlohmann::json::array();
        int g_wt = 0;
        for (const auto& item : greedy.items) {
            nlohmann::json item_j;
            item_j["id"] = item.id;
            item_j["value"] = item.value;
            item_j["weight"] = item.weight;
            item_j["ratio"] = item.ratio();
            g_j["items"].push_back(item_j);
            g_wt += item.weight;
        }
        g_j["total_weight"] = g_wt;
        g_j["trace"] = to_json(greedy.trace);
        response["greedy_solution"] = g_j;

        nlohmann::json o_j;
        o_j["success"] = optimal.success;
        o_j["cost"] = optimal.cost.value_or(0.0);
        o_j["items"] = nlohmann::json::array();
        int o_wt = 0;
        for (const auto& item : optimal.items) {
            nlohmann::json item_j;
            item_j["id"] = item.id;
            item_j["value"] = item.value;
            item_j["weight"] = item.weight;
            item_j["ratio"] = item.ratio();
            o_j["items"].push_back(item_j);
            o_wt += item.weight;
        }
        o_j["total_weight"] = o_wt;
        o_j["trace"] = to_json(optimal.trace);
        response["optimal_solution"] = o_j;

        response["metrics"] = to_json(metrics);
        response["divergence"] = to_json(divergence);

    } else if (pt == ProblemType::KNAPSACK_FRACTIONAL) {
        FractionalKnapsackProblem problem;
        KnapsackCandidate candidate;
        if (!c_json.contains("capacity") || !c_json.contains("items")) {
            return make_error_json("INVALID_CANDIDATE", "Knapsack candidate requires 'capacity' and 'items' array.");
        }
        candidate.capacity = c_json["capacity"].get<int>();
        for (const auto& it : c_json["items"]) {
            candidate.items.push_back({
                .id = it["id"].get<int>(),
                .value = it["value"].get<int>(),
                .weight = it["weight"].get<int>()
            });
        }

        problem.validate_candidate(candidate);
        auto greedy = problem.greedy_solution(candidate);
        auto optimal = problem.optimal_solution(candidate);
        auto metrics = problem.compare_solutions(greedy, optimal);
        auto divergence = problem.analyze_divergence(candidate, greedy, optimal);

        response["counterexample_found"] = metrics.is_counterexample;
        response["candidate"] = to_json(candidate);

        nlohmann::json g_j;
        g_j["success"] = greedy.success;
        g_j["cost"] = greedy.cost.value_or(0.0);
        g_j["items"] = nlohmann::json::array();
        for (const auto& item : greedy.items) {
            nlohmann::json item_j;
            item_j["id"] = item.id;
            item_j["value"] = item.value;
            item_j["weight"] = item.weight;
            item_j["fraction"] = item.fraction;
            item_j["value_taken"] = item.value_taken;
            item_j["weight_taken"] = item.weight_taken;
            g_j["items"].push_back(item_j);
        }
        g_j["trace"] = to_json(greedy.trace);
        response["greedy_solution"] = g_j;

        nlohmann::json o_j;
        o_j["success"] = optimal.success;
        o_j["cost"] = optimal.cost.value_or(0.0);
        o_j["items"] = nlohmann::json::array();
        for (const auto& item : optimal.items) {
            nlohmann::json item_j;
            item_j["id"] = item.id;
            item_j["value"] = item.value;
            item_j["weight"] = item.weight;
            item_j["fraction"] = item.fraction;
            item_j["value_taken"] = item.value_taken;
            item_j["weight_taken"] = item.weight_taken;
            o_j["items"].push_back(item_j);
        }
        o_j["trace"] = to_json(optimal.trace);
        response["optimal_solution"] = o_j;

        response["metrics"] = to_json(metrics);
        response["divergence"] = to_json(divergence);
    }

    return response;
}

} // namespace greedybreaker
