#include "greedybreaker/search/search_engine.hpp"
#include <cmath>
#include <algorithm>

namespace greedybreaker {

bool is_candidate_smaller(const CoinChangeCandidate& a, const CoinChangeCandidate& b) {
    if (a.target != b.target) {
        return a.target < b.target;
    }
    if (a.denominations.size() != b.denominations.size()) {
        return a.denominations.size() < b.denominations.size();
    }
    return a.denominations < b.denominations;
}

bool is_candidate_smaller(const ActivitySelectionCandidate& a, const ActivitySelectionCandidate& b) {
    if (a.activities.size() != b.activities.size()) {
        return a.activities.size() < b.activities.size();
    }
    for (size_t i = 0; i < a.activities.size(); ++i) {
        if (a.activities[i] == b.activities[i]) continue;
        return a.activities[i] < b.activities[i];
    }
    return false;
}

bool is_candidate_smaller(const KnapsackCandidate& a, const KnapsackCandidate& b) {
    if (a.capacity != b.capacity) {
        return a.capacity < b.capacity;
    }
    if (a.items.size() != b.items.size()) {
        return a.items.size() < b.items.size();
    }
    for (size_t i = 0; i < a.items.size(); ++i) {
        if (a.items[i].id != b.items[i].id) return a.items[i].id < b.items[i].id;
        if (a.items[i].value != b.items[i].value) return a.items[i].value < b.items[i].value;
        if (a.items[i].weight != b.items[i].weight) return a.items[i].weight < b.items[i].weight;
    }
    return false;
}

bool is_severity_worse(
    const ComparisonResult& a_cmp,
    const CoinChangeCandidate& a_cand,
    const ComparisonResult& b_cmp,
    const CoinChangeCandidate& b_cand
) {
    const double a_ratio = a_cmp.ratio.value_or(0.0);
    const double b_ratio = b_cmp.ratio.value_or(0.0);

    // 1. Primary criterion: Competitiveness Ratio
    if (std::abs(a_ratio - b_ratio) > 1e-9) {
        return a_ratio > b_ratio;
    }

    // 2. Secondary criterion: Absolute Gap
    const double a_gap = a_cmp.absolute_gap.value_or(0.0);
    const double b_gap = b_cmp.absolute_gap.value_or(0.0);
    if (std::abs(a_gap - b_gap) > 1e-9) {
        return a_gap > b_gap;
    }

    // 3. Deterministic tie-breaker: Smallest candidate
    return is_candidate_smaller(a_cand, b_cand);
}

bool is_severity_worse(
    const ComparisonResult& a_cmp,
    const KnapsackCandidate& a_cand,
    const ComparisonResult& b_cmp,
    const KnapsackCandidate& b_cand
) {
    const double a_ratio = a_cmp.ratio.value_or(0.0);
    const double b_ratio = b_cmp.ratio.value_or(0.0);

    // 1. Primary criterion: Deficit factor / Competitiveness Ratio
    if (std::abs(a_ratio - b_ratio) > 1e-9) {
        return a_ratio > b_ratio;
    }

    // 2. Secondary criterion: Absolute Gap
    const double a_gap = a_cmp.absolute_gap.value_or(0.0);
    const double b_gap = b_cmp.absolute_gap.value_or(0.0);
    if (std::abs(a_gap - b_gap) > 1e-9) {
        return a_gap > b_gap;
    }

    // 3. Deterministic tie-breaker: Smallest candidate
    return is_candidate_smaller(a_cand, b_cand);
}

CandidateEvaluation evaluate_candidate(
    const CoinChangeProblem& problem,
    const CoinChangeCandidate& candidate
) {
    CandidateEvaluation eval;
    eval.greedy = problem.greedy_solution(candidate);
    eval.optimal = problem.optimal_solution(candidate);
    eval.comparison = problem.compare_solutions(eval.greedy, eval.optimal);
    eval.is_counterexample = eval.comparison.is_counterexample;

    if (eval.is_counterexample) {
        eval.divergence = problem.analyze_divergence(candidate, eval.greedy, eval.optimal);
    }
    return eval;
}

ActivityCandidateEvaluation evaluate_activity_candidate(
    const ActivitySelectionProblem& problem,
    const ActivitySelectionCandidate& candidate
) {
    ActivityCandidateEvaluation eval;
    eval.greedy = problem.greedy_solution(candidate);
    eval.optimal = problem.optimal_solution(candidate);
    eval.comparison = problem.compare_solutions(eval.greedy, eval.optimal);
    eval.is_counterexample = eval.comparison.is_counterexample;

    if (eval.is_counterexample) {
        eval.divergence = problem.analyze_divergence(candidate, eval.greedy, eval.optimal);
    }
    return eval;
}

KnapsackCandidateEvaluation evaluate_knapsack_candidate(
    const Knapsack01Problem& problem,
    const KnapsackCandidate& candidate
) {
    KnapsackCandidateEvaluation eval;
    eval.greedy = problem.greedy_solution(candidate);
    eval.optimal = problem.optimal_solution(candidate);
    eval.comparison = problem.compare_solutions(eval.greedy, eval.optimal);
    eval.is_counterexample = eval.comparison.is_counterexample;

    if (eval.is_counterexample) {
        eval.divergence = problem.analyze_divergence(candidate, eval.greedy, eval.optimal);
    }
    return eval;
}

FractionalKnapsackCandidateEvaluation evaluate_fractional_candidate(
    const FractionalKnapsackProblem& problem,
    const KnapsackCandidate& candidate
) {
    FractionalKnapsackCandidateEvaluation eval;
    eval.greedy = problem.greedy_solution(candidate);
    eval.optimal = problem.optimal_solution(candidate);
    eval.comparison = problem.compare_solutions(eval.greedy, eval.optimal);
    eval.is_counterexample = eval.comparison.is_counterexample;

    if (eval.is_counterexample) {
        eval.divergence = problem.analyze_divergence(candidate, eval.greedy, eval.optimal);
    }
    return eval;
}

} // namespace greedybreaker
