#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace greedybreaker::core {

struct CandidateChoice {
    std::string identifier;
    double heuristic_score{0.0};
    bool feasible{true};
    std::string notes;
};

struct DecisionStep {
    size_t step_index{0};
    std::string action_description;
    std::string heuristic_rule;
    std::vector<CandidateChoice> candidates_considered;
    std::string state_before;
    std::string state_after;
    double cumulative_objective{0.0};
};

class DecisionTrace {
public:
    void add_step(DecisionStep step) {
        steps_.push_back(std::move(step));
    }

    [[nodiscard]] const std::vector<DecisionStep>& steps() const noexcept { return steps_; }
    [[nodiscard]] size_t size() const noexcept { return steps_.size(); }
    [[nodiscard]] bool empty() const noexcept { return steps_.empty(); }
    void clear() noexcept { steps_.clear(); }

    [[nodiscard]] const DecisionStep& step_at(size_t index) const {
        return steps_.at(index);
    }

private:
    std::vector<DecisionStep> steps_;
};

} // namespace greedybreaker::core
