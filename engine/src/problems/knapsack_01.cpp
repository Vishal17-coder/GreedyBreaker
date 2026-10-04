#include "greedybreaker/problems/knapsack_01.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <sstream>

namespace greedybreaker::problems {

void to_json(nlohmann::json& j, const KnapsackItem& item) {
    j = nlohmann::json{
        {"id", item.id},
        {"value", item.value},
        {"weight", item.weight}
    };
}

void from_json(const nlohmann::json& j, KnapsackItem& item) {
    item.id = j.value("id", int64_t{0});
    item.value = j.value("value", 0.0);
    item.weight = j.value("weight", 0.0);
}

void to_json(nlohmann::json& j, const Knapsack01Instance& inst) {
    j = nlohmann::json{
        {"capacity", inst.capacity},
        {"items", inst.items}
    };
}

void from_json(const nlohmann::json& j, Knapsack01Instance& inst) {
    inst.capacity = j.value("capacity", 0.0);
    if (j.contains("items")) {
        inst.items = j.at("items").get<std::vector<KnapsackItem>>();
    }
}

bool Knapsack01Problem::is_valid_instance(const Knapsack01Instance& inst, std::string* error_msg) noexcept {
    if (inst.capacity <= 0.0) {
        if (error_msg) *error_msg = "Capacity must be strictly positive.";
        return false;
    }
    if (inst.items.empty()) {
        if (error_msg) *error_msg = "Knapsack items list cannot be empty.";
        return false;
    }
    for (const auto& itm : inst.items) {
        if (itm.value <= 0.0) {
            if (error_msg) *error_msg = "Item values must be strictly positive.";
            return false;
        }
        if (itm.weight <= 0.0) {
            if (error_msg) *error_msg = "Item weights must be strictly positive.";
            return false;
        }
    }
    return true;
}

core::GreedyResult Knapsack01Problem::solve_greedy(
    const Knapsack01Instance& inst,
    const std::string& strategy)
{
    const auto start_time = std::chrono::steady_clock::now();
    core::GreedyResult result;

    if (!is_valid_instance(inst)) {
        result.feasible = false;
        result.objective_value = 0.0;
        result.solution_description = "Invalid Knapsack instance.";
        return result;
    }

    auto sorted_items = inst.items;
    std::string rule_name = "Value density descending (v_i / w_i)";
    if (strategy == "max_value_first") {
        rule_name = "Max value first";
        std::sort(sorted_items.begin(), sorted_items.end(), [](const KnapsackItem& a, const KnapsackItem& b) {
            if (a.value != b.value) return a.value > b.value;
            return a.id < b.id;
        });
    } else if (strategy == "min_weight_first") {
        rule_name = "Min weight first";
        std::sort(sorted_items.begin(), sorted_items.end(), [](const KnapsackItem& a, const KnapsackItem& b) {
            if (a.weight != b.weight) return a.weight < b.weight;
            return a.id < b.id;
        });
    } else {
        // Default: value density descending
        std::sort(sorted_items.begin(), sorted_items.end(), [](const KnapsackItem& a, const KnapsackItem& b) {
            double da = a.density();
            double db = b.density();
            if (std::abs(da - db) > 1e-9) return da > db;
            return a.id < b.id;
        });
    }

    double rem_capacity = inst.capacity;
    double total_val = 0.0;
    std::vector<int64_t> selected_ids;

    for (size_t i = 0; i < sorted_items.size(); ++i) {
        const auto& itm = sorted_items[i];
        core::DecisionStep step;
        step.step_index = i;
        step.state_before = "Remaining capacity: " + std::to_string(rem_capacity);
        step.heuristic_rule = rule_name;

        core::CandidateChoice choice;
        choice.identifier = "Item " + std::to_string(itm.id) + " (v=" + std::to_string(itm.value) + ", w=" + std::to_string(itm.weight) + ")";
        choice.heuristic_score = itm.density();
        choice.feasible = (itm.weight <= rem_capacity);
        step.candidates_considered.push_back(choice);

        if (itm.weight <= rem_capacity) {
            rem_capacity -= itm.weight;
            total_val += itm.value;
            selected_ids.push_back(itm.id);
            step.action_description = "Selected " + choice.identifier;
            choice.notes = "Fits in knapsack; included";
        } else {
            step.action_description = "Skipped " + choice.identifier + " (exceeds capacity)";
            choice.notes = "Exceeds remaining capacity; rejected";
        }

        step.state_after = "Remaining capacity: " + std::to_string(rem_capacity);
        step.cumulative_objective = total_val;
        result.trace.add_step(step);
    }

    const auto end_time = std::chrono::steady_clock::now();
    result.metrics.execution_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    result.metrics.states_evaluated = sorted_items.size();

    result.feasible = true;
    result.objective_value = total_val;
    result.items_selected = selected_ids;

    std::ostringstream oss;
    oss << "Total value: " << total_val << ", used capacity: " << (inst.capacity - rem_capacity) << "/" << inst.capacity;
    result.solution_description = oss.str();

    return result;
}

core::OracleResult Knapsack01Problem::solve_exact_dp(const Knapsack01Instance& inst) {
    const auto start_time = std::chrono::steady_clock::now();
    core::OracleResult result;
    result.confidence = core::OracleConfidence::EXACT;
    result.oracle_method = "Dynamic Programming 2D Table O(N*W)";

    if (!is_valid_instance(inst)) {
        result.optimal_value = 0.0;
        result.solution_description = "Invalid Knapsack instance.";
        return result;
    }

    const size_t N = inst.items.size();
    const int64_t W = static_cast<int64_t>(std::round(inst.capacity));

    // DP table: (N+1) x (W+1)
    std::vector<std::vector<double>> dp(N + 1, std::vector<double>(static_cast<size_t>(W + 1), 0.0));
    uint64_t states_count = 0;

    for (size_t i = 1; i <= N; ++i) {
        const auto& itm = inst.items[i - 1];
        int64_t itm_w = static_cast<int64_t>(std::round(itm.weight));
        for (int64_t w = 0; w <= W; ++w) {
            ++states_count;
            dp[i][static_cast<size_t>(w)] = dp[i - 1][static_cast<size_t>(w)];
            if (itm_w <= w) {
                double with_curr = itm.value + dp[i - 1][static_cast<size_t>(w - itm_w)];
                if (with_curr > dp[i][static_cast<size_t>(w)]) {
                    dp[i][static_cast<size_t>(w)] = with_curr;
                }
            }
        }
    }

    // Backtrack to find items selected
    std::vector<int64_t> optimal_ids;
    int64_t curr_w = W;
    for (size_t i = N; i > 0; --i) {
        const auto& itm = inst.items[i - 1];
        int64_t itm_w = static_cast<int64_t>(std::round(itm.weight));
        if (curr_w >= itm_w && std::abs(dp[i][static_cast<size_t>(curr_w)] - (itm.value + dp[i - 1][static_cast<size_t>(curr_w - itm_w)])) < 1e-9) {
            optimal_ids.push_back(itm.id);
            curr_w -= itm_w;
        }
    }
    std::reverse(optimal_ids.begin(), optimal_ids.end());

    const auto end_time = std::chrono::steady_clock::now();
    result.metrics.execution_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    result.metrics.states_evaluated = states_count;

    result.optimal_value = dp[N][static_cast<size_t>(W)];
    result.items_selected = optimal_ids;

    std::ostringstream oss;
    oss << "Exact optimum value: " << result.optimal_value << " using " << optimal_ids.size() << " items";
    result.solution_description = oss.str();

    return result;
}

core::OracleResult Knapsack01Problem::solve_exact_exhaustive(const Knapsack01Instance& inst) {
    const auto start_time = std::chrono::steady_clock::now();
    core::OracleResult result;
    result.confidence = core::OracleConfidence::EXACT;
    result.oracle_method = "Exhaustive Subset Search O(2^N)";

    if (!is_valid_instance(inst)) {
        result.optimal_value = 0.0;
        result.solution_description = "Invalid Knapsack instance.";
        return result;
    }

    const size_t N = inst.items.size();
    if (N > 25) {
        // Fall back to DP if N too large for 2^N
        return solve_exact_dp(inst);
    }

    const uint64_t total_subsets = 1ULL << N;
    double best_value = 0.0;
    uint64_t best_mask = 0;
    uint64_t states_count = 0;

    for (uint64_t mask = 0; mask < total_subsets; ++mask) {
        ++states_count;
        double current_weight = 0.0;
        double current_value = 0.0;
        for (size_t i = 0; i < N; ++i) {
            if (mask & (1ULL << i)) {
                current_weight += inst.items[i].weight;
                current_value += inst.items[i].value;
            }
        }
        if (current_weight <= inst.capacity + 1e-9) {
            if (current_value > best_value) {
                best_value = current_value;
                best_mask = mask;
            }
        }
    }

    std::vector<int64_t> optimal_ids;
    for (size_t i = 0; i < N; ++i) {
        if (best_mask & (1ULL << i)) {
            optimal_ids.push_back(inst.items[i].id);
        }
    }

    const auto end_time = std::chrono::steady_clock::now();
    result.metrics.execution_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    result.metrics.states_evaluated = states_count;

    result.optimal_value = best_value;
    result.items_selected = optimal_ids;

    std::ostringstream oss;
    oss << "Exhaustive optimum value: " << best_value << " using " << optimal_ids.size() << " items";
    result.solution_description = oss.str();

    return result;
}

} // namespace greedybreaker::problems
