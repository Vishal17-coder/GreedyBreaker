#include "greedybreaker/minimization/minimizer.hpp"
#include "greedybreaker/serialization/json_adapters.hpp"
#include <algorithm>
#include <cmath>

namespace greedybreaker::minimization {

void to_json(nlohmann::json& j, const MinimizationResult& r) {
    j = nlohmann::json{
        {"original_instance", r.original_instance},
        {"minimal_instance", r.minimal_instance},
        {"reduction_steps", r.reduction_steps},
        {"original_size_metric", r.original_size_metric},
        {"minimal_size_metric", r.minimal_size_metric},
        {"reduction_percent", r.reduction_percent},
        {"ordering_criteria", r.ordering_criteria},
        {"minimality_status", r.minimality_status},
        {"final_verification", r.final_verification}
    };
}

void from_json(const nlohmann::json& j, MinimizationResult& r) {
    if (j.contains("original_instance")) r.original_instance = j.at("original_instance");
    if (j.contains("minimal_instance")) r.minimal_instance = j.at("minimal_instance");
    r.reduction_steps = j.value("reduction_steps", size_t{0});
    r.original_size_metric = j.value("original_size_metric", 0.0);
    r.minimal_size_metric = j.value("minimal_size_metric", 0.0);
    r.reduction_percent = j.value("reduction_percent", 0.0);
    r.ordering_criteria = j.value("ordering_criteria", "");
    r.minimality_status = j.value("minimality_status", "");
    if (j.contains("final_verification")) {
        r.final_verification = j.at("final_verification").get<core::VerificationResult>();
    }
}

MinimizationResult CounterexampleMinimizer::minimize_coin_change(
    const problems::CoinChangeInstance& original,
    const std::string& strategy)
{
    MinimizationResult res;
    res.original_instance = original;
    res.original_size_metric = static_cast<double>(original.denominations.size() * 1000 + original.target);

    auto curr = original;
    auto greedy = problems::CoinChangeProblem::solve_greedy(curr, strategy);
    auto oracle = problems::CoinChangeProblem::solve_exact_dp(curr);
    auto ver = core::verify(problems::CoinChangeProblem::objective(), greedy, oracle);

    if (!ver.is_counterexample) {
        res.minimal_instance = original;
        res.final_verification = ver;
        return res;
    }

    bool improved = true;
    while (improved) {
        improved = false;

        // Pass 1: Try dropping denominations (except base coin 1)
        if (curr.denominations.size() > 2) {
            for (size_t i = 1; i < curr.denominations.size(); ++i) {
                auto cand = curr;
                cand.denominations.erase(cand.denominations.begin() + static_cast<ptrdiff_t>(i));
                if (problems::CoinChangeProblem::is_valid_instance(cand)) {
                    auto g = problems::CoinChangeProblem::solve_greedy(cand, strategy);
                    auto o = problems::CoinChangeProblem::solve_exact_dp(cand);
                    auto v = core::verify(problems::CoinChangeProblem::objective(), g, o);
                    if (v.is_counterexample) {
                        curr = cand;
                        ++res.reduction_steps;
                        improved = true;
                        break;
                    }
                }
            }
        }
        if (improved) continue;

        // Pass 2: Try reducing target
        for (int64_t t = curr.target - 1; t >= 1; --t) {
            auto cand = curr;
            cand.target = t;
            auto g = problems::CoinChangeProblem::solve_greedy(cand, strategy);
            auto o = problems::CoinChangeProblem::solve_exact_dp(cand);
            auto v = core::verify(problems::CoinChangeProblem::objective(), g, o);
            if (v.is_counterexample) {
                curr = cand;
                ++res.reduction_steps;
                improved = true;
                break;
            }
        }
        if (improved) continue;

        // Pass 3: Try reducing denomination values
        for (size_t i = 1; i < curr.denominations.size(); ++i) {
            int64_t min_allowed = curr.denominations[i - 1] + 1;
            if (curr.denominations[i] > min_allowed) {
                auto cand = curr;
                cand.denominations[i] -= 1;
                auto g = problems::CoinChangeProblem::solve_greedy(cand, strategy);
                auto o = problems::CoinChangeProblem::solve_exact_dp(cand);
                auto v = core::verify(problems::CoinChangeProblem::objective(), g, o);
                if (v.is_counterexample) {
                    curr = cand;
                    ++res.reduction_steps;
                    improved = true;
                    break;
                }
            }
        }
    }

    res.minimal_instance = curr;
    res.minimal_size_metric = static_cast<double>(curr.denominations.size() * 1000 + curr.target);
    if (res.original_size_metric > 0.0) {
        res.reduction_percent = ((res.original_size_metric - res.minimal_size_metric) / res.original_size_metric) * 100.0;
    }
    auto final_g = problems::CoinChangeProblem::solve_greedy(curr, strategy);
    auto final_o = problems::CoinChangeProblem::solve_exact_dp(curr);
    res.final_verification = core::verify(problems::CoinChangeProblem::objective(), final_g, final_o);

    return res;
}

MinimizationResult CounterexampleMinimizer::minimize_knapsack_01(
    const problems::Knapsack01Instance& original,
    const std::string& strategy)
{
    MinimizationResult res;
    res.original_instance = original;
    res.original_size_metric = static_cast<double>(original.items.size() * 1000 + static_cast<size_t>(original.capacity));

    auto curr = original;
    auto greedy = problems::Knapsack01Problem::solve_greedy(curr, strategy);
    auto oracle = problems::Knapsack01Problem::solve_exact_dp(curr);
    auto ver = core::verify(problems::Knapsack01Problem::objective(), greedy, oracle);

    if (!ver.is_counterexample) {
        res.minimal_instance = original;
        res.final_verification = ver;
        return res;
    }

    bool improved = true;
    while (improved) {
        improved = false;

        // Pass 1: Try dropping items
        if (curr.items.size() > 2) {
            for (size_t i = 0; i < curr.items.size(); ++i) {
                auto cand = curr;
                cand.items.erase(cand.items.begin() + static_cast<ptrdiff_t>(i));
                if (problems::Knapsack01Problem::is_valid_instance(cand)) {
                    auto g = problems::Knapsack01Problem::solve_greedy(cand, strategy);
                    auto o = problems::Knapsack01Problem::solve_exact_dp(cand);
                    auto v = core::verify(problems::Knapsack01Problem::objective(), g, o);
                    if (v.is_counterexample) {
                        curr = cand;
                        ++res.reduction_steps;
                        improved = true;
                        break;
                    }
                }
            }
        }
        if (improved) continue;

        // Pass 2: Try reducing capacity
        int64_t cap_int = static_cast<int64_t>(std::round(curr.capacity));
        for (int64_t c = cap_int - 1; c >= 2; --c) {
            auto cand = curr;
            cand.capacity = static_cast<double>(c);
            auto g = problems::Knapsack01Problem::solve_greedy(cand, strategy);
            auto o = problems::Knapsack01Problem::solve_exact_dp(cand);
            auto v = core::verify(problems::Knapsack01Problem::objective(), g, o);
            if (v.is_counterexample) {
                curr = cand;
                ++res.reduction_steps;
                improved = true;
                break;
            }
        }
    }

    res.minimal_instance = curr;
    res.minimal_size_metric = static_cast<double>(curr.items.size() * 1000 + static_cast<size_t>(curr.capacity));
    if (res.original_size_metric > 0.0) {
        res.reduction_percent = ((res.original_size_metric - res.minimal_size_metric) / res.original_size_metric) * 100.0;
    }
    auto final_g = problems::Knapsack01Problem::solve_greedy(curr, strategy);
    auto final_o = problems::Knapsack01Problem::solve_exact_dp(curr);
    res.final_verification = core::verify(problems::Knapsack01Problem::objective(), final_g, final_o);

    return res;
}

MinimizationResult CounterexampleMinimizer::minimize_activity_selection(
    const problems::ActivitySelectionInstance& original,
    const std::string& strategy)
{
    MinimizationResult res;
    res.original_instance = original;
    res.original_size_metric = static_cast<double>(original.activities.size());

    auto curr = original;
    auto greedy = problems::ActivitySelectionProblem::solve_greedy(curr, strategy);
    auto oracle = problems::ActivitySelectionProblem::solve_exact_dp(curr);
    auto ver = core::verify(problems::ActivitySelectionProblem::objective(), greedy, oracle);

    if (!ver.is_counterexample) {
        res.minimal_instance = original;
        res.final_verification = ver;
        return res;
    }

    bool improved = true;
    while (improved) {
        improved = false;
        if (curr.activities.size() > 3) {
            for (size_t i = 0; i < curr.activities.size(); ++i) {
                auto cand = curr;
                cand.activities.erase(cand.activities.begin() + static_cast<ptrdiff_t>(i));
                if (problems::ActivitySelectionProblem::is_valid_instance(cand)) {
                    auto g = problems::ActivitySelectionProblem::solve_greedy(cand, strategy);
                    auto o = problems::ActivitySelectionProblem::solve_exact_dp(cand);
                    auto v = core::verify(problems::ActivitySelectionProblem::objective(), g, o);
                    if (v.is_counterexample) {
                        curr = cand;
                        ++res.reduction_steps;
                        improved = true;
                        break;
                    }
                }
            }
        }
    }

    res.minimal_instance = curr;
    res.minimal_size_metric = static_cast<double>(curr.activities.size());
    if (res.original_size_metric > 0.0) {
        res.reduction_percent = ((res.original_size_metric - res.minimal_size_metric) / res.original_size_metric) * 100.0;
    }
    auto final_g = problems::ActivitySelectionProblem::solve_greedy(curr, strategy);
    auto final_o = problems::ActivitySelectionProblem::solve_exact_dp(curr);
    res.final_verification = core::verify(problems::ActivitySelectionProblem::objective(), final_g, final_o);

    return res;
}

MinimizationResult CounterexampleMinimizer::minimize_huffman(
    const problems::HuffmanCodingInstance& original,
    const std::string& strategy)
{
    MinimizationResult res;
    res.original_instance = original;
    res.original_size_metric = static_cast<double>(original.symbols.size());

    auto curr = original;
    auto greedy = problems::HuffmanCodingProblem::solve_greedy(curr, strategy);
    auto oracle = problems::HuffmanCodingProblem::solve_exact(curr);
    auto ver = core::verify(problems::HuffmanCodingProblem::objective(), greedy, oracle);

    if (!ver.is_counterexample) {
        res.minimal_instance = original;
        res.final_verification = ver;
        return res;
    }

    bool improved = true;
    while (improved) {
        improved = false;
        if (curr.symbols.size() > 3) {
            for (size_t i = 0; i < curr.symbols.size(); ++i) {
                auto cand = curr;
                cand.symbols.erase(cand.symbols.begin() + static_cast<ptrdiff_t>(i));
                if (problems::HuffmanCodingProblem::is_valid_instance(cand)) {
                    auto g = problems::HuffmanCodingProblem::solve_greedy(cand, strategy);
                    auto o = problems::HuffmanCodingProblem::solve_exact(cand);
                    auto v = core::verify(problems::HuffmanCodingProblem::objective(), g, o);
                    if (v.is_counterexample) {
                        curr = cand;
                        ++res.reduction_steps;
                        improved = true;
                        break;
                    }
                }
            }
        }
    }

    res.minimal_instance = curr;
    res.minimal_size_metric = static_cast<double>(curr.symbols.size());
    if (res.original_size_metric > 0.0) {
        res.reduction_percent = ((res.original_size_metric - res.minimal_size_metric) / res.original_size_metric) * 100.0;
    }
    auto final_g = problems::HuffmanCodingProblem::solve_greedy(curr, strategy);
    auto final_o = problems::HuffmanCodingProblem::solve_exact(curr);
    res.final_verification = core::verify(problems::HuffmanCodingProblem::objective(), final_g, final_o);

    return res;
}

MinimizationResult CounterexampleMinimizer::minimize(
    const std::string& problem_type,
    const nlohmann::json& instance_json,
    const std::string& strategy)
{
    if (problem_type == "coin_change") {
        auto inst = instance_json.get<problems::CoinChangeInstance>();
        std::string strat = (strategy == "default") ? "largest_denomination_first" : strategy;
        return minimize_coin_change(inst, strat);
    } else if (problem_type == "knapsack_01") {
        auto inst = instance_json.get<problems::Knapsack01Instance>();
        std::string strat = (strategy == "default") ? "value_density_descending" : strategy;
        return minimize_knapsack_01(inst, strat);
    } else if (problem_type == "activity_selection") {
        auto inst = instance_json.get<problems::ActivitySelectionInstance>();
        std::string strat = (strategy == "default") ? "shortest_interval" : strategy;
        return minimize_activity_selection(inst, strat);
    } else if (problem_type == "huffman_coding") {
        auto inst = instance_json.get<problems::HuffmanCodingInstance>();
        std::string strat = (strategy == "default") ? "shannon_fano" : strategy;
        return minimize_huffman(inst, strat);
    }

    MinimizationResult res;
    res.original_instance = instance_json;
    res.minimal_instance = instance_json;
    return res;
}

} // namespace greedybreaker::minimization
