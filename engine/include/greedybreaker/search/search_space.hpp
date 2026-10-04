#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace greedybreaker::search {

struct IntRange {
    int64_t min{0};
    int64_t max{0};

    [[nodiscard]] int64_t span() const noexcept {
        return (max >= min) ? (max - min + 1) : 0;
    }
};

inline void to_json(nlohmann::json& j, const IntRange& r) {
    j = nlohmann::json{{"min", r.min}, {"max", r.max}};
}

inline void from_json(const nlohmann::json& j, IntRange& r) {
    r.min = j.value("min", int64_t{0});
    r.max = j.value("max", int64_t{0});
}

// Search space bounds for Coin Change
struct CoinChangeSearchSpace {
    IntRange denomination_count{3, 4};
    IntRange denomination_range{1, 20};
    IntRange target_range{1, 30};

    [[nodiscard]] uint64_t estimate_cardinality() const noexcept;
};

void to_json(nlohmann::json& j, const CoinChangeSearchSpace& s);
void from_json(const nlohmann::json& j, CoinChangeSearchSpace& s);

// Search space bounds for Knapsack (0/1 and Fractional)
struct KnapsackSearchSpace {
    IntRange item_count{3, 5};
    IntRange weight_range{1, 20};
    IntRange value_range{1, 50};
    IntRange capacity_range{10, 40};

    [[nodiscard]] uint64_t estimate_cardinality() const noexcept;
};

void to_json(nlohmann::json& j, const KnapsackSearchSpace& s);
void from_json(const nlohmann::json& j, KnapsackSearchSpace& s);

// Search space bounds for Activity Selection
struct ActivitySelectionSearchSpace {
    IntRange activity_count{3, 6};
    IntRange time_range{0, 20};
    IntRange duration_range{1, 10};

    [[nodiscard]] uint64_t estimate_cardinality() const noexcept;
};

void to_json(nlohmann::json& j, const ActivitySelectionSearchSpace& s);
void from_json(const nlohmann::json& j, ActivitySelectionSearchSpace& s);

// Search space bounds for Huffman Coding
struct HuffmanCodingSearchSpace {
    IntRange symbol_count{3, 6};
    IntRange frequency_range{1, 50};

    [[nodiscard]] uint64_t estimate_cardinality() const noexcept;
};

void to_json(nlohmann::json& j, const HuffmanCodingSearchSpace& s);
void from_json(const nlohmann::json& j, HuffmanCodingSearchSpace& s);

} // namespace greedybreaker::search
