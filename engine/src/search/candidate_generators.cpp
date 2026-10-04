#include "greedybreaker/search/candidate_generators.hpp"
#include <algorithm>
#include <vector>
#include <cmath>

namespace greedybreaker {

bool stream_coin_change_candidates(
    const SearchConfig& config,
    const std::function<bool(const CoinChangeCandidate&)>& consumer
) {
    for (int target = config.min_target; target <= config.max_target; ++target) {
        for (int k = config.min_coin_count; k <= config.max_coin_count; ++k) {
            if (config.require_unit_coin) {
                if (k == 1) {
                    CoinChangeCandidate cand{{1}, target};
                    if (!consumer(cand)) return false;
                    continue;
                }

                int rem_k = k - 1;
                int start_val = std::max(2, config.min_denomination);
                int end_val = config.max_denomination;
                int avail = end_val - start_val + 1;
                if (rem_k > avail) continue;

                std::vector<int> rem_combo(rem_k);
                for (int i = 0; i < rem_k; ++i) rem_combo[i] = start_val + i;

                bool done = false;
                while (!done) {
                    std::vector<int> full_combo = {1};
                    full_combo.insert(full_combo.end(), rem_combo.begin(), rem_combo.end());

                    CoinChangeCandidate cand{full_combo, target};
                    if (!consumer(cand)) return false;

                    int idx = rem_k - 1;
                    while (idx >= 0 && rem_combo[idx] == end_val - (rem_k - 1 - idx)) {
                        --idx;
                    }
                    if (idx < 0) {
                        done = true;
                    } else {
                        rem_combo[idx]++;
                        for (int j = idx + 1; j < rem_k; ++j) {
                            rem_combo[j] = rem_combo[j - 1] + 1;
                        }
                    }
                }
            } else {
                int range_size = config.max_denomination - config.min_denomination + 1;
                if (k > range_size) continue;

                std::vector<int> combo(k);
                for (int i = 0; i < k; ++i) {
                    combo[i] = config.min_denomination + i;
                }

                bool done = false;
                while (!done) {
                    CoinChangeCandidate cand{combo, target};
                    if (!consumer(cand)) return false;

                    int idx = k - 1;
                    while (idx >= 0 && combo[idx] == config.max_denomination - (k - 1 - idx)) {
                        --idx;
                    }
                    if (idx < 0) {
                        done = true;
                    } else {
                        combo[idx]++;
                        for (int j = idx + 1; j < k; ++j) {
                            combo[j] = combo[j - 1] + 1;
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool stream_activity_candidates(
    const SearchConfig& config,
    const std::function<bool(const ActivitySelectionCandidate&)>& consumer
) {
    // Generate valid intervals [start, finish]
    struct SimpleInterval { int s; int f; };
    std::vector<SimpleInterval> pool;
    for (int s = config.min_time; s <= config.max_time - config.min_duration; ++s) {
        for (int f = s + config.min_duration; f <= config.max_time; ++f) {
            pool.push_back({s, f});
        }
    }

    const int total_avail = static_cast<int>(pool.size());

    for (int k = config.min_activities; k <= config.max_activities; ++k) {
        if (k > total_avail) continue;

        std::vector<int> indices(k);
        for (int i = 0; i < k; ++i) indices[i] = i;

        bool done = false;
        while (!done) {
            ActivitySelectionCandidate candidate;
            candidate.activities.reserve(k);
            for (int i = 0; i < k; ++i) {
                candidate.activities.push_back(Activity{
                    .id = i + 1,
                    .start = pool[indices[i]].s,
                    .finish = pool[indices[i]].f
                });
            }

            if (!consumer(candidate)) return false;

            int idx = k - 1;
            while (idx >= 0 && indices[idx] == total_avail - (k - idx)) {
                --idx;
            }
            if (idx < 0) {
                done = true;
            } else {
                indices[idx]++;
                for (int j = idx + 1; j < k; ++j) {
                    indices[j] = indices[j - 1] + 1;
                }
            }
        }
    }
    return true;
}

bool stream_knapsack_candidates(
    const SearchConfig& config,
    const std::function<bool(const KnapsackCandidate&)>& consumer
) {
    for (int W = config.min_capacity; W <= config.max_capacity; ++W) {
        // Collect a compact, representative grid of item pairs (value, weight)
        std::vector<std::pair<int, int>> item_pool;
        int max_w = std::min(config.max_weight, W);
        for (int w = config.min_weight; w <= max_w; ++w) {
            for (int v = config.min_value; v <= config.max_value; ++v) {
                item_pool.push_back({v, w});
            }
        }

        // Sort items by ratio descending, then value descending, then weight ascending
        std::sort(item_pool.begin(), item_pool.end(), [](const auto& a, const auto& b) {
            double r_a = static_cast<double>(a.first) / a.second;
            double r_b = static_cast<double>(b.first) / b.second;
            if (std::abs(r_a - r_b) > 1e-9) return r_a > r_b;
            if (a.first != b.first) return a.first > b.first;
            return a.second < b.second;
        });

        const int pool_size = static_cast<int>(item_pool.size());

        for (int k = config.min_items; k <= config.max_items; ++k) {
            if (k > pool_size) continue;

            std::vector<int> indices(k);
            for (int i = 0; i < k; ++i) indices[i] = i;

            bool done = false;
            while (!done) {
                KnapsackCandidate candidate;
                candidate.capacity = W;
                candidate.items.reserve(k);
                for (int i = 0; i < k; ++i) {
                    candidate.items.push_back(KnapsackItem{
                        .id = i + 1,
                        .value = item_pool[indices[i]].first,
                        .weight = item_pool[indices[i]].second
                    });
                }

                if (!consumer(candidate)) return false;

                int idx = k - 1;
                while (idx >= 0 && indices[idx] == pool_size - (k - idx)) {
                    --idx;
                }
                if (idx < 0) {
                    done = true;
                } else {
                    indices[idx]++;
                    for (int j = idx + 1; j < k; ++j) {
                        indices[j] = indices[j - 1] + 1;
                    }
                }
            }
        }
    }
    return true;
}

} // namespace greedybreaker
