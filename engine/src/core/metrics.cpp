#include "greedybreaker/core/metrics.hpp"
#include <cmath>
#include <limits>
#include <sstream>
#include <iomanip>

namespace greedybreaker {

ComparisonResult calculate_comparison(
    ProblemObjective objective,
    bool greedy_solved,
    bool optimal_solved,
    std::optional<double> greedy_cost,
    std::optional<double> optimal_cost
) {
    ComparisonResult result;
    result.objective = objective;
    result.greedy_solved = greedy_solved;
    result.optimal_solved = optimal_solved;
    result.greedy_cost = greedy_cost;
    result.optimal_cost = optimal_cost;

    // Case 1: Both solvers failed (unsolvable instance)
    if (!greedy_solved && !optimal_solved) {
        result.is_counterexample = false;
        result.absolute_gap = 0.0;
        result.ratio = 1.0;
        result.overhead_percentage = 0.0;
        result.interpretation = "Neither algorithm found a feasible solution. Instance is unsolvable.";
        return result;
    }

    // Case 2: Optimal solved, but Greedy hit a dead end
    if (!greedy_solved && optimal_solved) {
        result.is_counterexample = true;
        result.absolute_gap = std::numeric_limits<double>::infinity();
        result.ratio = std::numeric_limits<double>::infinity();
        result.overhead_percentage = std::numeric_limits<double>::infinity();
        
        std::ostringstream oss;
        oss << "CRITICAL FAILURE: Greedy heuristic reached a dead end, whereas the optimal solver found a solution of cost "
            << optimal_cost.value_or(0.0) << ".";
        result.interpretation = oss.str();
        return result;
    }

    // Case 3: Greedy solved, but Optimal says impossible (invalid state)
    if (greedy_solved && !optimal_solved) {
        throw std::runtime_error("Invalid comparison state: Greedy found a solution where optimal solver claims none exists.");
    }

    const double g_val = greedy_cost.value();
    const double opt_val = optimal_cost.value();

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);

    if (objective == ProblemObjective::Minimize) {
        double gap = g_val - opt_val;
        bool is_counter = (gap > 1e-9);
        result.is_counterexample = is_counter;
        result.absolute_gap = gap;

        if (std::abs(opt_val) < 1e-9) {
            result.ratio = (std::abs(g_val) < 1e-9) ? 1.0 : std::numeric_limits<double>::infinity();
            result.overhead_percentage = (std::abs(g_val) < 1e-9) ? 0.0 : std::numeric_limits<double>::infinity();
        } else {
            result.ratio = g_val / opt_val;
            result.overhead_percentage = (gap / opt_val) * 100.0;
        }

        if (is_counter) {
            oss << "COUNTEREXAMPLE CONFIRMED: Greedy used " << g_val << " (optimal: " << opt_val << "). "
                << "Absolute gap: +" << gap << " (" << result.overhead_percentage.value()
                << "% overhead, ratio " << result.ratio.value() << "x).";
        } else {
            oss << "OPTIMAL MATCH: Greedy matched optimal cost of " << opt_val << " (ratio 1.00x).";
        }
    } else { // Maximize
        double gap = opt_val - g_val;
        bool is_counter = (gap > 1e-9);
        result.is_counterexample = is_counter;
        result.absolute_gap = gap;

        if (std::abs(opt_val) < 1e-9) {
            result.ratio = 1.0;
            result.overhead_percentage = 0.0;
        } else {
            result.ratio = (std::abs(g_val) > 1e-9) ? (opt_val / g_val) : std::numeric_limits<double>::infinity();
            result.overhead_percentage = (gap / opt_val) * 100.0;
        }

        if (is_counter) {
            oss << "COUNTEREXAMPLE CONFIRMED: Greedy achieved " << g_val << " (optimal: " << opt_val << "). "
                << "Deficit gap: -" << gap << " (" << result.overhead_percentage.value()
                << "% loss, deficit factor " << result.ratio.value() << "x).";
        } else {
            oss << "OPTIMAL MATCH: Greedy matched maximum optimal value of " << opt_val << ".";
        }
    }

    result.interpretation = oss.str();
    return result;
}

} // namespace greedybreaker
