#pragma once

#include <string>

namespace greedybreaker {

/**
 * @brief Search termination objective mode.
 */
enum class SearchMode {
    ANY,       ///< Terminate immediately upon discovering the first verified counterexample.
    SMALLEST,  ///< Find the smallest verified counterexample under canonical ordering.
    WORST      ///< Search the bounded space and maximize failure severity (ratio and gap).
};

/**
 * @brief Strategy used to navigate the candidate search space.
 */
enum class SearchStrategy {
    EXHAUSTIVE,    ///< Systematic, deterministic Cartesian enumeration.
    BACKTRACKING,  ///< Recursive candidate generation with branch-and-bound pruning.
    RANDOMIZED,    ///< PRNG-guided exploration with reproducible seed.
    HEURISTIC      ///< Deterministic heuristic prioritizing remainder traps and non-canonical gaps.
};

/**
 * @brief Reason for search termination.
 */
enum class TerminationReason {
    COMPLETED_SPACE_EXHAUSTED,
    FOUND_FIRST_COUNTEREXAMPLE,
    FOUND_SMALLEST_VERIFIED,
    MAX_CANDIDATES_REACHED,
    MAX_ITERATIONS_REACHED,
    CANCELLED_BY_USER
};

inline std::string to_string(SearchMode mode) {
    switch (mode) {
        case SearchMode::ANY: return "ANY";
        case SearchMode::SMALLEST: return "SMALLEST";
        case SearchMode::WORST: return "WORST";
    }
    return "UNKNOWN";
}

inline std::string to_string(SearchStrategy strategy) {
    switch (strategy) {
        case SearchStrategy::EXHAUSTIVE: return "EXHAUSTIVE";
        case SearchStrategy::BACKTRACKING: return "BACKTRACKING";
        case SearchStrategy::RANDOMIZED: return "RANDOMIZED";
        case SearchStrategy::HEURISTIC: return "HEURISTIC";
    }
    return "UNKNOWN";
}

inline std::string to_string(TerminationReason reason) {
    switch (reason) {
        case TerminationReason::COMPLETED_SPACE_EXHAUSTED: return "Search space exhausted";
        case TerminationReason::FOUND_FIRST_COUNTEREXAMPLE: return "Stopped after first counterexample (ANY mode)";
        case TerminationReason::FOUND_SMALLEST_VERIFIED: return "Smallest verified counterexample found (SMALLEST mode)";
        case TerminationReason::MAX_CANDIDATES_REACHED: return "Safety limit: max candidates evaluated reached";
        case TerminationReason::MAX_ITERATIONS_REACHED: return "Safety limit: max iterations reached";
        case TerminationReason::CANCELLED_BY_USER: return "Search cancelled by user request";
    }
    return "UNKNOWN";
}

} // namespace greedybreaker
