#pragma once

#include "greedybreaker/core/error.hpp"
#include "greedybreaker/core/solution.hpp"
#include "greedybreaker/core/types.hpp"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace greedybreaker::problems {

struct HuffmanSymbol {
    int64_t id{0};
    std::string symbol;
    int64_t frequency{0};

    [[nodiscard]] bool operator==(const HuffmanSymbol& other) const noexcept {
        return id == other.id && symbol == other.symbol && frequency == other.frequency;
    }
};

void to_json(nlohmann::json& j, const HuffmanSymbol& sym);
void from_json(const nlohmann::json& j, HuffmanSymbol& sym);

struct HuffmanCodingInstance {
    std::vector<HuffmanSymbol> symbols;

    [[nodiscard]] bool operator==(const HuffmanCodingInstance& other) const noexcept {
        return symbols == other.symbols;
    }
};

void to_json(nlohmann::json& j, const HuffmanCodingInstance& inst);
void from_json(const nlohmann::json& j, HuffmanCodingInstance& inst);

class HuffmanCodingProblem {
public:
    [[nodiscard]] static std::string problem_id() noexcept { return "huffman_coding"; }
    [[nodiscard]] static std::string display_name() noexcept { return "Huffman Coding"; }
    [[nodiscard]] static core::ObjectiveDirection objective() noexcept { return core::ObjectiveDirection::MINIMIZE; }

    [[nodiscard]] static bool is_valid_instance(const HuffmanCodingInstance& inst, std::string* error_msg = nullptr) noexcept;

    // Greedy solver supporting:
    // - "standard_huffman" (Classic optimal two-lowest merge)
    // - "shannon_fano" (Breakable top-down splitting heuristic)
    [[nodiscard]] static core::GreedyResult solve_greedy(
        const HuffmanCodingInstance& inst,
        const std::string& strategy = "standard_huffman"
    );

    // Exact Oracle: Computes optimal prefix code cost via Huffman optimality theorem
    [[nodiscard]] static core::OracleResult solve_exact(
        const HuffmanCodingInstance& inst
    );
};

} // namespace greedybreaker::problems
