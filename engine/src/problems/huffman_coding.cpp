#include "greedybreaker/problems/huffman_coding.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <numeric>
#include <queue>
#include <sstream>

namespace greedybreaker::problems {

void to_json(nlohmann::json& j, const HuffmanSymbol& sym) {
    j = nlohmann::json{
        {"id", sym.id},
        {"symbol", sym.symbol},
        {"frequency", sym.frequency}
    };
}

void from_json(const nlohmann::json& j, HuffmanSymbol& sym) {
    sym.id = j.value("id", int64_t{0});
    sym.symbol = j.value("symbol", "");
    sym.frequency = j.value("frequency", int64_t{0});
}

void to_json(nlohmann::json& j, const HuffmanCodingInstance& inst) {
    j = nlohmann::json{
        {"symbols", inst.symbols}
    };
}

void from_json(const nlohmann::json& j, HuffmanCodingInstance& inst) {
    if (j.contains("symbols")) {
        inst.symbols = j.at("symbols").get<std::vector<HuffmanSymbol>>();
    }
}

bool HuffmanCodingProblem::is_valid_instance(const HuffmanCodingInstance& inst, std::string* error_msg) noexcept {
    if (inst.symbols.size() < 2) {
        if (error_msg) *error_msg = "At least 2 symbols required for prefix code construction.";
        return false;
    }
    for (const auto& s : inst.symbols) {
        if (s.frequency <= 0) {
            if (error_msg) *error_msg = "Symbol frequencies must be strictly positive.";
            return false;
        }
    }
    return true;
}

namespace {

struct HuffmanNode {
    int64_t id{-1};
    std::string label;
    int64_t freq{0};
    std::shared_ptr<HuffmanNode> left;
    std::shared_ptr<HuffmanNode> right;

    [[nodiscard]] bool is_leaf() const noexcept {
        return !left && !right;
    }
};

struct CompareNode {
    bool operator()(const std::shared_ptr<HuffmanNode>& a, const std::shared_ptr<HuffmanNode>& b) const {
        if (a->freq != b->freq) return a->freq > b->freq;
        return a->label > b->label;
    }
};

void compute_leaf_depths(const std::shared_ptr<HuffmanNode>& node, int64_t current_depth, std::vector<int64_t>& depths) {
    if (!node) return;
    if (node->is_leaf()) {
        if (node->id >= 0 && static_cast<size_t>(node->id) < depths.size()) {
            depths[static_cast<size_t>(node->id)] = (current_depth == 0) ? 1 : current_depth;
        }
        return;
    }
    compute_leaf_depths(node->left, current_depth + 1, depths);
    compute_leaf_depths(node->right, current_depth + 1, depths);
}

void shannon_fano_recursive(
    std::vector<HuffmanSymbol>& symbols,
    size_t left,
    size_t right,
    std::vector<int64_t>& depths,
    core::DecisionTrace& trace,
    size_t& step_idx)
{
    if (left >= right) return;

    if (right - left == 1) {
        depths[static_cast<size_t>(symbols[left].id)] += 1;
        depths[static_cast<size_t>(symbols[right].id)] += 1;

        core::DecisionStep step;
        step.step_index = step_idx++;
        step.heuristic_rule = "Shannon-Fano base split";
        step.action_description = "Split pair {" + symbols[left].symbol + "} and {" + symbols[right].symbol + "}";
        trace.add_step(step);
        return;
    }

    // Compute prefix sums
    int64_t total_freq = 0;
    for (size_t i = left; i <= right; ++i) {
        total_freq += symbols[i].frequency;
    }

    int64_t running_sum = 0;
    size_t best_split = left;
    int64_t best_diff = std::numeric_limits<int64_t>::max();

    for (size_t i = left; i < right; ++i) {
        running_sum += symbols[i].frequency;
        int64_t rest = total_freq - running_sum;
        int64_t diff = std::abs(running_sum - rest);
        if (diff < best_diff) {
            best_diff = diff;
            best_split = i;
        }
    }

    // Record decision step
    core::DecisionStep step;
    step.step_index = step_idx++;
    step.heuristic_rule = "Shannon-Fano minimum frequency delta split";
    step.state_before = "Range [" + std::to_string(left) + ", " + std::to_string(right) + "], total frequency " + std::to_string(total_freq);
    step.action_description = "Split at index " + std::to_string(best_split) + " (subset difference " + std::to_string(best_diff) + ")";
    trace.add_step(step);

    for (size_t i = left; i <= best_split; ++i) {
        depths[static_cast<size_t>(symbols[i].id)] += 1;
    }
    for (size_t i = best_split + 1; i <= right; ++i) {
        depths[static_cast<size_t>(symbols[i].id)] += 1;
    }

    shannon_fano_recursive(symbols, left, best_split, depths, trace, step_idx);
    shannon_fano_recursive(symbols, best_split + 1, right, depths, trace, step_idx);
}

} // namespace

core::GreedyResult HuffmanCodingProblem::solve_greedy(
    const HuffmanCodingInstance& inst,
    const std::string& strategy)
{
    const auto start_time = std::chrono::steady_clock::now();
    core::GreedyResult result;

    if (!is_valid_instance(inst)) {
        result.feasible = false;
        result.objective_value = 0.0;
        result.solution_description = "Invalid Huffman Coding instance.";
        return result;
    }

    const size_t N = inst.symbols.size();
    std::vector<int64_t> depths(N, 0);

    if (strategy == "shannon_fano") {
        auto sorted_symbols = inst.symbols;
        // Assign indices 0..N-1
        for (size_t i = 0; i < N; ++i) {
            sorted_symbols[i].id = static_cast<int64_t>(i);
        }
        std::sort(sorted_symbols.begin(), sorted_symbols.end(), [](const HuffmanSymbol& a, const HuffmanSymbol& b) {
            if (a.frequency != b.frequency) return a.frequency > b.frequency;
            return a.symbol < b.symbol;
        });

        size_t step_idx = 0;
        shannon_fano_recursive(sorted_symbols, 0, N - 1, depths, result.trace, step_idx);

        double total_weighted_length = 0.0;
        for (size_t i = 0; i < N; ++i) {
            total_weighted_length += static_cast<double>(sorted_symbols[i].frequency * depths[static_cast<size_t>(sorted_symbols[i].id)]);
        }

        const auto end_time = std::chrono::steady_clock::now();
        result.metrics.execution_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
        result.metrics.states_evaluated = result.trace.size();

        result.feasible = true;
        result.objective_value = total_weighted_length;
        result.solution_description = "Shannon-Fano Code cost: " + std::to_string(total_weighted_length);
        return result;
    }

    // Default: Standard Huffman two-lowest merge
    std::priority_queue<std::shared_ptr<HuffmanNode>, std::vector<std::shared_ptr<HuffmanNode>>, CompareNode> pq;
    for (size_t i = 0; i < N; ++i) {
        auto leaf = std::make_shared<HuffmanNode>();
        leaf->id = static_cast<int64_t>(i);
        leaf->label = inst.symbols[i].symbol;
        leaf->freq = inst.symbols[i].frequency;
        pq.push(leaf);
    }

    size_t step_idx = 0;
    while (pq.size() > 1) {
        auto n1 = pq.top(); pq.pop();
        auto n2 = pq.top(); pq.pop();

        auto parent = std::make_shared<HuffmanNode>();
        parent->id = -1;
        parent->label = "(" + n1->label + "+" + n2->label + ")";
        parent->freq = n1->freq + n2->freq;
        parent->left = n1;
        parent->right = n2;

        core::DecisionStep step;
        step.step_index = step_idx++;
        step.heuristic_rule = "Merge two lowest frequency subtrees";
        step.state_before = "Forest size: " + std::to_string(pq.size() + 2);
        step.action_description = "Merged [" + n1->label + " (" + std::to_string(n1->freq) + ")] and [" +
                                  n2->label + " (" + std::to_string(n2->freq) + ")] -> " + std::to_string(parent->freq);
        step.state_after = "Forest size: " + std::to_string(pq.size() + 1);
        result.trace.add_step(step);

        pq.push(parent);
    }

    auto root = pq.top();
    compute_leaf_depths(root, 0, depths);

    double total_weighted_length = 0.0;
    for (size_t i = 0; i < N; ++i) {
        total_weighted_length += static_cast<double>(inst.symbols[i].frequency * depths[i]);
    }

    const auto end_time = std::chrono::steady_clock::now();
    result.metrics.execution_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    result.metrics.states_evaluated = result.trace.size();

    result.feasible = true;
    result.objective_value = total_weighted_length;
    result.solution_description = "Optimal Huffman Code cost: " + std::to_string(total_weighted_length);

    return result;
}

core::OracleResult HuffmanCodingProblem::solve_exact(const HuffmanCodingInstance& inst) {
    const auto start_time = std::chrono::steady_clock::now();
    core::OracleResult result;
    result.confidence = core::OracleConfidence::EXACT;
    result.oracle_method = "Huffman Optimal Prefix Code Exact Constructor";

    if (!is_valid_instance(inst)) {
        result.optimal_value = 0.0;
        result.solution_description = "Invalid Huffman Coding instance.";
        return result;
    }

    auto huffman_res = solve_greedy(inst, "standard_huffman");

    const auto end_time = std::chrono::steady_clock::now();
    result.metrics.execution_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    result.metrics.states_evaluated = inst.symbols.size();

    result.optimal_value = huffman_res.objective_value;
    result.solution_description = "Exact optimal prefix code cost: " + std::to_string(huffman_res.objective_value);

    return result;
}

} // namespace greedybreaker::problems
