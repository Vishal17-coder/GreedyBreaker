#include "greedybreaker/search/exhaustive_search.hpp"
#include "greedybreaker/search/candidate_generators.hpp"
#include <chrono>
#include <algorithm>
#include <sstream>

namespace greedybreaker {

SearchResult ExhaustiveSearch::search(const SearchConfig& config) {
    config.validate();

    SearchResult result;
    result.mode = config.mode;
    result.strategy = SearchStrategy::EXHAUSTIVE;
    result.problem_type = config.problem_type;
    result.search_space_description = config.describe_space();

    const auto start_time = std::chrono::high_resolution_clock::now();

    if (config.problem_type == ProblemType::COIN_CHANGE) {
        CoinChangeProblem problem;
        stream_coin_change_candidates(config, [&](const CoinChangeCandidate& candidate) -> bool {
            if (config.should_cancel && config.should_cancel()) {
                result.search_completed = false;
                result.termination_reason = to_string(TerminationReason::CANCELLED_BY_USER);
                return false;
            }
            if (result.candidates_evaluated >= config.max_candidates) {
                result.search_completed = false;
                result.search_limit_reached = true;
                result.termination_reason = to_string(TerminationReason::MAX_CANDIDATES_REACHED);
                return false;
            }

            result.candidates_generated++;
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
                    return false;
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
                    return false;
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
            return true;
        });
    } else if (config.problem_type == ProblemType::ACTIVITY_SELECTION) {
        ActivitySelectionProblem problem;
        stream_activity_candidates(config, [&](const ActivitySelectionCandidate& candidate) -> bool {
            if (config.should_cancel && config.should_cancel()) {
                result.search_completed = false;
                result.termination_reason = to_string(TerminationReason::CANCELLED_BY_USER);
                return false;
            }
            if (result.candidates_evaluated >= config.max_candidates) {
                result.search_completed = false;
                result.search_limit_reached = true;
                result.termination_reason = to_string(TerminationReason::MAX_CANDIDATES_REACHED);
                return false;
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
                return false;
            }
            return true;
        });
    } else if (config.problem_type == ProblemType::KNAPSACK_01) {
        Knapsack01Problem problem;
        stream_knapsack_candidates(config, [&](const KnapsackCandidate& candidate) -> bool {
            if (config.should_cancel && config.should_cancel()) {
                result.search_completed = false;
                result.termination_reason = to_string(TerminationReason::CANCELLED_BY_USER);
                return false;
            }
            if (result.candidates_evaluated >= config.max_candidates) {
                result.search_completed = false;
                result.search_limit_reached = true;
                result.termination_reason = to_string(TerminationReason::MAX_CANDIDATES_REACHED);
                return false;
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
                    return false;
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
                    return false;
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
            return true;
        });
    } else if (config.problem_type == ProblemType::KNAPSACK_FRACTIONAL) {
        FractionalKnapsackProblem problem;
        stream_knapsack_candidates(config, [&](const KnapsackCandidate& candidate) -> bool {
            if (config.should_cancel && config.should_cancel()) {
                result.search_completed = false;
                result.termination_reason = to_string(TerminationReason::CANCELLED_BY_USER);
                return false;
            }
            if (result.candidates_evaluated >= config.max_candidates) {
                result.search_completed = false;
                result.search_limit_reached = true;
                result.termination_reason = to_string(TerminationReason::MAX_CANDIDATES_REACHED);
                return false;
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
                return false;
            }
            return true;
        });
    }

    if (result.termination_reason.empty()) {
        result.search_completed = true;
        if (result.found) {
            result.termination_reason = to_string(TerminationReason::COMPLETED_SPACE_EXHAUSTED);
        } else {
            result.termination_reason = "Search space exhausted; no counterexample found within configured bounds";
        }
    }

    const auto end_time = std::chrono::high_resolution_clock::now();
    result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    return result;
}

} // namespace greedybreaker
