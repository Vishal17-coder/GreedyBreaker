#include "greedybreaker/search/randomized_search.hpp"
#include <random>
#include <algorithm>
#include <chrono>
#include <sstream>

namespace greedybreaker {

SearchResult RandomizedSearch::search(const SearchConfig& config) {
    config.validate();

    SearchResult result;
    result.mode = config.mode;
    result.strategy = SearchStrategy::RANDOMIZED;
    result.problem_type = config.problem_type;
    result.random_seed = config.random_seed;
    result.search_space_description = config.describe_space();
    result.smallest_verified = false;

    const auto start_time = std::chrono::high_resolution_clock::now();
    std::mt19937 rng(config.random_seed);

    if (config.problem_type == ProblemType::COIN_CHANGE) {
        CoinChangeProblem problem;
        std::uniform_int_distribution<int> target_dist(config.min_target, config.max_target);
        std::uniform_int_distribution<int> k_dist(config.min_coin_count, config.max_coin_count);

        int start_pool_val = config.require_unit_coin ? std::max(2, config.min_denomination) : config.min_denomination;
        int end_pool_val = config.max_denomination;
        int range_size = std::max(0, end_pool_val - start_pool_val + 1);

        std::vector<int> pool(range_size);
        for (int i = 0; i < range_size; ++i) {
            pool[i] = start_pool_val + i;
        }

        for (size_t iter = 1; iter <= config.max_iterations; ++iter) {
            result.iterations = iter;

            if (config.should_cancel && config.should_cancel()) {
                result.search_completed = false;
                result.termination_reason = to_string(TerminationReason::CANCELLED_BY_USER);
                break;
            }

            if (result.candidates_evaluated >= config.max_candidates) {
                result.search_completed = false;
                result.search_limit_reached = true;
                result.termination_reason = to_string(TerminationReason::MAX_CANDIDATES_REACHED);
                break;
            }

            int target = target_dist(rng);
            int k = k_dist(rng);

            std::vector<int> denominations;

            if (config.require_unit_coin) {
                denominations.push_back(1);
                int rem_needed = k - 1;
                if (rem_needed > range_size) rem_needed = range_size;

                std::vector<int> current_pool = pool;
                for (int i = 0; i < rem_needed; ++i) {
                    std::uniform_int_distribution<int> pick(i, range_size - 1);
                    int idx = pick(rng);
                    std::swap(current_pool[i], current_pool[idx]);
                    denominations.push_back(current_pool[i]);
                }
            } else {
                if (k > range_size) k = range_size;
                std::vector<int> current_pool = pool;
                for (int i = 0; i < k; ++i) {
                    std::uniform_int_distribution<int> pick(i, range_size - 1);
                    int idx = pick(rng);
                    std::swap(current_pool[i], current_pool[idx]);
                    denominations.push_back(current_pool[i]);
                }
            }

            std::sort(denominations.begin(), denominations.end());

            result.candidates_generated++;
            CoinChangeCandidate candidate{denominations, target};

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
                    break;
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
        }
    } else if (config.problem_type == ProblemType::ACTIVITY_SELECTION) {
        ActivitySelectionProblem problem;
        std::uniform_int_distribution<int> k_dist(config.min_activities, config.max_activities);
        std::uniform_int_distribution<int> start_dist(config.min_time, config.max_time - config.min_duration);

        for (size_t iter = 1; iter <= config.max_iterations; ++iter) {
            result.iterations = iter;

            if (config.should_cancel && config.should_cancel()) {
                result.search_completed = false;
                result.termination_reason = to_string(TerminationReason::CANCELLED_BY_USER);
                break;
            }

            if (result.candidates_evaluated >= config.max_candidates) {
                result.search_completed = false;
                result.search_limit_reached = true;
                result.termination_reason = to_string(TerminationReason::MAX_CANDIDATES_REACHED);
                break;
            }

            int k = k_dist(rng);
            ActivitySelectionCandidate candidate;
            candidate.activities.reserve(k);

            for (int i = 0; i < k; ++i) {
                int s = start_dist(rng);
                std::uniform_int_distribution<int> finish_dist(s + config.min_duration, config.max_time);
                int f = finish_dist(rng);
                candidate.activities.push_back(Activity{.id = i + 1, .start = s, .finish = f});
            }

            result.candidates_generated++;
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
                break;
            }
        }
    } else if (config.problem_type == ProblemType::KNAPSACK_01) {
        Knapsack01Problem problem;
        std::uniform_int_distribution<int> cap_dist(config.min_capacity, config.max_capacity);
        std::uniform_int_distribution<int> k_dist(config.min_items, config.max_items);
        std::uniform_int_distribution<int> val_dist(config.min_value, config.max_value);

        for (size_t iter = 1; iter <= config.max_iterations; ++iter) {
            result.iterations = iter;

            if (config.should_cancel && config.should_cancel()) {
                result.search_completed = false;
                result.termination_reason = to_string(TerminationReason::CANCELLED_BY_USER);
                break;
            }

            if (result.candidates_evaluated >= config.max_candidates) {
                result.search_completed = false;
                result.search_limit_reached = true;
                result.termination_reason = to_string(TerminationReason::MAX_CANDIDATES_REACHED);
                break;
            }

            int W = cap_dist(rng);
            int k = k_dist(rng);
            std::uniform_int_distribution<int> wt_dist(config.min_weight, std::min(config.max_weight, W));

            KnapsackCandidate candidate;
            candidate.capacity = W;
            candidate.items.reserve(k);

            for (int i = 0; i < k; ++i) {
                candidate.items.push_back(KnapsackItem{
                    .id = i + 1,
                    .value = val_dist(rng),
                    .weight = wt_dist(rng)
                });
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
                    break;
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
        }
    } else if (config.problem_type == ProblemType::KNAPSACK_FRACTIONAL) {
        FractionalKnapsackProblem problem;
        std::uniform_int_distribution<int> cap_dist(config.min_capacity, config.max_capacity);
        std::uniform_int_distribution<int> k_dist(config.min_items, config.max_items);
        std::uniform_int_distribution<int> val_dist(config.min_value, config.max_value);

        for (size_t iter = 1; iter <= config.max_iterations; ++iter) {
            result.iterations = iter;

            if (config.should_cancel && config.should_cancel()) {
                result.search_completed = false;
                result.termination_reason = to_string(TerminationReason::CANCELLED_BY_USER);
                break;
            }

            if (result.candidates_evaluated >= config.max_candidates) {
                result.search_completed = false;
                result.search_limit_reached = true;
                result.termination_reason = to_string(TerminationReason::MAX_CANDIDATES_REACHED);
                break;
            }

            int W = cap_dist(rng);
            int k = k_dist(rng);
            std::uniform_int_distribution<int> wt_dist(config.min_weight, std::min(config.max_weight, W));

            KnapsackCandidate candidate;
            candidate.capacity = W;
            candidate.items.reserve(k);

            for (int i = 0; i < k; ++i) {
                candidate.items.push_back(KnapsackItem{
                    .id = i + 1,
                    .value = val_dist(rng),
                    .weight = wt_dist(rng)
                });
            }

            result.candidates_generated++;
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
                break;
            }
        }
    }

    if (result.termination_reason.empty()) {
        result.search_completed = true;
        if (result.found) {
            result.termination_reason = to_string(TerminationReason::MAX_ITERATIONS_REACHED);
        } else {
            result.termination_reason = "Max iterations completed; no counterexample discovered";
        }
    }

    const auto end_time = std::chrono::high_resolution_clock::now();
    result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    return result;
}

} // namespace greedybreaker
