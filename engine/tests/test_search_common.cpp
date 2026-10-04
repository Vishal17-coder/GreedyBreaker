#include <gtest/gtest.h>
#include "greedybreaker/search/search_config.hpp"
#include "greedybreaker/search/search_engine.hpp"
#include "greedybreaker/search/exhaustive_search.hpp"

using namespace greedybreaker;

TEST(SearchCommonTest, ValidConfigPasses) {
    SearchConfig config;
    EXPECT_NO_THROW(config.validate());
}

TEST(SearchCommonTest, InvertedDenominationsThrows) {
    SearchConfig config;
    config.min_denomination = 10;
    config.max_denomination = 5;
    EXPECT_THROW(config.validate(), InvalidCandidateException);
}

TEST(SearchCommonTest, ZeroOrNegativeDenominationThrows) {
    SearchConfig config;
    config.min_denomination = 0;
    EXPECT_THROW(config.validate(), InvalidCandidateException);
}

TEST(SearchCommonTest, InvertedCoinCountThrows) {
    SearchConfig config;
    config.min_coin_count = 5;
    config.max_coin_count = 3;
    EXPECT_THROW(config.validate(), InvalidCandidateException);
}

TEST(SearchCommonTest, CoinCountExceedsRangeThrows) {
    SearchConfig config;
    config.min_denomination = 1;
    config.max_denomination = 3; // 3 available
    config.min_coin_count = 2;
    config.max_coin_count = 4; // requires 4 distinct coins
    EXPECT_THROW(config.validate(), InvalidCandidateException);
}

TEST(SearchCommonTest, InvertedTargetThrows) {
    SearchConfig config;
    config.min_target = 50;
    config.max_target = 20;
    EXPECT_THROW(config.validate(), InvalidCandidateException);
}

TEST(SearchCommonTest, CandidateSmallerComparator) {
    CoinChangeCandidate a{{1, 3, 4}, 6};
    CoinChangeCandidate b{{1, 3, 4}, 7}; // larger target
    CoinChangeCandidate c{{1, 2, 3, 4}, 6}; // more coins
    CoinChangeCandidate d{{1, 3, 5}, 6}; // lexicographically larger

    EXPECT_TRUE(is_candidate_smaller(a, b));
    EXPECT_FALSE(is_candidate_smaller(b, a));

    EXPECT_TRUE(is_candidate_smaller(a, c));
    EXPECT_FALSE(is_candidate_smaller(c, a));

    EXPECT_TRUE(is_candidate_smaller(a, d));
    EXPECT_FALSE(is_candidate_smaller(d, a));
}

TEST(SearchCommonTest, SeverityWorseComparator) {
    ComparisonResult high_ratio;
    high_ratio.ratio = 2.0;
    high_ratio.absolute_gap = 2.0;

    ComparisonResult low_ratio;
    low_ratio.ratio = 1.5;
    low_ratio.absolute_gap = 3.0; // larger gap, but lower ratio

    CoinChangeCandidate cand_a{{1, 6, 9}, 12};
    CoinChangeCandidate cand_b{{1, 3, 4}, 6};

    // Primary criterion: ratio 2.0 > 1.5
    EXPECT_TRUE(is_severity_worse(high_ratio, cand_a, low_ratio, cand_b));
    EXPECT_FALSE(is_severity_worse(low_ratio, cand_b, high_ratio, cand_a));

    // Equal ratio, different gap
    ComparisonResult eq1;
    eq1.ratio = 1.5;
    eq1.absolute_gap = 2.0;

    ComparisonResult eq2;
    eq2.ratio = 1.5;
    eq2.absolute_gap = 1.0;

    EXPECT_TRUE(is_severity_worse(eq1, cand_a, eq2, cand_b));
    EXPECT_FALSE(is_severity_worse(eq2, cand_b, eq1, cand_a));
}

TEST(SearchCommonTest, CancellationHookTerminatesEarly) {
    SearchConfig config;
    config.min_denomination = 1;
    config.max_denomination = 10;
    config.min_coin_count = 2;
    config.max_coin_count = 3;
    config.min_target = 1;
    config.max_target = 30;
    config.should_cancel = []() { return true; }; // Cancel immediately

    ExhaustiveSearch engine;
    auto result = engine.search(config);

    EXPECT_FALSE(result.search_completed);
    EXPECT_EQ(result.candidates_evaluated, 0);
    EXPECT_NE(result.termination_reason.find("cancelled"), std::string::npos);
}

TEST(SearchCommonTest, SafetyCandidateLimitEnforced) {
    SearchConfig config;
    config.min_denomination = 1;
    config.max_denomination = 15;
    config.min_coin_count = 2;
    config.max_coin_count = 4;
    config.min_target = 1;
    config.max_target = 50;
    config.max_candidates = 10;
    config.mode = SearchMode::WORST;

    ExhaustiveSearch engine;
    auto result = engine.search(config);

    EXPECT_TRUE(result.search_limit_reached);
    EXPECT_FALSE(result.search_completed);
    EXPECT_LE(result.candidates_evaluated, 10);
    EXPECT_NE(result.termination_reason.find("max candidates"), std::string::npos);
}
