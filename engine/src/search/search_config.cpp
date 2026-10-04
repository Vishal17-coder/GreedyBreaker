#include "greedybreaker/search/search_config.hpp"
#include <sstream>

namespace greedybreaker {

void SearchConfig::validate() const {
    if (max_candidates == 0) {
        throw InvalidCandidateException("max_candidates safety limit must be > 0.");
    }
    if (max_iterations == 0) {
        throw InvalidCandidateException("max_iterations must be > 0.");
    }

    if (problem_type == ProblemType::COIN_CHANGE) {
        if (min_denomination <= 0) {
            throw InvalidCandidateException("min_denomination must be strictly positive (> 0); received: " +
                std::to_string(min_denomination));
        }
        if (max_denomination < min_denomination) {
            throw InvalidCandidateException("max_denomination (" + std::to_string(max_denomination) +
                ") cannot be smaller than min_denomination (" + std::to_string(min_denomination) + ").");
        }
        if (min_coin_count <= 0) {
            throw InvalidCandidateException("min_coin_count must be strictly positive (> 0); received: " +
                std::to_string(min_coin_count));
        }
        if (max_coin_count < min_coin_count) {
            throw InvalidCandidateException("max_coin_count (" + std::to_string(max_coin_count) +
                ") cannot be smaller than min_coin_count (" + std::to_string(min_coin_count) + ").");
        }
        int available_range = max_denomination - min_denomination + 1;
        if (max_coin_count > available_range) {
            throw InvalidCandidateException("max_coin_count (" + std::to_string(max_coin_count) +
                ") exceeds total available distinct denominations in range [" +
                std::to_string(min_denomination) + ".." + std::to_string(max_denomination) + "] (" +
                std::to_string(available_range) + ").");
        }
        if (min_target <= 0) {
            throw InvalidCandidateException("min_target must be strictly positive (> 0); received: " +
                std::to_string(min_target));
        }
        if (max_target < min_target) {
            throw InvalidCandidateException("max_target (" + std::to_string(max_target) +
                ") cannot be smaller than min_target (" + std::to_string(min_target) + ").");
        }
        if (require_unit_coin && (min_denomination > 1 || max_denomination < 1)) {
            throw InvalidCandidateException("require_unit_coin is true, but denomination 1 is outside the range [" +
                std::to_string(min_denomination) + ".." + std::to_string(max_denomination) + "].");
        }
    } else if (problem_type == ProblemType::ACTIVITY_SELECTION) {
        if (min_activities <= 0) {
            throw InvalidCandidateException("min_activities must be strictly positive (> 0); received: " +
                std::to_string(min_activities));
        }
        if (max_activities < min_activities) {
            throw InvalidCandidateException("max_activities (" + std::to_string(max_activities) +
                ") cannot be smaller than min_activities (" + std::to_string(min_activities) + ").");
        }
        if (min_time < 0) {
            throw InvalidCandidateException("min_time must be non-negative (>= 0); received: " +
                std::to_string(min_time));
        }
        if (max_time <= min_time) {
            throw InvalidCandidateException("max_time (" + std::to_string(max_time) +
                ") must be strictly greater than min_time (" + std::to_string(min_time) + ").");
        }
        if (min_duration <= 0) {
            throw InvalidCandidateException("min_duration must be strictly positive (> 0); received: " +
                std::to_string(min_duration));
        }
    } else if (problem_type == ProblemType::KNAPSACK_01 || problem_type == ProblemType::KNAPSACK_FRACTIONAL) {
        if (min_items <= 0) {
            throw InvalidCandidateException("min_items must be strictly positive (> 0); received: " +
                std::to_string(min_items));
        }
        if (max_items < min_items) {
            throw InvalidCandidateException("max_items (" + std::to_string(max_items) +
                ") cannot be smaller than min_items (" + std::to_string(min_items) + ").");
        }
        if (min_weight <= 0) {
            throw InvalidCandidateException("min_weight must be strictly positive (> 0); received: " +
                std::to_string(min_weight));
        }
        if (max_weight < min_weight) {
            throw InvalidCandidateException("max_weight (" + std::to_string(max_weight) +
                ") cannot be smaller than min_weight (" + std::to_string(min_weight) + ").");
        }
        if (min_value < 0) {
            throw InvalidCandidateException("min_value must be non-negative (>= 0); received: " +
                std::to_string(min_value));
        }
        if (max_value < min_value) {
            throw InvalidCandidateException("max_value (" + std::to_string(max_value) +
                ") cannot be smaller than min_value (" + std::to_string(min_value) + ").");
        }
        if (min_capacity <= 0) {
            throw InvalidCandidateException("min_capacity must be strictly positive (> 0); received: " +
                std::to_string(min_capacity));
        }
        if (max_capacity < min_capacity) {
            throw InvalidCandidateException("max_capacity (" + std::to_string(max_capacity) +
                ") cannot be smaller than min_capacity (" + std::to_string(min_capacity) + ").");
        }
    }
}

std::string SearchConfig::describe_space() const {
    std::ostringstream oss;
    oss << "Problem: " << to_string(problem_type)
        << " | Strategy: " << to_string(strategy)
        << " | Mode: " << to_string(mode);

    if (problem_type == ProblemType::COIN_CHANGE) {
        oss << " | Denominations: [" << min_denomination << ".." << max_denomination << "]"
            << " | Count: [" << min_coin_count << ".." << max_coin_count << "]"
            << " | Target: [" << min_target << ".." << max_target << "]"
            << " | Require 1: " << (require_unit_coin ? "YES" : "NO");
    } else if (problem_type == ProblemType::ACTIVITY_SELECTION) {
        oss << " | Activities: [" << min_activities << ".." << max_activities << "]"
            << " | Time: [" << min_time << ".." << max_time << "]"
            << " | Min Duration: " << min_duration;
    } else {
        oss << " | Items: [" << min_items << ".." << max_items << "]"
            << " | Weight: [" << min_weight << ".." << max_weight << "]"
            << " | Value: [" << min_value << ".." << max_value << "]"
            << " | Capacity: [" << min_capacity << ".." << max_capacity << "]";
    }

    oss << " | Max Candidates: " << max_candidates;
    if (strategy == SearchStrategy::RANDOMIZED) {
        oss << " | Seed: " << random_seed << " | Max Iterations: " << max_iterations;
    }
    return oss.str();
}

} // namespace greedybreaker
