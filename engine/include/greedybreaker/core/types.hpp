#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <optional>
#include <cstdint>

namespace greedybreaker {

/**
 * @brief Direction of optimization.
 */
enum class ProblemObjective {
    Minimize,
    Maximize
};

/**
 * @brief Exception thrown when candidate problem inputs violate domain constraints.
 */
class InvalidCandidateException : public std::runtime_error {
public:
    explicit InvalidCandidateException(const std::string& message)
        : std::runtime_error(message) {}
};

} // namespace greedybreaker
