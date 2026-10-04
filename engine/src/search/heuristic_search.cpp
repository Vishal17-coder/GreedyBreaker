#include "greedybreaker/search/heuristic_search.hpp"
#include "greedybreaker/search/candidate_generators.hpp"
#include <algorithm>
#include <unordered_set>
#include <chrono>
#include <sstream>

namespace greedybreaker {

SearchResult HeuristicSearch::search(const SearchConfig& config) {
    config.validate();

    SearchResult result;
    result.mode = config.mode;
    result.strategy = SearchStrategy::HEURISTIC;
    result.problem_type = config.problem_type;
    result.search_space_description = config.describe_space();
    result.smallest_verified = false; // Heuristic ordering prioritizes failure severity, not size minimality

    const auto start_time = std::chrono::high_resolution_clock::now();

    if (config.problem_type == ProblemType::COIN_CHANGE) {
        CoinChangeProblem problem;
        std::unordered_set<std::string> evaluated_hashes;

        auto make_candidate_hash = [](int target, const std::vector<int>& denoms) {
            std::string s = std::to_string(target) + ":";
            for (int d : denoms) s += std::to_string(d) + ",";
            return s;
        };

        auto try_candidate = [&](int target, std::vector<int> denoms) -> bool {
            std::sort(denoms.begin(), denoms.end());
            denoms.erase(std::unique(denoms.begin(), denoms.end()), denoms.end());

            if (static_cast<int>(denoms.size()) < config.min_coin_count ||
                static_cast<int>(denoms.size()) > config.max_coin_count) {
                return false;
            }

            std::string h = make_candidate_hash(target, denoms);
            if (evaluated_hashes.find(h) != evaluated_hashes.end()) {
                return false;
            }
            evaluated_hashes.insert(h);

            if (config.should_cancel && config.should_cancel()) {
                result.search_completed = false;
                result.termination_reason = to_string(TerminationReason::CANCELLED_BY_USER);
                return true;
            }

            if (result.candidates_evaluated >= config.max_candidates) {
                result.search_completed = false;
                result.search_limit_reached = true;
                result.termination_reason = to_string(TerminationReason::MAX_CANDIDATES_REACHED);
                return true;
            }

            result.candidates_generated++;
            CoinChangeCandidate candidate{denoms, target};
            auto eval = evaluate_candidate(problem, candidate);
            result.candidates_evaluated++;

            if (eval.is_counterexample) {
                result.counterexamples_found++;

                if (config.mode == SearchMode::ANY) {
                    result.found = true;
                    result.best_candidate = candidate;
                    result.greedy_solution = eval.greedy;
                    result.optimal_solution = eval.optimal;
                    result.comparison_metrics = eval.comparison;
                    result.first_divergence = eval.divergence;
                    result.search_completed = true;
                    result.termination_reason = to_string(TerminationReason::FOUND_FIRST_COUNTEREXAMPLE);
                    return true;
                }

                if (config.mode == SearchMode::SMALLEST) {
                    if (!result.found || is_candidate_smaller(candidate, result.best_candidate.value())) {
                        result.found = true;
                        result.best_candidate = candidate;
                        result.greedy_solution = eval.greedy;
                        result.optimal_solution = eval.optimal;
                        result.comparison_metrics = eval.comparison;
                        result.first_divergence = eval.divergence;
                    }
                }

                if (config.mode == SearchMode::WORST) {
                    if (!result.found || is_severity_worse(
                            eval.comparison, candidate,
                            result.comparison_metrics.value(), result.best_candidate.value())) {
                        result.found = true;
                        result.best_candidate = candidate;
                        result.greedy_solution = eval.greedy;
                        result.optimal_solution = eval.optimal;
                        result.comparison_metrics = eval.comparison;
                        result.first_divergence = eval.divergence;
                    }
                }
            }
            return false;
        };

        // HEURISTIC TIER 1: Kozen-Zaks Double-Coin Traps
        for (int a = std::max(2, config.min_denomination); a <= config.max_denomination; ++a) {
            int target = 2 * a;
            if (target < config.min_target || target > config.max_target) continue;

            for (int b = a + 1; b <= std::min(2 * a - 1, config.max_denomination); ++b) {
                std::vector<int> base_coins;
                if (config.min_denomination <= 1) {
                    base_coins.push_back(1);
                } else {
                    base_coins.push_back(config.min_denomination);
                }
                base_coins.push_back(a);
                base_coins.push_back(b);

                if (try_candidate(target, base_coins)) goto finalize;

                for (int k = 4; k <= config.max_coin_count; ++k) {
                    for (int pad = config.min_denomination + 1; pad < a; ++pad) {
                        auto extended = base_coins;
                        extended.push_back(pad);
                        if (try_candidate(target, extended)) goto finalize;
                    }
                }
            }
        }

        // HEURISTIC TIER 2: Triple-Coin Traps
        for (int a = std::max(2, config.min_denomination); a <= config.max_denomination; ++a) {
            int target = 3 * a;
            if (target < config.min_target || target > config.max_target) continue;

            for (int b = 2 * a + 1; b <= std::min(3 * a - 1, config.max_denomination); ++b) {
                std::vector<int> base_coins = {1, a, b};
                if (try_candidate(target, base_coins)) goto finalize;
            }
        }

        // HEURISTIC TIER 3: Remainder Gaps Around Non-Super-Increasing Pairs
        for (int a = std::max(2, config.min_denomination); a <= config.max_denomination; ++a) {
            for (int delta = -1; delta <= 1; delta += 2) {
                int target = 2 * a + delta;
                if (target < config.min_target || target > config.max_target) continue;

                for (int b = a + 1; b <= std::min(target - 1, config.max_denomination); ++b) {
                    std::vector<int> coins = {1, a, b};
                    if (try_candidate(target, coins)) goto finalize;
                }
            }
        }

        // HEURISTIC TIER 4: Systematic Fallback
        for (int target = config.min_target; target <= config.max_target; ++target) {
            for (int k = config.min_coin_count; k <= config.max_coin_count; ++k) {
                std::vector<int> denoms;
                denoms.push_back(config.min_denomination);
                for (int i = 1; i < k; ++i) {
                    int next_d = denoms.back() + (target / (k + 1)) + 1;
                    if (next_d <= config.max_denomination) {
                        denoms.push_back(next_d);
                    }
                }
                if (static_cast<int>(denoms.size()) == k) {
                    if (try_candidate(target, denoms)) goto finalize;
                }
            }
        }
    } else if (config.problem_type == ProblemType::KNAPSACK_01) {
        Knapsack01Problem problem;

        auto try_knapsack_candidate = [&](const KnapsackCandidate& candidate) -> bool {
            if (config.should_cancel && config.should_cancel()) {
                result.search_completed = false;
                result.termination_reason = to_string(TerminationReason::CANCELLED_BY_USER);
                return true;
            }
            if (result.candidates_evaluated >= config.max_candidates) {
                result.search_completed = false;
                result.search_limit_reached = true;
                result.termination_reason = to_string(TerminationReason::MAX_CANDIDATES_REACHED);
                return true;
            }

            result.candidates_generated++;
            auto eval = evaluate_knapsack_candidate(problem, candidate);
            result.candidates_evaluated++;

            if (eval.is_counterexample) {
                result.counterexamples_found++;

                std::ostringstream ss;
                ss << "Capacity: " << candidate.capacity << ", Items: [";
                for (size_t i = 0; i < candidate.items.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << "(v=" << candidate.items[i].value << ", w=" << candidate.items[i].weight << ")";
                }
                ss << "]";

                if (config.mode == SearchMode::ANY) {
                    result.found = true;
                    result.best_knapsack_candidate = candidate;
                    result.greedy_knapsack_solution = eval.greedy;
                    result.optimal_knapsack_solution = eval.optimal;
                    result.comparison_metrics = eval.comparison;
                    result.first_divergence = eval.divergence;
                    result.candidate_summary = ss.str();
                    result.search_completed = true;
                    result.termination_reason = to_string(TerminationReason::FOUND_FIRST_COUNTEREXAMPLE);
                    return true;
                }
                if (config.mode == SearchMode::SMALLEST) {
                    if (!result.found || is_candidate_smaller(candidate, result.best_knapsack_candidate.value())) {
                        result.found = true;
                        result.best_knapsack_candidate = candidate;
                        result.greedy_knapsack_solution = eval.greedy;
                        result.optimal_knapsack_solution = eval.optimal;
                        result.comparison_metrics = eval.comparison;
                        result.first_divergence = eval.divergence;
                        result.candidate_summary = ss.str();
                    }
                }
                if (config.mode == SearchMode::WORST) {
                    if (!result.found || is_severity_worse(
                            eval.comparison, candidate,
                            result.comparison_metrics.value(), result.best_knapsack_candidate.value())) {
                        result.found = true;
                        result.best_knapsack_candidate = candidate;
                        result.greedy_knapsack_solution = eval.greedy;
                        result.optimal_knapsack_solution = eval.optimal;
                        result.comparison_metrics = eval.comparison;
                        result.first_divergence = eval.divergence;
                        result.candidate_summary = ss.str();
                    }
                }
            }
            return false;
        };

        // KNAPSACK HEURISTIC TIER 1: Stranded-Capacity Traps
        // W = w2 + w3, Item 1 has weight W-1 or W-2 and high ratio, blocking w2 and w3.
        for (int W = config.min_capacity; W <= config.max_capacity; ++W) {
            for (int w2 = config.min_weight; w2 < W; ++w2) {
                int w3 = W - w2;
                if (w3 < config.min_weight || w3 > config.max_weight) continue;

                for (int w1 = std::max(w2, w3); w1 < W && w1 <= config.max_weight; ++w1) {
                    // Ratio of Item 1 must be higher than ratios of 2 and 3, but v1 < v2 + v3
                    for (int v2 = config.min_value; v2 <= config.max_value / 2; ++v2) {
                        for (int v3 = config.min_value; v3 <= config.max_value / 2; ++v3) {
                            int v_sum = v2 + v3;
                            int v1 = v_sum - 1; // v1 < v_sum
                            if (v1 < config.min_value || v1 > config.max_value) continue;

                            double r1 = static_cast<double>(v1) / w1;
                            double r2 = static_cast<double>(v2) / w2;
                            double r3 = static_cast<double>(v3) / w3;

                            if (r1 > r2 && r1 > r3) {
                                KnapsackCandidate cand{
                                    .items = {
                                        KnapsackItem{.id = 1, .value = v1, .weight = w1},
                                        KnapsackItem{.id = 2, .value = v2, .weight = w2},
                                        KnapsackItem{.id = 3, .value = v3, .weight = w3}
                                    },
                                    .capacity = W
                                };
                                if (try_knapsack_candidate(cand)) goto finalize;
                            }
                        }
                    }
                }
            }
        }

        // KNAPSACK HEURISTIC TIER 2: Systematic streaming fallback
        stream_knapsack_candidates(config, [&](const KnapsackCandidate& cand) -> bool {
            return !try_knapsack_candidate(cand);
        });
    } else if (config.problem_type == ProblemType::ACTIVITY_SELECTION) {
        ActivitySelectionProblem problem;
        stream_activity_candidates(config, [&](const ActivitySelectionCandidate& cand) -> bool {
            if (config.should_cancel && config.should_cancel()) return false;
            if (result.candidates_evaluated >= config.max_candidates) return false;

            result.candidates_generated++;
            auto eval = evaluate_activity_candidate(problem, cand);
            result.candidates_evaluated++;

            if (eval.is_counterexample) {
                result.counterexamples_found++;
                result.found = true;
                result.best_activity_candidate = cand;
                result.greedy_activity_solution = eval.greedy;
                result.optimal_activity_solution = eval.optimal;
                result.comparison_metrics = eval.comparison;
                result.first_divergence = eval.divergence;
                return false;
            }
            return true;
        });
    } else if (config.problem_type == ProblemType::KNAPSACK_FRACTIONAL) {
        FractionalKnapsackProblem problem;
        stream_knapsack_candidates(config, [&](const KnapsackCandidate& cand) -> bool {
            if (config.should_cancel && config.should_cancel()) return false;
            if (result.candidates_evaluated >= config.max_candidates) return false;

            result.candidates_generated++;
            auto eval = evaluate_fractional_candidate(problem, cand);
            result.candidates_evaluated++;

            if (eval.is_counterexample) {
                result.counterexamples_found++;
                result.found = true;
                result.best_knapsack_candidate = cand;
                result.greedy_fractional_solution = eval.greedy;
                result.optimal_fractional_solution = eval.optimal;
                result.comparison_metrics = eval.comparison;
                result.first_divergence = eval.divergence;
                return false;
            }
            return true;
        });
    }

finalize:
    if (result.termination_reason.empty()) {
        result.search_completed = true;
        if (result.found) {
            result.termination_reason = "Heuristic candidate search completed successfully";
        } else {
            result.termination_reason = "Heuristic search completed; no counterexample discovered";
        }
    }

    const auto end_time = std::chrono::high_resolution_clock::now();
    result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    return result;
}

} // namespace greedybreaker
