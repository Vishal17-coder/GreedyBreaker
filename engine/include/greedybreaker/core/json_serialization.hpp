#pragma once

#include "greedybreaker/search/search_config.hpp"
#include "greedybreaker/search/search_result.hpp"
#include "greedybreaker/core/registry.hpp"
#include "greedybreaker/core/complexity.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace greedybreaker {

// Configuration serialization / deserialization
nlohmann::json to_json(const SearchConfig& config);
SearchConfig search_config_from_json(const nlohmann::json& j);

// Trace and metrics serialization
nlohmann::json to_json(const TraceStep& step);
nlohmann::json to_json(const AlgorithmTrace& trace);
nlohmann::json to_json(const ComparisonResult& cmp);
nlohmann::json to_json(const DivergenceResult& div);

// Candidate serialization
nlohmann::json to_json(const CoinChangeCandidate& cand);
nlohmann::json to_json(const ActivitySelectionCandidate& cand);
nlohmann::json to_json(const KnapsackCandidate& cand);

// Full SearchResult serialization
nlohmann::json to_json(const SearchResult& result, const SearchConfig& config);

// Error response generation
nlohmann::json make_error_json(const std::string& code, const std::string& message);

// Problem registry metadata serialization
nlohmann::json problem_registry_to_json();

// Direct single-candidate verification
nlohmann::json verify_candidate_json(const nlohmann::json& input_json);

} // namespace greedybreaker
