#include "greedybreaker/problems/coin_change.hpp"
#include <algorithm>
#include <chrono>
#include <limits>
#include <queue>
#include <sstream>

namespace greedybreaker::problems {

void to_json(nlohmann::json& j, const CoinChangeInstance& inst) {
    j = nlohmann::json{
        {"denominations", inst.denominations},
        {"target", inst.target}
    };
}

void from_json(const nlohmann::json& j, CoinChangeInstance& inst) {
    inst.denominations = j.value("denominations", std::vector<int64_t>{});
    inst.target = j.value("target", int64_t{0});
}

bool CoinChangeProblem::is_valid_instance(const CoinChangeInstance& inst, std::string* error_msg) noexcept {
    if (inst.target <= 0) {
        if (error_msg) *error_msg = "Target must be strictly positive.";
        return false;
    }
    if (inst.denominations.empty()) {
        if (error_msg) *error_msg = "Denominations cannot be empty.";
        return false;
    }
    for (size_t i = 0; i < inst.denominations.size(); ++i) {
        if (inst.denominations[i] <= 0) {
            if (error_msg) *error_msg = "Denominations must be strictly positive.";
            return false;
        }
        if (i > 0 && inst.denominations[i] <= inst.denominations[i - 1]) {
            if (error_msg) *error_msg = "Denominations must be strictly sorted ascending without duplicates.";
            return false;
        }
    }
    return true;
}

core::GreedyResult CoinChangeProblem::solve_greedy(
    const CoinChangeInstance& inst,
    const std::string& /*strategy*/)
{
    const auto start_time = std::chrono::steady_clock::now();
    core::GreedyResult result;

    if (!is_valid_instance(inst)) {
        result.feasible = false;
        result.objective_value = std::numeric_limits<double>::infinity();
        result.solution_description = "Invalid Coin Change instance.";
        return result;
    }

    int64_t rem = inst.target;
    size_t step_idx = 0;
    std::vector<int64_t> coins_used;

    while (rem > 0) {
        core::DecisionStep step;
        step.step_index = step_idx++;
        step.state_before = "Remaining target: " + std::to_string(rem);
        step.heuristic_rule = "Pick largest denomination <= remaining target";

        int64_t chosen_coin = -1;
        for (auto it = inst.denominations.rbegin(); it != inst.denominations.rend(); ++it) {
            int64_t coin = *it;
            core::CandidateChoice choice;
            choice.identifier = "Coin " + std::to_string(coin);
            choice.heuristic_score = static_cast<double>(coin);
            choice.feasible = (coin <= rem);
            choice.notes = choice.feasible ? "Fits in remaining target" : "Exceeds remaining target";
            step.candidates_considered.push_back(choice);

            if (chosen_coin == -1 && coin <= rem) {
                chosen_coin = coin;
            }
        }

        if (chosen_coin == -1) {
            // Cannot form exact amount with remaining denominations
            result.feasible = false;
            result.objective_value = std::numeric_limits<double>::infinity();
            result.solution_description = "Greedy solver got stuck: cannot make change for remaining target " + std::to_string(rem);
            step.action_description = "No denomination fits remaining target: FEASIBILITY FAILURE";
            step.state_after = step.state_before;
            result.trace.add_step(step);
            break;
        }

        rem -= chosen_coin;
        coins_used.push_back(chosen_coin);
        step.action_description = "Selected coin " + std::to_string(chosen_coin);
        step.state_after = "Remaining target: " + std::to_string(rem);
        step.cumulative_objective = static_cast<double>(coins_used.size());
        result.trace.add_step(step);
    }

    const auto end_time = std::chrono::steady_clock::now();
    result.metrics.execution_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    result.metrics.states_evaluated = step_idx;

    if (rem == 0) {
        result.feasible = true;
        result.objective_value = static_cast<double>(coins_used.size());
        result.items_selected = coins_used;
        std::ostringstream oss;
        oss << "Used " << coins_used.size() << " coins: [";
        for (size_t i = 0; i < coins_used.size(); ++i) {
            oss << coins_used[i] << (i + 1 < coins_used.size() ? ", " : "");
        }
        oss << "]";
        result.solution_description = oss.str();
    }

    return result;
}

core::OracleResult CoinChangeProblem::solve_exact_dp(const CoinChangeInstance& inst) {
    const auto start_time = std::chrono::steady_clock::now();
    core::OracleResult result;
    result.confidence = core::OracleConfidence::EXACT;
    result.oracle_method = "Dynamic Programming Unbounded Knapsack O(K*T)";

    if (!is_valid_instance(inst)) {
        result.optimal_value = std::numeric_limits<double>::infinity();
        result.solution_description = "Invalid Coin Change instance.";
        return result;
    }

    const int64_t target = inst.target;
    constexpr int64_t INF = std::numeric_limits<int64_t>::max() / 2;
    std::vector<int64_t> dp(static_cast<size_t>(target + 1), INF);
    std::vector<int64_t> parent_coin(static_cast<size_t>(target + 1), -1);

    dp[0] = 0;
    uint64_t states_count = 0;

    for (int64_t t = 1; t <= target; ++t) {
        for (int64_t c : inst.denominations) {
            ++states_count;
            if (c <= t && dp[static_cast<size_t>(t - c)] != INF) {
                int64_t cand = 1 + dp[static_cast<size_t>(t - c)];
                if (cand < dp[static_cast<size_t>(t)]) {
                    dp[static_cast<size_t>(t)] = cand;
                    parent_coin[static_cast<size_t>(t)] = c;
                }
            }
        }
    }

    const auto end_time = std::chrono::steady_clock::now();
    result.metrics.execution_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    result.metrics.states_evaluated = states_count;

    if (dp[static_cast<size_t>(target)] == INF) {
        result.optimal_value = std::numeric_limits<double>::infinity();
        result.solution_description = "Target cannot be formed by denominations.";
    } else {
        result.optimal_value = static_cast<double>(dp[static_cast<size_t>(target)]);
        std::vector<int64_t> coins;
        int64_t curr = target;
        while (curr > 0) {
            int64_t c = parent_coin[static_cast<size_t>(curr)];
            coins.push_back(c);
            curr -= c;
        }
        result.items_selected = coins;
        std::ostringstream oss;
        oss << "Exact optimum: " << coins.size() << " coins: [";
        for (size_t i = 0; i < coins.size(); ++i) {
            oss << coins[i] << (i + 1 < coins.size() ? ", " : "");
        }
        oss << "]";
        result.solution_description = oss.str();
    }

    return result;
}

core::OracleResult CoinChangeProblem::solve_exact_bfs(const CoinChangeInstance& inst) {
    const auto start_time = std::chrono::steady_clock::now();
    core::OracleResult result;
    result.confidence = core::OracleConfidence::EXACT;
    result.oracle_method = "Breadth-First Search Exact Oracle";

    if (!is_valid_instance(inst)) {
        result.optimal_value = std::numeric_limits<double>::infinity();
        result.solution_description = "Invalid Coin Change instance.";
        return result;
    }

    const int64_t target = inst.target;
    std::vector<bool> visited(static_cast<size_t>(target + 1), false);
    std::vector<int64_t> parent_coin(static_cast<size_t>(target + 1), -1);
    std::vector<int64_t> parent_node(static_cast<size_t>(target + 1), -1);

    std::queue<int64_t> q;
    q.push(0);
    visited[0] = true;

    uint64_t states_count = 0;
    bool found = false;

    while (!q.empty()) {
        int64_t curr = q.front();
        q.pop();

        if (curr == target) {
            found = true;
            break;
        }

        for (int64_t c : inst.denominations) {
            ++states_count;
            int64_t next = curr + c;
            if (next <= target && !visited[static_cast<size_t>(next)]) {
                visited[static_cast<size_t>(next)] = true;
                parent_coin[static_cast<size_t>(next)] = c;
                parent_node[static_cast<size_t>(next)] = curr;
                q.push(next);
            }
        }
    }

    const auto end_time = std::chrono::steady_clock::now();
    result.metrics.execution_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    result.metrics.states_evaluated = states_count;

    if (!found) {
        result.optimal_value = std::numeric_limits<double>::infinity();
        result.solution_description = "Target cannot be formed by denominations.";
    } else {
        std::vector<int64_t> coins;
        int64_t curr = target;
        while (curr > 0) {
            int64_t c = parent_coin[static_cast<size_t>(curr)];
            coins.push_back(c);
            curr = parent_node[static_cast<size_t>(curr)];
        }
        result.optimal_value = static_cast<double>(coins.size());
        result.items_selected = coins;
        std::ostringstream oss;
        oss << "BFS optimum: " << coins.size() << " coins: [";
        for (size_t i = 0; i < coins.size(); ++i) {
            oss << coins[i] << (i + 1 < coins.size() ? ", " : "");
        }
        oss << "]";
        result.solution_description = oss.str();
    }

    return result;
}

} // namespace greedybreaker::problems
