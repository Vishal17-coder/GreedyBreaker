#include "greedybreaker/problems/fractional_knapsack.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <sstream>

namespace greedybreaker::problems {

void to_json(nlohmann::json& j, const FractionalKnapsackInstance& inst) {
    j = nlohmann::json{
        {"capacity", inst.capacity},
        {"items", inst.items}
    };
}

void from_json(const nlohmann::json& j, FractionalKnapsackInstance& inst) {
    inst.capacity = j.value("capacity", 0.0);
    if (j.contains("items")) {
        inst.items = j.at("items").get<std::vector<KnapsackItem>>();
    }
}

bool FractionalKnapsackProblem::is_valid_instance(const FractionalKnapsackInstance& inst, std::string* error_msg) noexcept {
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

core::GreedyResult FractionalKnapsackProblem::solve_greedy(
    const FractionalKnapsackInstance& inst,
    const std::string& strategy)
{
    const auto start_time = std::chrono::steady_clock::now();
    core::GreedyResult result;

    if (!is_valid_instance(inst)) {
        result.feasible = false;
        result.objective_value = 0.0;
        result.solution_description = "Invalid Fractional Knapsack instance.";
        return result;
    }

    auto sorted_items = inst.items;
    std::string rule_name = "Value density descending";

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
        // Default: density descending
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
        if (rem_capacity <= 1e-9) break;

        const auto& itm = sorted_items[i];
        core::DecisionStep step;
        step.step_index = i;
        step.state_before = "Remaining capacity: " + std::to_string(rem_capacity);
        step.heuristic_rule = rule_name;

        core::CandidateChoice choice;
        choice.identifier = "Item " + std::to_string(itm.id) + " (v=" + std::to_string(itm.value) + ", w=" + std::to_string(itm.weight) + ")";
        choice.heuristic_score = itm.density();
        choice.feasible = true;
        step.candidates_considered.push_back(choice);

        if (itm.weight <= rem_capacity) {
            // Take 100% of item
            rem_capacity -= itm.weight;
            total_val += itm.value;
            selected_ids.push_back(itm.id);
            step.action_description = "Selected 100% of " + choice.identifier;
        } else {
            // Take fraction of item
            double fraction = rem_capacity / itm.weight;
            total_val += itm.value * fraction;
            selected_ids.push_back(itm.id);
            step.action_description = "Selected " + std::to_string(fraction * 100.0) + "% of " + choice.identifier;
            rem_capacity = 0.0;
        }

        step.state_after = "Remaining capacity: " + std::to_string(rem_capacity);
        step.cumulative_objective = total_val;
        result.trace.add_step(step);
    }

    const auto end_time = std::chrono::steady_clock::now();
    result.metrics.execution_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    result.metrics.states_evaluated = result.trace.size();

    result.feasible = true;
    result.objective_value = total_val;
    result.items_selected = selected_ids;

    std::ostringstream oss;
    oss << "Total fractional value: " << total_val << ", used capacity: " << (inst.capacity - rem_capacity) << "/" << inst.capacity;
    result.solution_description = oss.str();

    return result;
}

core::OracleResult FractionalKnapsackProblem::solve_exact_lp(const FractionalKnapsackInstance& inst) {
    const auto start_time = std::chrono::steady_clock::now();
    core::OracleResult result;
    result.confidence = core::OracleConfidence::EXACT;
    result.oracle_method = "Continuous Linear Relaxation Exact Optimum";

    if (!is_valid_instance(inst)) {
        result.optimal_value = 0.0;
        result.solution_description = "Invalid Fractional Knapsack instance.";
        return result;
    }

    // Exact LP optimum for fractional knapsack is achieved by continuous density greedy
    auto greedy_res = solve_greedy(inst, "value_density_descending");

    const auto end_time = std::chrono::steady_clock::now();
    result.metrics.execution_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    result.metrics.states_evaluated = inst.items.size();

    result.optimal_value = greedy_res.objective_value;
    result.items_selected = greedy_res.items_selected;
    result.solution_description = "Exact LP Relaxation Optimum value: " + std::to_string(greedy_res.objective_value);

    return result;
}

} // namespace greedybreaker::problems
