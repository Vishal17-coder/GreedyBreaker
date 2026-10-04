#pragma once

#include "greedybreaker/search/search_config.hpp"
#include "greedybreaker/search/search_result.hpp"

namespace greedybreaker::search {

class SearchEngine {
public:
    [[nodiscard]] static SearchResult run(const SearchConfig& config);
};

} // namespace greedybreaker::search
