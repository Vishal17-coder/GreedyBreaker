#include "greedybreaker/search/search_engine.hpp"
#include "greedybreaker/problems/coin_change.hpp"
#include "greedybreaker/problems/knapsack_01.hpp"
#include "greedybreaker/problems/activity_selection.hpp"
#include "greedybreaker/problems/fractional_knapsack.hpp"
#include "greedybreaker/problems/huffman_coding.hpp"
#include "greedybreaker/search/search_space.hpp"
#include <algorithm>
#include <chrono>
#include <random>

namespace greedybreaker::search {

namespace {

// Helper combinations generator for coin change
void generate_coin_combos(
    int64_t current_val,
    int64_t max_val,
    size_t k_remaining,
    std::vector<int64_t>& current_combo,
    std::vector<std::vector<int64_t>>& all_combos,
    uint64_t max_limit)
{
    if (all_combos.size() >= max_limit) return;
    if (k_remaining == 0) {
        all_combos.push_back(current_combo);
        return;
    }
    for (int64_t v = current_val; v <= max_val; ++v) {
        if (all_combos.size() >= max_limit) return;
        current_combo.push_back(v);
        generate_coin_combos(v + 1, max_val, k_remaining - 1, current_combo, all_combos, max_limit);
        current_combo.pop_back();
    }
}

} // namespace

SearchResult SearchEngine::run(const SearchConfig& config) {
    const auto search_start = std::chrono::steady_clock::now();
    SearchResult res;
    res.experiment_id = config.experiment_id;
    res.problem_type = config.problem_type;
    res.strategy_id = config.strategy_id;
    res.oracle_id = config.oracle_id;
    res.mode = config.mode;
    res.strategy = config.strategy;

    // Reproducibility Token Construction
    res.reproducibility_token = nlohmann::json{
        {"experiment_id", config.experiment_id},
        {"problem_type", config.problem_type},
        {"strategy_id", config.strategy_id},
        {"oracle_id", config.oracle_id},
        {"mode", std::string(to_string(config.mode))},
        {"strategy", std::string(to_string(config.strategy))},
        {"random_seed", config.random_seed},
        {"search_space", config.search_space},
        {"engine_version", "2.0.0"}
    };

    std::mt19937_64 rng(config.random_seed);
    double total_gap = 0.0;
    bool time_limit_hit = false;
    bool candidate_limit_hit = false;

    // Dispatch by problem type
    if (config.problem_type == "coin_change") {
        CoinChangeSearchSpace space;
        if (!config.search_space.is_null() && !config.search_space.empty()) {
            space = config.search_space.get<CoinChangeSearchSpace>();
        }
        res.estimated_space_size = space.estimate_cardinality();

        std::vector<problems::CoinChangeInstance> candidates;
        if (config.strategy == SearchStrategy::EXHAUSTIVE) {
            // Systematically generate coin sets
            for (int64_t k = space.denomination_count.min; k <= space.denomination_count.max; ++k) {
                if (k < 1) continue;
                std::vector<std::vector<int64_t>> combos;
                std::vector<int64_t> cur = {1}; // standard baseline coin 1
                generate_coin_combos(2, space.denomination_range.max, static_cast<size_t>(k - 1), cur, combos, config.max_candidates);
                for (const auto& c_set : combos) {
                    for (int64_t t = space.target_range.min; t <= space.target_range.max; ++t) {
                        candidates.push_back({c_set, t});
                        if (candidates.size() >= config.max_candidates) break;
                    }
                    if (candidates.size() >= config.max_candidates) break;
                }
                if (candidates.size() >= config.max_candidates) break;
            }
        } else if (config.strategy == SearchStrategy::GUIDED) {
            // Focus on known non-canonical trap patterns e.g. {1, c-1, c} with target 2*(c-1)
            for (int64_t c = 3; c <= space.denomination_range.max; ++c) {
                std::vector<int64_t> coins = {1, c - 1, c};
                int64_t target = 2 * (c - 1);
                if (target <= space.target_range.max) {
                    candidates.push_back({coins, target});
                }
                std::vector<int64_t> coins2 = {1, c, c + 1};
                int64_t target2 = 2 * c;
                if (target2 <= space.target_range.max) {
                    candidates.push_back({coins2, target2});
                }
            }
        } else {
            // RANDOMIZED / HYBRID
            std::uniform_int_distribution<int64_t> dist_k(space.denomination_count.min, space.denomination_count.max);
            std::uniform_int_distribution<int64_t> dist_d(2, space.denomination_range.max);
            std::uniform_int_distribution<int64_t> dist_t(space.target_range.min, space.target_range.max);

            for (uint64_t i = 0; i < config.max_candidates; ++i) {
                int64_t k = dist_k(rng);
                std::vector<int64_t> coins = {1};
                for (int64_t j = 1; j < k; ++j) {
                    coins.push_back(dist_d(rng));
                }
                std::sort(coins.begin(), coins.end());
                coins.erase(std::unique(coins.begin(), coins.end()), coins.end());
                if (static_cast<int64_t>(coins.size()) >= space.denomination_count.min) {
                    candidates.push_back({coins, dist_t(rng)});
                }
            }
        }

        // Evaluate candidates
        for (const auto& inst : candidates) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - search_start).count() > static_cast<int64_t>(config.max_time_ms)) {
                time_limit_hit = true;
                break;
            }
            if (res.instances_evaluated >= config.max_candidates) {
                candidate_limit_hit = true;
                break;
            }

            ++res.instances_evaluated;
            auto greedy = problems::CoinChangeProblem::solve_greedy(inst, config.strategy_id);
            auto oracle = problems::CoinChangeProblem::solve_exact_dp(inst);
            auto ver = core::verify(problems::CoinChangeProblem::objective(), greedy, oracle);

            if (ver.is_counterexample) {
                ++res.counterexamples_found;
                total_gap += ver.absolute_gap;
                if (ver.absolute_gap > res.worst_gap) {
                    res.worst_gap = ver.absolute_gap;
                }

                bool update_best = false;
                if (!res.has_counterexample) {
                    update_best = true;
                } else if (config.mode == SearchMode::WORST) {
                    if (ver.absolute_gap > res.best_verification.absolute_gap) update_best = true;
                } else if (config.mode == SearchMode::SMALLEST) {
                    // Smallest target primary, fewest coins secondary
                    auto best_inst = res.best_instance.get<problems::CoinChangeInstance>();
                    if (inst.target < best_inst.target) update_best = true;
                    else if (inst.target == best_inst.target && inst.denominations.size() < best_inst.denominations.size()) update_best = true;
                }

                if (update_best) {
                    res.has_counterexample = true;
                    res.best_instance = inst;
                    res.best_greedy = greedy;
                    res.best_oracle = oracle;
                    res.best_verification = ver;
                }

                if (config.mode == SearchMode::ANY) {
                    break;
                }
            }
        }
    } else if (config.problem_type == "knapsack_01" || config.problem_type == "fractional_knapsack") {
        KnapsackSearchSpace space;
        if (!config.search_space.is_null() && !config.search_space.empty()) {
            space = config.search_space.get<KnapsackSearchSpace>();
        }
        res.estimated_space_size = space.estimate_cardinality();

        std::vector<problems::Knapsack01Instance> candidates;
        std::uniform_int_distribution<int64_t> dist_n(space.item_count.min, space.item_count.max);
        std::uniform_int_distribution<int64_t> dist_w(space.weight_range.min, space.weight_range.max);
        std::uniform_int_distribution<int64_t> dist_v(space.value_range.min, space.value_range.max);
        std::uniform_int_distribution<int64_t> dist_c(space.capacity_range.min, space.capacity_range.max);

        if (config.strategy == SearchStrategy::GUIDED) {
            // Generate mathematically proven density trap:
            // High density small item (6.0) causes stranded capacity,
            // while combination of lower density items (5.0 + 4.0) fills knapsack for higher total value.
            for (int64_t s = 1; s <= 10; ++s) {
                problems::Knapsack01Instance inst;
                inst.capacity = static_cast<double>(50 * s);
                inst.items = {
                    {1, static_cast<double>(60 * s), static_cast<double>(10 * s)},
                    {2, static_cast<double>(100 * s), static_cast<double>(20 * s)},
                    {3, static_cast<double>(120 * s), static_cast<double>(30 * s)}
                };
                candidates.push_back(inst);
            }
        } else {
            for (uint64_t i = 0; i < config.max_candidates; ++i) {
                problems::Knapsack01Instance inst;
                inst.capacity = static_cast<double>(dist_c(rng));
                int64_t n = dist_n(rng);
                for (int64_t j = 1; j <= n; ++j) {
                    inst.items.push_back({j, static_cast<double>(dist_v(rng)), static_cast<double>(dist_w(rng))});
                }
                candidates.push_back(inst);
            }
        }

        // Evaluate candidates
        for (const auto& inst : candidates) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - search_start).count() > static_cast<int64_t>(config.max_time_ms)) {
                time_limit_hit = true;
                break;
            }
            if (res.instances_evaluated >= config.max_candidates) {
                candidate_limit_hit = true;
                break;
            }

            ++res.instances_evaluated;
            core::GreedyResult greedy;
            core::OracleResult oracle;
            core::VerificationResult ver;

            if (config.problem_type == "knapsack_01") {
                greedy = problems::Knapsack01Problem::solve_greedy(inst, config.strategy_id);
                oracle = problems::Knapsack01Problem::solve_exact_dp(inst);
                ver = core::verify(problems::Knapsack01Problem::objective(), greedy, oracle);
            } else {
                problems::FractionalKnapsackInstance frac_inst{inst.items, inst.capacity};
                greedy = problems::FractionalKnapsackProblem::solve_greedy(frac_inst, config.strategy_id);
                oracle = problems::FractionalKnapsackProblem::solve_exact_lp(frac_inst);
                ver = core::verify(problems::FractionalKnapsackProblem::objective(), greedy, oracle);
            }

            if (ver.is_counterexample) {
                ++res.counterexamples_found;
                total_gap += ver.absolute_gap;
                if (ver.absolute_gap > res.worst_gap) {
                    res.worst_gap = ver.absolute_gap;
                }

                bool update_best = false;
                if (!res.has_counterexample) {
                    update_best = true;
                } else if (config.mode == SearchMode::WORST) {
                    if (ver.absolute_gap > res.best_verification.absolute_gap) update_best = true;
                } else if (config.mode == SearchMode::SMALLEST) {
                    auto best_inst = res.best_instance.get<problems::Knapsack01Instance>();
                    if (inst.items.size() < best_inst.items.size()) update_best = true;
                    else if (inst.items.size() == best_inst.items.size() && inst.capacity < best_inst.capacity) update_best = true;
                }

                if (update_best) {
                    res.has_counterexample = true;
                    res.best_instance = inst;
                    res.best_greedy = greedy;
                    res.best_oracle = oracle;
                    res.best_verification = ver;
                }

                if (config.mode == SearchMode::ANY) {
                    break;
                }
            }
        }
    } else if (config.problem_type == "activity_selection") {
        ActivitySelectionSearchSpace space;
        if (!config.search_space.is_null() && !config.search_space.empty()) {
            space = config.search_space.get<ActivitySelectionSearchSpace>();
        }
        res.estimated_space_size = space.estimate_cardinality();

        std::vector<problems::ActivitySelectionInstance> candidates;
        std::uniform_int_distribution<int64_t> dist_n(space.activity_count.min, space.activity_count.max);
        std::uniform_int_distribution<int64_t> dist_start(space.time_range.min, space.time_range.max);
        std::uniform_int_distribution<int64_t> dist_dur(space.duration_range.min, space.duration_range.max);

        for (uint64_t i = 0; i < config.max_candidates; ++i) {
            problems::ActivitySelectionInstance inst;
            int64_t n = dist_n(rng);
            for (int64_t j = 1; j <= n; ++j) {
                int64_t s = dist_start(rng);
                int64_t f = s + dist_dur(rng);
                inst.activities.push_back({j, s, f});
            }
            candidates.push_back(inst);
        }

        // Evaluate candidates
        for (const auto& inst : candidates) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - search_start).count() > static_cast<int64_t>(config.max_time_ms)) {
                time_limit_hit = true;
                break;
            }
            if (res.instances_evaluated >= config.max_candidates) {
                candidate_limit_hit = true;
                break;
            }

            ++res.instances_evaluated;
            auto greedy = problems::ActivitySelectionProblem::solve_greedy(inst, config.strategy_id);
            auto oracle = problems::ActivitySelectionProblem::solve_exact_dp(inst);
            auto ver = core::verify(problems::ActivitySelectionProblem::objective(), greedy, oracle);

            if (ver.is_counterexample) {
                ++res.counterexamples_found;
                total_gap += ver.absolute_gap;
                if (ver.absolute_gap > res.worst_gap) {
                    res.worst_gap = ver.absolute_gap;
                }

                bool update_best = false;
                if (!res.has_counterexample) {
                    update_best = true;
                } else if (config.mode == SearchMode::WORST) {
                    if (ver.absolute_gap > res.best_verification.absolute_gap) update_best = true;
                } else if (config.mode == SearchMode::SMALLEST) {
                    auto best_inst = res.best_instance.get<problems::ActivitySelectionInstance>();
                    if (inst.activities.size() < best_inst.activities.size()) update_best = true;
                }

                if (update_best) {
                    res.has_counterexample = true;
                    res.best_instance = inst;
                    res.best_greedy = greedy;
                    res.best_oracle = oracle;
                    res.best_verification = ver;
                }

                if (config.mode == SearchMode::ANY) {
                    break;
                }
            }
        }
    } else if (config.problem_type == "huffman_coding") {
        HuffmanCodingSearchSpace space;
        if (!config.search_space.is_null() && !config.search_space.empty()) {
            space = config.search_space.get<HuffmanCodingSearchSpace>();
        }
        res.estimated_space_size = space.estimate_cardinality();

        std::vector<problems::HuffmanCodingInstance> candidates;
        std::uniform_int_distribution<int64_t> dist_n(space.symbol_count.min, space.symbol_count.max);
        std::uniform_int_distribution<int64_t> dist_f(space.frequency_range.min, space.frequency_range.max);

        for (uint64_t i = 0; i < config.max_candidates; ++i) {
            problems::HuffmanCodingInstance inst;
            int64_t n = dist_n(rng);
            for (int64_t j = 1; j <= n; ++j) {
                std::string sym = "S" + std::to_string(j);
                inst.symbols.push_back({j, sym, dist_f(rng)});
            }
            candidates.push_back(inst);
        }

        // Evaluate candidates
        for (const auto& inst : candidates) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - search_start).count() > static_cast<int64_t>(config.max_time_ms)) {
                time_limit_hit = true;
                break;
            }
            if (res.instances_evaluated >= config.max_candidates) {
                candidate_limit_hit = true;
                break;
            }

            ++res.instances_evaluated;
            auto greedy = problems::HuffmanCodingProblem::solve_greedy(inst, config.strategy_id);
            auto oracle = problems::HuffmanCodingProblem::solve_exact(inst);
            auto ver = core::verify(problems::HuffmanCodingProblem::objective(), greedy, oracle);

            if (ver.is_counterexample) {
                ++res.counterexamples_found;
                total_gap += ver.absolute_gap;
                if (ver.absolute_gap > res.worst_gap) {
                    res.worst_gap = ver.absolute_gap;
                }

                bool update_best = false;
                if (!res.has_counterexample) {
                    update_best = true;
                } else if (config.mode == SearchMode::WORST) {
                    if (ver.absolute_gap > res.best_verification.absolute_gap) update_best = true;
                }

                if (update_best) {
                    res.has_counterexample = true;
                    res.best_instance = inst;
                    res.best_greedy = greedy;
                    res.best_oracle = oracle;
                    res.best_verification = ver;
                }

                if (config.mode == SearchMode::ANY) {
                    break;
                }
            }
        }
    } else {
        res.status = core::Status::InvalidConfiguration("Unknown problem type: " + config.problem_type);
        return res;
    }

    const auto search_end = std::chrono::steady_clock::now();
    res.elapsed_time_ms = std::chrono::duration<double, std::milli>(search_end - search_start).count();

    // Summary statistics
    if (res.instances_evaluated > 0) {
        res.failure_rate = static_cast<double>(res.counterexamples_found) / static_cast<double>(res.instances_evaluated);
    }
    if (res.counterexamples_found > 0) {
        res.average_gap = total_gap / static_cast<double>(res.counterexamples_found);
    }

    if (res.estimated_space_size > 0) {
        res.coverage_ratio = std::min(1.0, static_cast<double>(res.instances_evaluated) / static_cast<double>(res.estimated_space_size));
    } else {
        res.coverage_ratio = 1.0;
    }

    if (config.strategy == SearchStrategy::EXHAUSTIVE && !time_limit_hit && !candidate_limit_hit) {
        res.coverage = SearchCoverage::EXHAUSTIVE;
    } else if (config.strategy == SearchStrategy::RANDOMIZED) {
        res.coverage = SearchCoverage::RANDOMIZED_SAMPLE;
    } else {
        res.coverage = SearchCoverage::PARTIAL;
    }

    if (time_limit_hit) {
        res.status = core::Status::TimedOut("Search reached maximum time limit (" + std::to_string(config.max_time_ms) + " ms)");
    } else if (candidate_limit_hit) {
        res.status = core::Status::SearchIncomplete("Search reached maximum candidate evaluation limit (" + std::to_string(config.max_candidates) + ")");
    } else if (!res.has_counterexample) {
        res.status = core::Status::NoCounterexampleFound("Search completed: no counterexample discovered within bounds");
    } else {
        res.status = core::Status::OK();
    }

    return res;
}

} // namespace greedybreaker::search
