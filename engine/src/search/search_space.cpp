#include "greedybreaker/search/search_space.hpp"
#include <algorithm>
#include <cmath>

namespace greedybreaker::search {

static uint64_t nCr(uint64_t n, uint64_t r) {
    if (r > n) return 0;
    if (r == 0 || r == n) return 1;
    if (r > n / 2) r = n - r;
    uint64_t res = 1;
    for (uint64_t i = 1; i <= r; ++i) {
        res = res * (n - i + 1) / i;
    }
    return res;
}

uint64_t CoinChangeSearchSpace::estimate_cardinality() const noexcept {
    uint64_t span_d = static_cast<uint64_t>(std::max(int64_t{0}, denomination_range.span()));
    uint64_t span_t = static_cast<uint64_t>(std::max(int64_t{0}, target_range.span()));
    uint64_t total_coin_combos = 0;

    for (int64_t k = denomination_count.min; k <= denomination_count.max; ++k) {
        if (k > 0 && span_d >= static_cast<uint64_t>(k)) {
            total_coin_combos += nCr(span_d, static_cast<uint64_t>(k));
        }
    }
    return total_coin_combos * span_t;
}

uint64_t KnapsackSearchSpace::estimate_cardinality() const noexcept {
    uint64_t span_w = static_cast<uint64_t>(std::max(int64_t{0}, weight_range.span()));
    uint64_t span_v = static_cast<uint64_t>(std::max(int64_t{0}, value_range.span()));
    uint64_t span_c = static_cast<uint64_t>(std::max(int64_t{0}, capacity_range.span()));
    uint64_t item_pairs = span_w * span_v;

    uint64_t total_configs = 0;
    for (int64_t n = item_count.min; n <= item_count.max; ++n) {
        // Approximate combinations of n items
        double combos = std::pow(static_cast<double>(item_pairs), static_cast<double>(n));
        if (combos > 1e12) return 1'000'000'000'000ULL;
        total_configs += static_cast<uint64_t>(combos);
    }
    return total_configs * span_c;
}

uint64_t ActivitySelectionSearchSpace::estimate_cardinality() const noexcept {
    uint64_t span_t = static_cast<uint64_t>(std::max(int64_t{0}, time_range.span()));
    uint64_t span_d = static_cast<uint64_t>(std::max(int64_t{0}, duration_range.span()));
    uint64_t act_options = span_t * span_d;

    uint64_t total = 0;
    for (int64_t n = activity_count.min; n <= activity_count.max; ++n) {
        double combos = std::pow(static_cast<double>(act_options), static_cast<double>(n));
        if (combos > 1e12) return 1'000'000'000'000ULL;
        total += static_cast<uint64_t>(combos);
    }
    return total;
}

uint64_t HuffmanCodingSearchSpace::estimate_cardinality() const noexcept {
    uint64_t span_f = static_cast<uint64_t>(std::max(int64_t{0}, frequency_range.span()));
    uint64_t total = 0;
    for (int64_t n = symbol_count.min; n <= symbol_count.max; ++n) {
        double combos = std::pow(static_cast<double>(span_f), static_cast<double>(n));
        if (combos > 1e12) return 1'000'000'000'000ULL;
        total += static_cast<uint64_t>(combos);
    }
    return total;
}

void to_json(nlohmann::json& j, const CoinChangeSearchSpace& s) {
    j = nlohmann::json{
        {"denomination_count", s.denomination_count},
        {"denomination_range", s.denomination_range},
        {"target_range", s.target_range}
    };
}

void from_json(const nlohmann::json& j, CoinChangeSearchSpace& s) {
    if (j.contains("denomination_count")) s.denomination_count = j.at("denomination_count").get<IntRange>();
    if (j.contains("denomination_range")) s.denomination_range = j.at("denomination_range").get<IntRange>();
    if (j.contains("target_range")) s.target_range = j.at("target_range").get<IntRange>();
}

void to_json(nlohmann::json& j, const KnapsackSearchSpace& s) {
    j = nlohmann::json{
        {"item_count", s.item_count},
        {"weight_range", s.weight_range},
        {"value_range", s.value_range},
        {"capacity_range", s.capacity_range}
    };
}

void from_json(const nlohmann::json& j, KnapsackSearchSpace& s) {
    if (j.contains("item_count")) s.item_count = j.at("item_count").get<IntRange>();
    if (j.contains("weight_range")) s.weight_range = j.at("weight_range").get<IntRange>();
    if (j.contains("value_range")) s.value_range = j.at("value_range").get<IntRange>();
    if (j.contains("capacity_range")) s.capacity_range = j.at("capacity_range").get<IntRange>();
}

void to_json(nlohmann::json& j, const ActivitySelectionSearchSpace& s) {
    j = nlohmann::json{
        {"activity_count", s.activity_count},
        {"time_range", s.time_range},
        {"duration_range", s.duration_range}
    };
}

void from_json(const nlohmann::json& j, ActivitySelectionSearchSpace& s) {
    if (j.contains("activity_count")) s.activity_count = j.at("activity_count").get<IntRange>();
    if (j.contains("time_range")) s.time_range = j.at("time_range").get<IntRange>();
    if (j.contains("duration_range")) s.duration_range = j.at("duration_range").get<IntRange>();
}

void to_json(nlohmann::json& j, const HuffmanCodingSearchSpace& s) {
    j = nlohmann::json{
        {"symbol_count", s.symbol_count},
        {"frequency_range", s.frequency_range}
    };
}

void from_json(const nlohmann::json& j, HuffmanCodingSearchSpace& s) {
    if (j.contains("symbol_count")) s.symbol_count = j.at("symbol_count").get<IntRange>();
    if (j.contains("frequency_range")) s.frequency_range = j.at("frequency_range").get<IntRange>();
}

} // namespace greedybreaker::search
