#pragma once

#include "greedybreaker/core/types.hpp"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace greedybreaker::search {

enum class SearchMode : uint8_t {
    ANY = 0,
    SMALLEST = 1,
    WORST = 2,
    PROFILE = 3
};

[[nodiscard]] constexpr std::string_view to_string(SearchMode mode) noexcept {
    switch (mode) {
        case SearchMode::ANY: return "ANY";
        case SearchMode::SMALLEST: return "SMALLEST";
        case SearchMode::WORST: return "WORST";
        case SearchMode::PROFILE: return "PROFILE";
    }
    return "UNKNOWN";
}

enum class SearchStrategy : uint8_t {
    EXHAUSTIVE = 0,
    RANDOMIZED = 1,
    GUIDED = 2,
    HYBRID = 3
};

[[nodiscard]] constexpr std::string_view to_string(SearchStrategy strat) noexcept {
    switch (strat) {
        case SearchStrategy::EXHAUSTIVE: return "EXHAUSTIVE";
        case SearchStrategy::RANDOMIZED: return "RANDOMIZED";
        case SearchStrategy::GUIDED: return "GUIDED";
        case SearchStrategy::HYBRID: return "HYBRID";
    }
    return "UNKNOWN";
}

enum class SearchCoverage : uint8_t {
    EXHAUSTIVE = 0,
    PARTIAL = 1,
    RANDOMIZED_SAMPLE = 2
};

[[nodiscard]] constexpr std::string_view to_string(SearchCoverage cov) noexcept {
    switch (cov) {
        case SearchCoverage::EXHAUSTIVE: return "EXHAUSTIVE";
        case SearchCoverage::PARTIAL: return "PARTIAL";
        case SearchCoverage::RANDOMIZED_SAMPLE: return "RANDOMIZED_SAMPLE";
    }
    return "UNKNOWN";
}

struct SearchConfig {
    std::string experiment_id{"exp_default"};
    std::string problem_type{"coin_change"};
    std::string strategy_id{"default"};
    std::string oracle_id{"default"};

    SearchMode mode{SearchMode::ANY};
    SearchStrategy strategy{SearchStrategy::EXHAUSTIVE};

    uint64_t random_seed{42};
    uint64_t max_candidates{10'000};
    uint64_t max_time_ms{5'000};

    nlohmann::json search_space;
};

void to_json(nlohmann::json& j, const SearchConfig& cfg);
void from_json(const nlohmann::json& j, SearchConfig& cfg);

} // namespace greedybreaker::search
