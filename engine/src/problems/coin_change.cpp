#include "greedybreaker/problems/coin_change.hpp"
#include <algorithm>
#include <unordered_set>
#include <sstream>
#include <limits>

namespace greedybreaker {

namespace {
    constexpr int INF = 1'000'000'000;
}

std::string CoinChangeProblem::name() const {
    return "Coin Change";
}

ProblemObjective CoinChangeProblem::objective() const {
    return ProblemObjective::Minimize;
}

std::string CoinChangeProblem::default_strategy() const {
    return STRATEGY_LARGEST_FIRST;
}

std::vector<std::string> CoinChangeProblem::available_strategies() const {
    return {STRATEGY_LARGEST_FIRST};
}

void CoinChangeProblem::validate_candidate(const CoinChangeCandidate& candidate) const {
    if (candidate.denominations.empty()) {
        throw InvalidCandidateException("Denominations list must not be empty.");
    }

    if (candidate.target <= 0) {
        throw InvalidCandidateException(
            "Target must be a strictly positive integer (> 0); received: " +
            std::to_string(candidate.target)
        );
    }

    std::unordered_set<int> seen;
    for (int coin : candidate.denominations) {
        if (coin <= 0) {
            throw InvalidCandidateException(
                "All denominations must be strictly positive integers (> 0); received: " +
                std::to_string(coin)
            );
        }
        if (seen.find(coin) != seen.end()) {
            throw InvalidCandidateException(
                "Denominations must contain unique values; duplicate found: " +
                std::to_string(coin)
            );
        }
        seen.insert(coin);
    }
}

Solution<int> CoinChangeProblem::greedy_solution(
    const CoinChangeCandidate& candidate,
    const std::string& strategy
) const {
    validate_candidate(candidate);

    const std::string strat = strategy.empty() ? default_strategy() : strategy;
    if (strat != STRATEGY_LARGEST_FIRST) {
        throw std::invalid_argument("Unknown greedy strategy: " + strat);
    }

    std::vector<int> sorted_coins = candidate.denominations;
    std::sort(sorted_coins.rbegin(), sorted_coins.rend()); // Descending

    int remaining = candidate.target;
    std::vector<int> selected_coins;
    std::vector<TraceStep> steps;

    int step_idx = 1;
    while (remaining > 0) {
        std::vector<int> available;
        for (int c : sorted_coins) {
            if (c <= remaining) {
                available.push_back(c);
            }
        }

        if (available.empty()) {
            // Dead end
            TraceStep dead_step;
            dead_step.step_number = step_idx;
            dead_step.state_description = "Remaining target: " + std::to_string(remaining);
            dead_step.remaining_target = remaining;
            dead_step.available_choices = {};
            dead_step.selected_choice = std::nullopt;
            dead_step.outcome_description = "DEAD END: No available denomination is <= " +
                std::to_string(remaining) + ". Greedy heuristic failed to make exact change.";
            dead_step.remaining_after = remaining;
            dead_step.accumulated_count = static_cast<int>(selected_coins.size());
            dead_step.rationale = "Greedy cannot proceed because no denomination fits into remaining balance.";
            steps.push_back(dead_step);

            AlgorithmTrace trace;
            trace.algorithm_name = "Greedy";
            trace.strategy_name = "Largest Denomination First";
            trace.success = false;
            trace.steps = steps;
            trace.final_solution = selected_coins;
            trace.final_cost = std::nullopt;
            trace.execution_notes = {
                "Greedy terminated prematurely at remaining target " + std::to_string(remaining)
            };

            Solution<int> sol;
            sol.success = false;
            sol.cost = std::nullopt;
            sol.items = selected_coins;
            sol.trace = trace;
            return sol;
        }

        int chosen = available.front();
        int new_remaining = remaining - chosen;
        selected_coins.push_back(chosen);

        TraceStep step;
        step.step_number = step_idx;
        step.state_description = "Remaining target: " + std::to_string(remaining);
        step.remaining_target = remaining;
        step.available_choices = available;
        step.selected_choice = chosen;
        step.outcome_description = "Selected coin " + std::to_string(chosen) +
            ". Remaining target reduced from " + std::to_string(remaining) +
            " to " + std::to_string(new_remaining) + ".";
        step.remaining_after = new_remaining;
        step.accumulated_count = static_cast<int>(selected_coins.size());
        step.rationale = "Locally optimal greedy choice: denomination " + std::to_string(chosen) +
            " is the largest valid coin not exceeding remaining target " + std::to_string(remaining) + ".";
        steps.push_back(step);

        remaining = new_remaining;
        ++step_idx;
    }

    AlgorithmTrace trace;
    trace.algorithm_name = "Greedy";
    trace.strategy_name = "Largest Denomination First";
    trace.success = true;
    trace.steps = steps;
    trace.final_solution = selected_coins;
    trace.final_cost = static_cast<double>(selected_coins.size());
    trace.execution_notes = {
        "Greedy successfully formed target " + std::to_string(candidate.target) +
        " using " + std::to_string(selected_coins.size()) + " coins."
    };

    Solution<int> sol;
    sol.success = true;
    sol.cost = static_cast<double>(selected_coins.size());
    sol.items = selected_coins;
    sol.trace = trace;
    return sol;
}

Solution<int> CoinChangeProblem::optimal_solution(const CoinChangeCandidate& candidate) const {
    validate_candidate(candidate);

    const int target = candidate.target;
    std::vector<int> sorted_coins = candidate.denominations;
    std::sort(sorted_coins.begin(), sorted_coins.end()); // Ascending for DP

    std::vector<int> dp(target + 1, INF);
    dp[0] = 0;
    std::vector<int> parent(target + 1, -1);
    std::vector<std::vector<int>> optimal_choices_at(target + 1);

    for (int v = 1; v <= target; ++v) {
        for (int c : sorted_coins) {
            if (c <= v && dp[v - c] != INF) {
                int cand = dp[v - c] + 1;
                if (cand < dp[v]) {
                    dp[v] = cand;
                    parent[v] = c;
                    optimal_choices_at[v].clear();
                    optimal_choices_at[v].push_back(c);
                } else if (cand == dp[v]) {
                    optimal_choices_at[v].push_back(c);
                }
            }
        }
    }

    if (dp[target] == INF) {
        AlgorithmTrace trace;
        trace.algorithm_name = "Dynamic Programming";
        trace.strategy_name = "Exact Optimal Oracle";
        trace.success = false;
        trace.execution_notes = {
            "DP oracle verified that target " + std::to_string(target) +
            " cannot be formed with the given denominations."
        };

        Solution<int> sol;
        sol.success = false;
        sol.cost = std::nullopt;
        sol.items = {};
        sol.trace = trace;
        return sol;
    }

    std::vector<int> optimal_coins;
    std::vector<TraceStep> steps;
    int curr = target;
    int step_idx = 1;

    while (curr > 0) {
        int c = parent[curr];
        int prev = curr;
        curr -= c;
        optimal_coins.push_back(c);

        std::vector<int> available;
        for (int coin : sorted_coins) {
            if (coin <= prev) available.push_back(coin);
        }

        TraceStep step;
        step.step_number = step_idx;
        step.state_description = "Remaining target: " + std::to_string(prev);
        step.remaining_target = prev;
        step.available_choices = available;
        step.selected_choice = c;
        step.outcome_description = "Selected optimal coin " + std::to_string(c) +
            ". Remaining target reduced from " + std::to_string(prev) +
            " to " + std::to_string(curr) + ".";
        step.remaining_after = curr;
        step.accumulated_count = static_cast<int>(optimal_coins.size());

        std::ostringstream oss;
        oss << "Optimal choice: Coin " << c << " preserves the global minimum cost path. "
            << "Optimal choices from remaining " << prev << " are {";
        for (size_t i = 0; i < optimal_choices_at[prev].size(); ++i) {
            if (i > 0) oss << ", ";
            oss << optimal_choices_at[prev][i];
        }
        oss << "}.";
        step.rationale = oss.str();

        steps.push_back(step);
        ++step_idx;
    }

    AlgorithmTrace trace;
    trace.algorithm_name = "Dynamic Programming";
    trace.strategy_name = "Exact Optimal Oracle";
    trace.success = true;
    trace.steps = steps;
    trace.final_solution = optimal_coins;
    trace.final_cost = static_cast<double>(dp[target]);
    trace.execution_notes = {
        "DP oracle verified global minimum of " + std::to_string(dp[target]) +
        " coins for target " + std::to_string(target) + "."
    };

    Solution<int> sol;
    sol.success = true;
    sol.cost = static_cast<double>(dp[target]);
    sol.items = optimal_coins;
    sol.trace = trace;
    return sol;
}

DivergenceResult CoinChangeProblem::analyze_divergence(
    const CoinChangeCandidate& candidate,
    const Solution<int>& greedy,
    const Solution<int>& optimal
) const {
    DivergenceResult div;

    if (!optimal.success) {
        div.has_diverged = false;
        div.explanation = "No optimal solution exists for this candidate instance. Divergence is not applicable.";
        return div;
    }

    const int target = candidate.target;
    std::vector<int> sorted_coins = candidate.denominations;
    std::sort(sorted_coins.begin(), sorted_coins.end());

    std::vector<int> dp(target + 1, INF);
    dp[0] = 0;
    std::vector<std::vector<int>> optimal_choices_at(target + 1);

    for (int v = 1; v <= target; ++v) {
        for (int c : sorted_coins) {
            if (c <= v && dp[v - c] != INF) {
                int cand = dp[v - c] + 1;
                if (cand < dp[v]) {
                    dp[v] = cand;
                    optimal_choices_at[v].clear();
                    optimal_choices_at[v].push_back(c);
                } else if (cand == dp[v]) {
                    optimal_choices_at[v].push_back(c);
                }
            }
        }
    }

    // Case: Greedy failed while optimal succeeded (dead end)
    if (!greedy.success) {
        for (const auto& step : greedy.trace.steps) {
            int rem = step.remaining_target;
            if (rem > 0 && rem <= target) {
                const auto& valid_opts = optimal_choices_at[rem];
                if (!step.selected_choice.has_value() ||
                    std::find(valid_opts.begin(), valid_opts.end(), step.selected_choice.value()) == valid_opts.end()) {
                    div.has_diverged = true;
                    div.divergence_step = step.step_number;
                    div.state_remaining = rem;
                    div.greedy_choice = step.selected_choice;
                    div.optimal_choices = valid_opts;
                    div.greedy_projected_cost = std::numeric_limits<double>::infinity();
                    div.optimal_projected_cost = static_cast<double>(dp[rem]);

                    std::ostringstream oss;
                    oss << "CRITICAL DIVERGENCE AT STEP " << step.step_number << ": At remaining target " << rem
                        << ", greedy selected " << (step.selected_choice.has_value() ? std::to_string(step.selected_choice.value()) : "none")
                        << ", which led to an unsolvable dead end. The exact solver demonstrates that choosing from {";
                    for (size_t i = 0; i < valid_opts.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << valid_opts[i];
                    }
                    oss << "} leads to an optimal solution of " << dp[rem] << " coins.";
                    div.explanation = oss.str();
                    return div;
                }
            }
        }
    }

    // Case: Both succeeded and matched optimal cost
    if (greedy.success && optimal.success && std::abs(greedy.cost.value() - optimal.cost.value()) < 1e-9) {
        div.has_diverged = false;
        div.greedy_projected_cost = greedy.cost;
        div.optimal_projected_cost = optimal.cost;
        div.explanation = "Greedy matched the optimal cost (" +
            std::to_string(static_cast<int>(optimal.cost.value())) +
            " coins). All greedy decisions remained within the optimal decision manifold.";
        return div;
    }

    // Case: Greedy is suboptimal - find the earliest step where chosen coin is not in optimal_choices_at
    for (const auto& step : greedy.trace.steps) {
        int rem = step.remaining_target;
        if (rem <= 0 || rem > target || !step.selected_choice.has_value()) {
            continue;
        }

        int chosen = step.selected_choice.value();
        const auto& valid_opts = optimal_choices_at[rem];

        if (std::find(valid_opts.begin(), valid_opts.end(), chosen) == valid_opts.end()) {
            // First divergence isolated!
            int prev_coins = step.step_number - 1;
            int new_rem = rem - chosen;
            double greedy_from_here = 1.0 + (new_rem >= 0 && dp[new_rem] != INF ? dp[new_rem] : INF);
            double opt_from_here = dp[rem];

            double greedy_proj = prev_coins + greedy_from_here;
            double opt_proj = prev_coins + opt_from_here;

            div.has_diverged = true;
            div.divergence_step = step.step_number;
            div.state_remaining = rem;
            div.greedy_choice = chosen;
            div.optimal_choices = valid_opts;
            div.greedy_projected_cost = greedy_proj;
            div.optimal_projected_cost = opt_proj;

            std::ostringstream oss;
            oss << "FIRST DIVERGENCE AT STEP " << step.step_number << ": At remaining target " << rem
                << ", greedy selected denomination " << chosen << " (the locally largest valid coin). "
                << "However, selecting " << chosen << " leaves remaining target " << new_rem << ", which requires "
                << (new_rem >= 0 && dp[new_rem] != INF ? std::to_string(dp[new_rem]) : "infinite")
                << " additional coins (projected total " << static_cast<int>(greedy_proj) << " coins). "
                << "In contrast, choosing an optimal coin from {";
            for (size_t i = 0; i < valid_opts.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << valid_opts[i];
            }
            oss << "} requires only " << static_cast<int>(opt_from_here) << " coins in total from this state "
                << "(global total " << static_cast<int>(opt_proj) << " coins). "
                << "This greedy decision irreversibly forced an extra "
                << static_cast<int>(greedy_proj - opt_proj) << " coin(s).";
            div.explanation = oss.str();
            return div;
        }
    }

    // Fallback if loop finishes but costs differ
    div.has_diverged = true;
    div.divergence_step = 1;
    div.state_remaining = candidate.target;
    div.greedy_choice = greedy.items.empty() ? std::nullopt : std::optional<int>(greedy.items.front());
    div.optimal_choices = optimal.items.empty() ? std::vector<int>{} : std::vector<int>{optimal.items.front()};
    div.greedy_projected_cost = greedy.cost;
    div.optimal_projected_cost = optimal.cost;
    div.explanation = "Suboptimality detected in greedy trajectory.";
    return div;
}

ComplexityReport CoinChangeProblem::complexity_info() const {
    ComplexityReport report;
    report.problem_name = "Coin Change";
    report.solvers = {
        ComplexityEntry{
            "Greedy (Largest Denomination First)",
            "O(1)",
            "O(K + T / c_max)",
            "O(K log K + T / c_min)",
            "O(T / c_min)",
            "Sorting K denominations takes O(K log K). In the worst case, greedy repeatedly "
            "selects the smallest coin c_min, performing O(T / c_min) steps. Auxiliary space "
            "is O(1) for computation, or O(T / c_min) to record the execution trace."
        },
        ComplexityEntry{
            "Dynamic Programming (Exact Optimal Oracle)",
            "O(K * T)",
            "O(K * T)",
            "O(K * T)",
            "O(T)",
            "Iterates through all sub-targets from 1 to T, evaluating all K denominations at each "
            "state. The DP table stores optimal costs for T states, resulting in Theta(K * T) time "
            "and O(T) space. Solution reconstruction takes O(optimal_coins) <= O(T) steps."
        }
    };
    return report;
}

} // namespace greedybreaker
