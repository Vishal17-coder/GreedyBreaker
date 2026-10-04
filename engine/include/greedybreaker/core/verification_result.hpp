#pragma once

#include "greedybreaker/core/solution.hpp"
#include "greedybreaker/core/types.hpp"
#include <cmath>
#include <limits>
#include <string>

namespace greedybreaker::core {

struct VerificationResult {
    VerificationStatus status{VerificationStatus::OPTIMAL};
    bool is_counterexample{false};
    double greedy_value{0.0};
    double optimal_value{0.0};
    double absolute_gap{0.0};
    double relative_gap{1.0};
    double failure_percent{0.0}; // overhead % for MINIMIZE, deficit % for MAXIMIZE
    std::string summary;
};

[[nodiscard]] inline VerificationResult verify(
    ObjectiveDirection direction,
    const GreedyResult& greedy,
    const OracleResult& oracle,
    double epsilon = 1e-9) noexcept
{
    VerificationResult res;
    res.greedy_value = greedy.objective_value;
    res.optimal_value = oracle.optimal_value;

    // Infeasible greedy solution is a FEASIBILITY_FAILURE
    if (!greedy.feasible) {
        res.status = VerificationStatus::FEASIBILITY_FAILURE;
        // Feasibility failure on an exact oracle is a counterexample
        res.is_counterexample = oracle.is_exact();
        res.absolute_gap = std::numeric_limits<double>::infinity();
        res.relative_gap = std::numeric_limits<double>::infinity();
        res.failure_percent = 100.0;
        res.summary = "Greedy solver failed to produce a feasible solution.";
        return res;
    }

    if (direction == ObjectiveDirection::MINIMIZE) {
        // For MINIMIZATION: Greedy > Optimal => Failure
        double diff = greedy.objective_value - oracle.optimal_value;
        if (diff > epsilon) {
            res.status = VerificationStatus::OPTIMALITY_FAILURE;
            res.is_counterexample = oracle.is_exact();
            res.absolute_gap = diff;
            res.relative_gap = (std::abs(oracle.optimal_value) > epsilon)
                ? (greedy.objective_value / oracle.optimal_value)
                : 1.0;
            res.failure_percent = (std::abs(oracle.optimal_value) > epsilon)
                ? (diff / std::abs(oracle.optimal_value)) * 100.0
                : 0.0;
            res.summary = "Greedy produced suboptimal solution (exceeded exact minimum).";
        } else {
            res.status = VerificationStatus::OPTIMAL;
            res.is_counterexample = false;
            res.absolute_gap = 0.0;
            res.relative_gap = 1.0;
            res.failure_percent = 0.0;
            res.summary = "Greedy matches optimal objective.";
        }
    } else {
        // For MAXIMIZATION: Greedy < Optimal => Failure
        double diff = oracle.optimal_value - greedy.objective_value;
        if (diff > epsilon) {
            res.status = VerificationStatus::OPTIMALITY_FAILURE;
            res.is_counterexample = oracle.is_exact();
            res.absolute_gap = diff;
            res.relative_gap = (std::abs(greedy.objective_value) > epsilon)
                ? (oracle.optimal_value / greedy.objective_value)
                : std::numeric_limits<double>::infinity();
            res.failure_percent = (std::abs(oracle.optimal_value) > epsilon)
                ? (diff / std::abs(oracle.optimal_value)) * 100.0
                : 0.0;
            res.summary = "Greedy produced suboptimal solution (fell short of exact maximum).";
        } else {
            res.status = VerificationStatus::OPTIMAL;
            res.is_counterexample = false;
            res.absolute_gap = 0.0;
            res.relative_gap = 1.0;
            res.failure_percent = 0.0;
            res.summary = "Greedy matches optimal objective.";
        }
    }

    return res;
}

} // namespace greedybreaker::core
