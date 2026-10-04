#pragma once

#include <string>

namespace greedybreaker {

/**
 * @brief Registry of supported DAA greedy optimization problems.
 */
enum class ProblemType {
    COIN_CHANGE,          ///< Minimum coin change (Minimization)
    ACTIVITY_SELECTION,   ///< Maximum non-overlapping activities (Maximization)
    KNAPSACK_01,          ///< 0/1 Knapsack with value/weight ratio heuristic (Maximization)
    KNAPSACK_FRACTIONAL   ///< Fractional Knapsack with value/weight ratio heuristic (Maximization)
};

inline std::string to_string(ProblemType pt) {
    switch (pt) {
        case ProblemType::COIN_CHANGE: return "Coin Change";
        case ProblemType::ACTIVITY_SELECTION: return "Activity Selection";
        case ProblemType::KNAPSACK_01: return "0/1 Knapsack";
        case ProblemType::KNAPSACK_FRACTIONAL: return "Fractional Knapsack";
    }
    return "UNKNOWN";
}

inline std::string to_slug(ProblemType pt) {
    switch (pt) {
        case ProblemType::COIN_CHANGE: return "coin-change";
        case ProblemType::ACTIVITY_SELECTION: return "activity-selection";
        case ProblemType::KNAPSACK_01: return "knapsack-01";
        case ProblemType::KNAPSACK_FRACTIONAL: return "knapsack-fractional";
    }
    return "unknown";
}

inline ProblemType parse_problem_type(const std::string& str) {
    if (str == "coin-change" || str == "coin_change" || str == "coin") {
        return ProblemType::COIN_CHANGE;
    }
    if (str == "activity-selection" || str == "activity_selection" || str == "activity") {
        return ProblemType::ACTIVITY_SELECTION;
    }
    if (str == "knapsack-01" || str == "knapsack_01" || str == "01knapsack" || str == "knapsack") {
        return ProblemType::KNAPSACK_01;
    }
    if (str == "knapsack-fractional" || str == "knapsack_fractional" || str == "fractional") {
        return ProblemType::KNAPSACK_FRACTIONAL;
    }
    return ProblemType::COIN_CHANGE;
}

} // namespace greedybreaker
