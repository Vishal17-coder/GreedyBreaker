#pragma once

#include <string>
#include <vector>

namespace greedybreaker {

/**
 * @brief Algorithmic complexity specifications in Big-O notation.
 */
struct ComplexityEntry {
    std::string algorithm_name;
    std::string time_best;
    std::string time_average;
    std::string time_worst;
    std::string space_worst;
    std::string explanation;
};

/**
 * @brief Comprehensive complexity report for a problem and its solvers.
 */
struct ComplexityReport {
    std::string problem_name;
    std::vector<ComplexityEntry> solvers;
};

} // namespace greedybreaker
