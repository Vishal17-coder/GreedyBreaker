#include "greedybreaker/search/backtracking_search.hpp"
#include <numeric>
#include <chrono>
#include <algorithm>
#include <sstream>

namespace greedybreaker {

SearchResult BacktrackingSearch::search(const SearchConfig& config) {
    config.validate();

    SearchResult result;
    result.mode = config.mode;
    result.strategy = SearchStrategy::BACKTRACKING;
    result.problem_type = config.problem_type;
    result.search_space_description = config.describe_space();

    const auto start_time = std::chrono::high_resolution_clock::now();

    if (config.problem_type == ProblemType::COIN_CHANGE) {
        CoinChangeProblem problem;

        for (int target = config.min_target; target <= config.max_target; ++target) {
            for (int k = config.min_coin_count; k <= config.max_coin_count; ++k) {
                if (config.require_unit_coin) {
                    if (k == 1) {
                        result.branches_pruned++;
                        continue;
                    }
                    int rem_k = k - 1;
                    int start_val = std::max(2, config.min_denomination);
                    int avail = config.max_denomination - start_val + 1;
                    if (rem_k > avail) {
                        result.branches_pruned++;
                        continue;
                    }

                    std::vector<int> current_denoms = {1};
                    bool stop_search = backtrack_coin(
                        config, problem, target, k,
                        start_val, current_denoms, result
                    );
                    if (stop_search) {
                        const auto end_time = std::chrono::high_resolution_clock::now();
                        result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
                        return result;
                    }
                } else {
                    if (k == 1) {
                        result.branches_pruned++;
                        continue;
                    }
                    int range_size = config.max_denomination - config.min_denomination + 1;
                    if (k > range_size) {
                        result.branches_pruned++;
                        continue;
                    }

                    std::vector<int> current_denoms;
                    bool stop_search = backtrack_coin(
                        config, problem, target, k,
                        config.min_denomination, current_denoms, result
                    );
                    if (stop_search) {
                        const auto end_time = std::chrono::high_resolution_clock::now();
                        result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
                        return result;
                    }
                }
            }
        }
    } else if (config.problem_type == ProblemType::ACTIVITY_SELECTION) {
        ActivitySelectionProblem problem;
        std::vector<std::pair<int, int>> interval_pool;
        for (int s = config.min_time; s <= config.max_time - config.min_duration; ++s) {
            for (int f = s + config.min_duration; f <= config.max_time; ++f) {
                interval_pool.push_back({s, f});
            }
        }

        for (int k = config.min_activities; k <= config.max_activities; ++k) {
            std::vector<Activity> current_acts;
            bool stop = backtrack_activity(config, problem, k, 0, interval_pool, current_acts, result);
            if (stop) break;
        }
    } else if (config.problem_type == ProblemType::KNAPSACK_01) {
        Knapsack01Problem problem;
        for (int W = config.min_capacity; W <= config.max_capacity; ++W) {
            std::vector<std::pair<int, int>> item_pool;
            int max_w = std::min(config.max_weight, W);
            for (int w = config.min_weight; w <= max_w; ++w) {
                for (int v = config.min_value; v <= config.max_value; ++v) {
                    item_pool.push_back({v, w});
                }
            }
            std::sort(item_pool.begin(), item_pool.end(), [](const auto& a, const auto& b) {
                double r_a = static_cast<double>(a.first) / a.second;
                double r_b = static_cast<double>(b.first) / b.second;
                if (std::abs(r_a - r_b) > 1e-9) return r_a > r_b;
                if (a.first != b.first) return a.first > b.first;
                return a.second < b.second;
            });

            for (int k = config.min_items; k <= config.max_items; ++k) {
                std::vector<KnapsackItem> current_items;
                bool stop = backtrack_knapsack(config, problem, W, k, 0, item_pool, current_items, 0, result);
                if (stop) {
                    const auto end_time = std::chrono::high_resolution_clock::now();
                    result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
                    return result;
                }
            }
        }
    } else if (config.problem_type == ProblemType::KNAPSACK_FRACTIONAL) {
        FractionalKnapsackProblem problem;
        for (int W = config.min_capacity; W <= config.max_capacity; ++W) {
            std::vector<std::pair<int, int>> item_pool;
            int max_w = std::min(config.max_weight, W);
            for (int w = config.min_weight; w <= max_w; ++w) {
                for (int v = config.min_value; v <= config.max_value; ++v) {
                    item_pool.push_back({v, w});
                }
            }
            std::sort(item_pool.begin(), item_pool.end(), [](const auto& a, const auto& b) {
                double r_a = static_cast<double>(a.first) / a.second;
                double r_b = static_cast<double>(b.first) / b.second;
                if (std::abs(r_a - r_b) > 1e-9) return r_a > r_b;
                if (a.first != b.first) return a.first > b.first;
                return a.second < b.second;
            });

            for (int k = config.min_items; k <= config.max_items; ++k) {
                std::vector<KnapsackItem> current_items;
                bool stop = backtrack_fractional(config, problem, W, k, 0, item_pool, current_items, 0, result);
                if (stop) {
                    const auto end_time = std::chrono::high_resolution_clock::now();
                    result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
                    return result;
                }
            }
        }
    }

    result.search_completed = true;
    if (result.found) {
        result.termination_reason = to_string(TerminationReason::COMPLETED_SPACE_EXHAUSTED);
    } else {
        result.termination_reason = "Search space exhausted; no counterexample found within configured bounds";
    }

    const auto end_time = std::chrono::high_resolution_clock::now();
    result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    return result;
}

bool BacktrackingSearch::backtrack_coin(
    const SearchConfig& config,
    const CoinChangeProblem& problem,
    int target,
    int target_k,
    int next_val,
    std::vector<int>& current_denoms,
    SearchResult& result
) {
    result.nodes_explored++;

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

    if (static_cast<int>(current_denoms.size()) == target_k) {
        int g = current_denoms[0];
        for (size_t i = 1; i < current_denoms.size(); ++i) {
            g = std::gcd(g, current_denoms[i]);
        }
        if (target % g != 0) {
            result.branches_pruned++;
            return false;
        }

        result.candidates_generated++;
        CoinChangeCandidate candidate{current_denoms, target};
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
                result.found = true;
                result.best_candidate = candidate;
                result.greedy_solution = eval.greedy;
                result.optimal_solution = eval.optimal;
                result.comparison_metrics = eval.comparison;
                result.first_divergence = eval.divergence;
                result.search_completed = true;
                result.smallest_verified = true;
                result.termination_reason = to_string(TerminationReason::FOUND_SMALLEST_VERIFIED);
                return true;
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
    }

    for (int val = next_val; val <= config.max_denomination; ++val) {
        if (current_denoms.empty() && val > target) {
            result.branches_pruned++;
            break;
        }

        int needed = target_k - static_cast<int>(current_denoms.size());
        int available = config.max_denomination - val + 1;
        if (available < needed) {
            result.branches_pruned++;
            break;
        }

        current_denoms.push_back(val);
        bool should_stop = backtrack_coin(
            config, problem, target, target_k, val + 1, current_denoms, result
        );
        current_denoms.pop_back();

        if (should_stop) {
            return true;
        }
    }

    return false;
}

bool BacktrackingSearch::backtrack_knapsack(
    const SearchConfig& config,
    const Knapsack01Problem& problem,
    int capacity,
    int target_k,
    size_t next_item_idx,
    const std::vector<std::pair<int, int>>& item_pool,
    std::vector<KnapsackItem>& current_items,
    int current_weight_sum,
    SearchResult& result
) {
    result.nodes_explored++;

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

    if (static_cast<int>(current_items.size()) == target_k) {
        // Pruning: if all items fit within capacity, greedy trivially takes all (guaranteed optimal)
        if (current_weight_sum <= capacity) {
            result.branches_pruned++;
            return false;
        }

        result.candidates_generated++;
        KnapsackCandidate candidate{current_items, capacity};
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
                result.found = true;
                result.best_knapsack_candidate = candidate;
                result.greedy_knapsack_solution = eval.greedy;
                result.optimal_knapsack_solution = eval.optimal;
                result.comparison_metrics = eval.comparison;
                result.first_divergence = eval.divergence;
                result.candidate_summary = ss.str();
                result.search_completed = true;
                result.smallest_verified = true;
                result.termination_reason = to_string(TerminationReason::FOUND_SMALLEST_VERIFIED);
                return true;
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
    }

    const size_t pool_size = item_pool.size();
    for (size_t idx = next_item_idx; idx < pool_size; ++idx) {
        const int needed = target_k - static_cast<int>(current_items.size());
        const int available = static_cast<int>(pool_size - idx);
        if (available < needed) {
            result.branches_pruned++;
            break;
        }

        KnapsackItem item{
            .id = static_cast<int>(current_items.size()) + 1,
            .value = item_pool[idx].first,
            .weight = item_pool[idx].second
        };

        current_items.push_back(item);
        bool should_stop = backtrack_knapsack(
            config, problem, capacity, target_k, idx + 1,
            item_pool, current_items, current_weight_sum + item.weight, result
        );
        current_items.pop_back();

        if (should_stop) {
            return true;
        }
    }

    return false;
}

bool BacktrackingSearch::backtrack_fractional(
    const SearchConfig& config,
    const FractionalKnapsackProblem& problem,
    int capacity,
    int target_k,
    size_t next_item_idx,
    const std::vector<std::pair<int, int>>& item_pool,
    std::vector<KnapsackItem>& current_items,
    int current_weight_sum,
    SearchResult& result
) {
    result.nodes_explored++;

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

    if (static_cast<int>(current_items.size()) == target_k) {
        result.candidates_generated++;
        KnapsackCandidate candidate{current_items, capacity};
        auto eval = evaluate_fractional_candidate(problem, candidate);
        result.candidates_evaluated++;

        if (eval.is_counterexample) {
            result.counterexamples_found++;
            result.found = true;
            result.best_knapsack_candidate = candidate;
            result.greedy_fractional_solution = eval.greedy;
            result.optimal_fractional_solution = eval.optimal;
            result.comparison_metrics = eval.comparison;
            result.first_divergence = eval.divergence;
            return true;
        }
        return false;
    }

    const size_t pool_size = item_pool.size();
    for (size_t idx = next_item_idx; idx < pool_size; ++idx) {
        const int needed = target_k - static_cast<int>(current_items.size());
        const int available = static_cast<int>(pool_size - idx);
        if (available < needed) {
            result.branches_pruned++;
            break;
        }

        KnapsackItem item{
            .id = static_cast<int>(current_items.size()) + 1,
            .value = item_pool[idx].first,
            .weight = item_pool[idx].second
        };

        current_items.push_back(item);
        bool should_stop = backtrack_fractional(
            config, problem, capacity, target_k, idx + 1,
            item_pool, current_items, current_weight_sum + item.weight, result
        );
        current_items.pop_back();

        if (should_stop) {
            return true;
        }
    }

    return false;
}

bool BacktrackingSearch::backtrack_activity(
    const SearchConfig& config,
    const ActivitySelectionProblem& problem,
    int target_k,
    size_t next_interval_idx,
    const std::vector<std::pair<int, int>>& interval_pool,
    std::vector<Activity>& current_activities,
    SearchResult& result
) {
    result.nodes_explored++;

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

    if (static_cast<int>(current_activities.size()) == target_k) {
        result.candidates_generated++;
        ActivitySelectionCandidate candidate{current_activities};
        auto eval = evaluate_activity_candidate(problem, candidate);
        result.candidates_evaluated++;

        if (eval.is_counterexample) {
            result.counterexamples_found++;
            result.found = true;
            result.best_activity_candidate = candidate;
            result.greedy_activity_solution = eval.greedy;
            result.optimal_activity_solution = eval.optimal;
            result.comparison_metrics = eval.comparison;
            result.first_divergence = eval.divergence;
            return true;
        }
        return false;
    }

    const size_t pool_size = interval_pool.size();
    for (size_t idx = next_interval_idx; idx < pool_size; ++idx) {
        const int needed = target_k - static_cast<int>(current_activities.size());
        const int available = static_cast<int>(pool_size - idx);
        if (available < needed) {
            result.branches_pruned++;
            break;
        }

        Activity act{
            .id = static_cast<int>(current_activities.size()) + 1,
            .start = interval_pool[idx].first,
            .finish = interval_pool[idx].second
        };

        current_activities.push_back(act);
        bool should_stop = backtrack_activity(
            config, problem, target_k, idx + 1,
            interval_pool, current_activities, result
        );
        current_activities.pop_back();

        if (should_stop) {
            return true;
        }
    }

    return false;
}

} // namespace greedybreaker
