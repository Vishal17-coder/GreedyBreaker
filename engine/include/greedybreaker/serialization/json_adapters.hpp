#pragma once

#include "greedybreaker/core/error.hpp"
#include "greedybreaker/core/solution.hpp"
#include "greedybreaker/core/trace.hpp"
#include "greedybreaker/core/types.hpp"
#include "greedybreaker/core/verification_result.hpp"
#include <nlohmann/json.hpp>

namespace greedybreaker::core {

// ObjectiveDirection
inline void to_json(nlohmann::json& j, const ObjectiveDirection& dir) {
    j = std::string(to_string(dir));
}

inline void from_json(const nlohmann::json& j, ObjectiveDirection& dir) {
    std::string s = j.get<std::string>();
    if (s == "MAXIMIZE") {
        dir = ObjectiveDirection::MAXIMIZE;
    } else {
        dir = ObjectiveDirection::MINIMIZE;
    }
}

// VerificationStatus
inline void to_json(nlohmann::json& j, const VerificationStatus& status) {
    j = std::string(to_string(status));
}

inline void from_json(const nlohmann::json& j, VerificationStatus& status) {
    std::string s = j.get<std::string>();
    if (s == "FEASIBILITY_FAILURE") {
        status = VerificationStatus::FEASIBILITY_FAILURE;
    } else if (s == "OPTIMALITY_FAILURE") {
        status = VerificationStatus::OPTIMALITY_FAILURE;
    } else {
        status = VerificationStatus::OPTIMAL;
    }
}

// OracleConfidence
inline void to_json(nlohmann::json& j, const OracleConfidence& conf) {
    j = std::string(to_string(conf));
}

inline void from_json(const nlohmann::json& j, OracleConfidence& conf) {
    std::string s = j.get<std::string>();
    if (s == "APPROXIMATION") {
        conf = OracleConfidence::APPROXIMATION;
    } else if (s == "HEURISTIC") {
        conf = OracleConfidence::HEURISTIC;
    } else {
        conf = OracleConfidence::EXACT;
    }
}

// ExecutionMetrics
inline void to_json(nlohmann::json& j, const ExecutionMetrics& m) {
    j = nlohmann::json{
        {"execution_time_ns", m.execution_time_ns},
        {"execution_time_ms", m.execution_time_ms()},
        {"states_evaluated", m.states_evaluated},
        {"memory_bytes_peak", m.memory_bytes_peak}
    };
}

inline void from_json(const nlohmann::json& j, ExecutionMetrics& m) {
    m.execution_time_ns = j.value("execution_time_ns", uint64_t{0});
    m.states_evaluated = j.value("states_evaluated", uint64_t{0});
    m.memory_bytes_peak = j.value("memory_bytes_peak", uint64_t{0});
}

// CandidateChoice
inline void to_json(nlohmann::json& j, const CandidateChoice& c) {
    j = nlohmann::json{
        {"identifier", c.identifier},
        {"heuristic_score", c.heuristic_score},
        {"feasible", c.feasible},
        {"notes", c.notes}
    };
}

inline void from_json(const nlohmann::json& j, CandidateChoice& c) {
    c.identifier = j.value("identifier", "");
    c.heuristic_score = j.value("heuristic_score", 0.0);
    c.feasible = j.value("feasible", true);
    c.notes = j.value("notes", "");
}

// DecisionStep
inline void to_json(nlohmann::json& j, const DecisionStep& s) {
    j = nlohmann::json{
        {"step_index", s.step_index},
        {"action_description", s.action_description},
        {"heuristic_rule", s.heuristic_rule},
        {"candidates_considered", s.candidates_considered},
        {"state_before", s.state_before},
        {"state_after", s.state_after},
        {"cumulative_objective", s.cumulative_objective}
    };
}

inline void from_json(const nlohmann::json& j, DecisionStep& s) {
    s.step_index = j.value("step_index", size_t{0});
    s.action_description = j.value("action_description", "");
    s.heuristic_rule = j.value("heuristic_rule", "");
    if (j.contains("candidates_considered")) {
        s.candidates_considered = j.at("candidates_considered").get<std::vector<CandidateChoice>>();
    }
    s.state_before = j.value("state_before", "");
    s.state_after = j.value("state_after", "");
    s.cumulative_objective = j.value("cumulative_objective", 0.0);
}

// DecisionTrace
inline void to_json(nlohmann::json& j, const DecisionTrace& t) {
    j = nlohmann::json{
        {"steps", t.steps()},
        {"total_steps", t.size()}
    };
}

inline void from_json(const nlohmann::json& j, DecisionTrace& t) {
    t.clear();
    if (j.contains("steps")) {
        for (const auto& step_json : j.at("steps")) {
            t.add_step(step_json.get<DecisionStep>());
        }
    }
}

// GreedyResult
inline void to_json(nlohmann::json& j, const GreedyResult& r) {
    j = nlohmann::json{
        {"feasible", r.feasible},
        {"objective_value", r.objective_value},
        {"solution_description", r.solution_description},
        {"items_selected", r.items_selected},
        {"trace", r.trace},
        {"metrics", r.metrics}
    };
}

inline void from_json(const nlohmann::json& j, GreedyResult& r) {
    r.feasible = j.value("feasible", false);
    r.objective_value = j.value("objective_value", 0.0);
    r.solution_description = j.value("solution_description", "");
    if (j.contains("items_selected")) {
        r.items_selected = j.at("items_selected").get<std::vector<int64_t>>();
    }
    if (j.contains("trace")) {
        r.trace = j.at("trace").get<DecisionTrace>();
    }
    if (j.contains("metrics")) {
        r.metrics = j.at("metrics").get<ExecutionMetrics>();
    }
}

// OracleResult
inline void to_json(nlohmann::json& j, const OracleResult& r) {
    j = nlohmann::json{
        {"confidence", r.confidence},
        {"optimal_value", r.optimal_value},
        {"solution_description", r.solution_description},
        {"items_selected", r.items_selected},
        {"oracle_method", r.oracle_method},
        {"metrics", r.metrics}
    };
}

inline void from_json(const nlohmann::json& j, OracleResult& r) {
    r.confidence = j.value("confidence", OracleConfidence::EXACT);
    r.optimal_value = j.value("optimal_value", 0.0);
    r.solution_description = j.value("solution_description", "");
    if (j.contains("items_selected")) {
        r.items_selected = j.at("items_selected").get<std::vector<int64_t>>();
    }
    r.oracle_method = j.value("oracle_method", "");
    if (j.contains("metrics")) {
        r.metrics = j.at("metrics").get<ExecutionMetrics>();
    }
}

// VerificationResult
inline void to_json(nlohmann::json& j, const VerificationResult& r) {
    j = nlohmann::json{
        {"status", r.status},
        {"is_counterexample", r.is_counterexample},
        {"greedy_value", r.greedy_value},
        {"optimal_value", r.optimal_value},
        {"absolute_gap", r.absolute_gap},
        {"relative_gap", r.relative_gap},
        {"failure_percent", r.failure_percent},
        {"summary", r.summary}
    };
}

inline void from_json(const nlohmann::json& j, VerificationResult& r) {
    r.status = j.value("status", VerificationStatus::OPTIMAL);
    r.is_counterexample = j.value("is_counterexample", false);
    r.greedy_value = j.value("greedy_value", 0.0);
    r.optimal_value = j.value("optimal_value", 0.0);
    r.absolute_gap = j.value("absolute_gap", 0.0);
    r.relative_gap = j.value("relative_gap", 1.0);
    r.failure_percent = j.value("failure_percent", 0.0);
    r.summary = j.value("summary", "");
}

} // namespace greedybreaker::core
