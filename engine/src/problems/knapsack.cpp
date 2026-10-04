#include "greedybreaker/problems/knapsack.hpp"
#include <algorithm>
#include <unordered_set>
#include <sstream>
#include <iomanip>

namespace greedybreaker {

// ============================================================================
// 0/1 KNAPSACK PROBLEM
// ============================================================================

std::string Knapsack01Problem::name() const {
    return "0/1 Knapsack";
}

ProblemObjective Knapsack01Problem::objective() const {
    return ProblemObjective::Maximize;
}

std::string Knapsack01Problem::default_strategy() const {
    return STRATEGY_VALUE_WEIGHT_RATIO;
}

std::vector<std::string> Knapsack01Problem::available_strategies() const {
    return {STRATEGY_VALUE_WEIGHT_RATIO};
}

void Knapsack01Problem::validate_candidate(const KnapsackCandidate& candidate) const {
    if (candidate.items.empty()) {
        throw InvalidCandidateException("Knapsack items list cannot be empty.");
    }
    if (candidate.capacity <= 0) {
        throw InvalidCandidateException("Knapsack capacity must be strictly positive: found capacity=" +
                                        std::to_string(candidate.capacity));
    }

    std::unordered_set<int> seen_ids;
    for (const auto& item : candidate.items) {
        if (item.weight <= 0) {
            throw InvalidCandidateException("Item weight must be strictly positive: item ID " +
                                            std::to_string(item.id) + " has weight " +
                                            std::to_string(item.weight));
        }
        if (item.value < 0) {
            throw InvalidCandidateException("Item value cannot be negative: item ID " +
                                            std::to_string(item.id) + " has value " +
                                            std::to_string(item.value));
        }
        if (!seen_ids.insert(item.id).second) {
            throw InvalidCandidateException("Duplicate item ID detected: " + std::to_string(item.id));
        }
    }
}

Solution<KnapsackItem> Knapsack01Problem::greedy_solution(
    const KnapsackCandidate& candidate,
    const std::string& strategy
) const {
    validate_candidate(candidate);

    Solution<KnapsackItem> solution;
    solution.trace.algorithm_name = "Greedy 0/1 Knapsack";
    solution.trace.strategy_name = strategy.empty() ? STRATEGY_VALUE_WEIGHT_RATIO : strategy;

    std::vector<KnapsackItem> sorted_items = candidate.items;
    std::sort(sorted_items.begin(), sorted_items.end(), knapsack_item_ratio_comparator);

    int remaining_capacity = candidate.capacity;
    int total_value = 0;
    int step_num = 1;

    for (const auto& item : sorted_items) {
        TraceStep step;
        step.step_number = step_num++;
        
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Evaluating Item " << item.id << " (v=" << item.value << ", w=" << item.weight
           << ", ratio=" << item.ratio() << ")";
        step.state_description = ss.str();
        step.remaining_target = remaining_capacity;
        step.available_choices = {item.id};

        if (item.weight <= remaining_capacity) {
            step.selected_choice = item.id;
            step.outcome_description = "SELECTED: Item " + std::to_string(item.id) +
                                       " (weight " + std::to_string(item.weight) + " fits in remaining " +
                                       std::to_string(remaining_capacity) + ")";
            remaining_capacity -= item.weight;
            total_value += item.value;
            step.remaining_after = remaining_capacity;
            step.accumulated_count = total_value;
            
            std::ostringstream r_ss;
            r_ss << std::fixed << std::setprecision(2);
            r_ss << "Highest available ratio (" << item.ratio() << ") that fits in remaining capacity.";
            step.rationale = r_ss.str();
            
            solution.items.push_back(item);
        } else {
            step.selected_choice = std::nullopt;
            step.outcome_description = "SKIPPED: Item " + std::to_string(item.id) +
                                       " (weight " + std::to_string(item.weight) +
                                       " exceeds remaining capacity " + std::to_string(remaining_capacity) + ")";
            step.remaining_after = remaining_capacity;
            step.accumulated_count = total_value;
            step.rationale = "Capacity exceeded: item weight (" + std::to_string(item.weight) +
                             ") > remaining capacity (" + std::to_string(remaining_capacity) + ").";
        }

        solution.trace.steps.push_back(step);
    }

    solution.success = true;
    solution.cost = static_cast<double>(total_value);
    solution.trace.success = true;
    solution.trace.final_cost = solution.cost;
    for (const auto& it : solution.items) {
        solution.trace.final_solution.push_back(it.id);
    }

    return solution;
}

Solution<KnapsackItem> Knapsack01Problem::optimal_solution(
    const KnapsackCandidate& candidate
) const {
    validate_candidate(candidate);

    Solution<KnapsackItem> solution;
    solution.trace.algorithm_name = "Exact DP 0/1 Knapsack Oracle";
    solution.trace.strategy_name = "Dynamic Programming Recurrence dp[i][w] = max(dp[i-1][w], val + dp[i-1][w-wt])";

    const size_t n = candidate.items.size();
    const int W = candidate.capacity;

    // dp[i][w] is max value using a subset of first i items with capacity limit w
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(W + 1, 0));

    for (size_t i = 1; i <= n; ++i) {
        const int val = candidate.items[i - 1].value;
        const int wt = candidate.items[i - 1].weight;
        for (int w = 0; w <= W; ++w) {
            dp[i][w] = dp[i - 1][w];
            if (wt <= w) {
                dp[i][w] = std::max(dp[i][w], dp[i - 1][w - wt] + val);
            }
        }
    }

    // Reconstruct optimal subset
    int cur_w = W;
    std::vector<KnapsackItem> optimal_items;
    for (int i = static_cast<int>(n); i >= 1; --i) {
        if (dp[i][cur_w] != dp[i - 1][cur_w]) {
            optimal_items.push_back(candidate.items[i - 1]);
            cur_w -= candidate.items[i - 1].weight;
        }
    }

    std::reverse(optimal_items.begin(), optimal_items.end());
    solution.items = optimal_items;
    solution.success = true;
    solution.cost = static_cast<double>(dp[n][W]);
    solution.trace.success = true;
    solution.trace.final_cost = solution.cost;

    for (const auto& it : solution.items) {
        solution.trace.final_solution.push_back(it.id);
    }

    TraceStep summary_step;
    summary_step.step_number = 1;
    summary_step.state_description = "DP table computed for " + std::to_string(n) + " items and capacity " + std::to_string(W);
    summary_step.outcome_description = "Optimal knapsack value: " + std::to_string(dp[n][W]);
    summary_step.accumulated_count = dp[n][W];
    summary_step.rationale = "Exact dynamic programming recurrence solved with O(nW) pseudo-polynomial complexity.";
    solution.trace.steps.push_back(summary_step);

    return solution;
}

DivergenceResult Knapsack01Problem::analyze_divergence(
    const KnapsackCandidate& candidate,
    const Solution<KnapsackItem>& greedy,
    const Solution<KnapsackItem>& optimal
) const {
    DivergenceResult result;
    const double g_val = greedy.cost.value_or(0.0);
    const double opt_val = optimal.cost.value_or(0.0);

    if (g_val >= opt_val) {
        result.has_diverged = false;
        result.explanation = "Greedy ratio heuristic matched optimal value of " +
                             std::to_string(static_cast<int>(opt_val)) + ". No divergence detected.";
        return result;
    }

    result.has_diverged = true;

    // Check optimal subset item IDs
    std::unordered_set<int> opt_set;
    for (const auto& it : optimal.items) {
        opt_set.insert(it.id);
    }

    // Inspect items in greedy consideration order
    std::vector<KnapsackItem> sorted_items = candidate.items;
    std::sort(sorted_items.begin(), sorted_items.end(), knapsack_item_ratio_comparator);

    int remaining_capacity = candidate.capacity;
    int step_idx = 1;

    for (const auto& item : sorted_items) {
        bool greedy_chose = (item.weight <= remaining_capacity);
        bool optimal_chose = (opt_set.find(item.id) != opt_set.end());

        if (greedy_chose && !optimal_chose) {
            result.divergence_step = step_idx;
            result.state_remaining = remaining_capacity;
            result.greedy_choice = item.id;
            for (const auto& opt_it : optimal.items) {
                result.optimal_choices.push_back(opt_it.id);
            }
            result.greedy_projected_cost = g_val;
            result.optimal_projected_cost = opt_val;

            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2);
            oss << "Divergence at Step " << step_idx << ": Greedy selected Item " << item.id
                << " (val=" << item.value << ", wt=" << item.weight << ", ratio=" << item.ratio()
                << "), consuming capacity that blocked the higher-value optimal combination.";
            result.explanation = oss.str();
            return result;
        }

        if (greedy_chose) {
            remaining_capacity -= item.weight;
        }
        step_idx++;
    }

    // Fallback if not caught above
    result.divergence_step = 1;
    result.explanation = "Greedy heuristic diverged from optimal subset: greedy value " +
                         std::to_string(static_cast<int>(g_val)) + " vs optimal " +
                         std::to_string(static_cast<int>(opt_val)) + ".";
    return result;
}

ComplexityReport Knapsack01Problem::complexity_info() const {
    ComplexityReport report;
    report.problem_name = "0/1 Knapsack";
    report.solvers = {
        ComplexityEntry{
            "Greedy (Value/Weight Ratio Descending)",
            "O(n log n)",
            "O(n log n)",
            "O(n log n)",
            "O(n)",
            "Sorting n items by value/weight ratio takes O(n log n). A single linear pass of O(n) "
            "selects items that fit. Space is O(n) to store items and execution trace."
        },
        ComplexityEntry{
            "Dynamic Programming (Exact Optimal Oracle)",
            "O(n * W)",
            "O(n * W)",
            "O(n * W)",
            "O(n * W)",
            "Classic pseudo-polynomial dynamic programming recurrence dp[i][w] = max(dp[i-1][w], val + dp[i-1][w-wt]). "
            "Requires filling an (n+1) x (W+1) table in Theta(nW) time and O(nW) space for optimal subset reconstruction."
        }
    };
    return report;
}

// ============================================================================
// FRACTIONAL KNAPSACK PROBLEM
// ============================================================================

std::string FractionalKnapsackProblem::name() const {
    return "Fractional Knapsack";
}

ProblemObjective FractionalKnapsackProblem::objective() const {
    return ProblemObjective::Maximize;
}

std::string FractionalKnapsackProblem::default_strategy() const {
    return STRATEGY_FRACTIONAL_RATIO;
}

std::vector<std::string> FractionalKnapsackProblem::available_strategies() const {
    return {STRATEGY_FRACTIONAL_RATIO};
}

void FractionalKnapsackProblem::validate_candidate(const KnapsackCandidate& candidate) const {
    if (candidate.items.empty()) {
        throw InvalidCandidateException("Fractional Knapsack items list cannot be empty.");
    }
    if (candidate.capacity <= 0) {
        throw InvalidCandidateException("Knapsack capacity must be strictly positive: found capacity=" +
                                        std::to_string(candidate.capacity));
    }

    std::unordered_set<int> seen_ids;
    for (const auto& item : candidate.items) {
        if (item.weight <= 0) {
            throw InvalidCandidateException("Item weight must be strictly positive: item ID " +
                                            std::to_string(item.id) + " has weight " +
                                            std::to_string(item.weight));
        }
        if (item.value < 0) {
            throw InvalidCandidateException("Item value cannot be negative: item ID " +
                                            std::to_string(item.id) + " has value " +
                                            std::to_string(item.value));
        }
        if (!seen_ids.insert(item.id).second) {
            throw InvalidCandidateException("Duplicate item ID detected: " + std::to_string(item.id));
        }
    }
}

Solution<FractionalKnapsackItem> FractionalKnapsackProblem::greedy_solution(
    const KnapsackCandidate& candidate,
    const std::string& strategy
) const {
    validate_candidate(candidate);

    Solution<FractionalKnapsackItem> solution;
    solution.trace.algorithm_name = "Greedy Fractional Knapsack";
    solution.trace.strategy_name = strategy.empty() ? STRATEGY_FRACTIONAL_RATIO : strategy;

    std::vector<KnapsackItem> sorted = candidate.items;
    std::sort(sorted.begin(), sorted.end(), knapsack_item_ratio_comparator);

    double remaining_cap = static_cast<double>(candidate.capacity);
    double total_value = 0.0;
    int step_num = 1;

    for (const auto& item : sorted) {
        if (remaining_cap <= 1e-9) {
            break;
        }

        TraceStep step;
        step.step_number = step_num++;
        
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Evaluating Item " << item.id << " (v=" << item.value << ", w=" << item.weight
           << ", ratio=" << item.ratio() << ")";
        step.state_description = ss.str();
        step.remaining_target = static_cast<int>(remaining_cap);
        step.available_choices = {item.id};

        FractionalKnapsackItem frac_item;
        frac_item.id = item.id;
        frac_item.value = item.value;
        frac_item.weight = item.weight;

        if (static_cast<double>(item.weight) <= remaining_cap) {
            frac_item.fraction = 1.0;
            frac_item.value_taken = static_cast<double>(item.value);
            frac_item.weight_taken = static_cast<double>(item.weight);

            remaining_cap -= frac_item.weight_taken;
            total_value += frac_item.value_taken;

            step.selected_choice = item.id;
            step.outcome_description = "TAKEN 100%: Item " + std::to_string(item.id);
            step.remaining_after = static_cast<int>(remaining_cap);
            step.accumulated_count = static_cast<int>(total_value);
            step.rationale = "Whole item taken at highest available ratio.";
            solution.items.push_back(frac_item);
        } else {
            frac_item.fraction = remaining_cap / static_cast<double>(item.weight);
            frac_item.value_taken = static_cast<double>(item.value) * frac_item.fraction;
            frac_item.weight_taken = remaining_cap;

            total_value += frac_item.value_taken;
            remaining_cap = 0.0;

            step.selected_choice = item.id;
            std::ostringstream frac_ss;
            frac_ss << std::fixed << std::setprecision(2);
            frac_ss << "TAKEN FRACTION " << (frac_item.fraction * 100.0) << "%: Item " << item.id;
            step.outcome_description = frac_ss.str();
            step.remaining_after = 0;
            step.accumulated_count = static_cast<int>(total_value);
            step.rationale = "Fractional portion taken to exactly saturate remaining capacity.";
            solution.items.push_back(frac_item);
        }

        solution.trace.steps.push_back(step);
    }

    solution.success = true;
    solution.cost = total_value;
    solution.trace.success = true;
    solution.trace.final_cost = total_value;
    for (const auto& it : solution.items) {
        solution.trace.final_solution.push_back(it.id);
    }

    return solution;
}

Solution<FractionalKnapsackItem> FractionalKnapsackProblem::optimal_solution(
    const KnapsackCandidate& candidate
) const {
    auto sol = greedy_solution(candidate, STRATEGY_FRACTIONAL_RATIO);
    sol.trace.algorithm_name = "Exact Fractional Knapsack Oracle";
    sol.trace.strategy_name = "Continuous Relaxation Global Optimum";
    return sol;
}

DivergenceResult FractionalKnapsackProblem::analyze_divergence(
    const KnapsackCandidate& candidate,
    const Solution<FractionalKnapsackItem>& greedy,
    const Solution<FractionalKnapsackItem>& optimal
) const {
    (void)candidate;
    (void)greedy;
    (void)optimal;
    DivergenceResult result;
    result.has_diverged = false;
    result.explanation = "Fractional Knapsack ratio greedy is provably optimal. No divergence exists.";
    return result;
}

ComplexityReport FractionalKnapsackProblem::complexity_info() const {
    ComplexityReport report;
    report.problem_name = "Fractional Knapsack";
    report.solvers = {
        ComplexityEntry{
            "Greedy (Continuous Value/Weight Ratio Descending)",
            "O(n log n)",
            "O(n log n)",
            "O(n log n)",
            "O(n)",
            "Sorting n items by ratio takes O(n log n). Continuous greedy pass takes O(n), taking items whole "
            "or as a single fraction. Provably optimal via the greedy-choice property."
        },
        ComplexityEntry{
            "Exact Verification Oracle",
            "O(n log n)",
            "O(n log n)",
            "O(n log n)",
            "O(n)",
            "Continuous relaxation global optimum coincides with the ratio-greedy schedule. Verifies 0 gap."
        }
    };
    return report;
}

} // namespace greedybreaker
