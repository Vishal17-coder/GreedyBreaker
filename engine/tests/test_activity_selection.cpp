#include <gtest/gtest.h>
#include "greedybreaker/problems/activity_selection.hpp"
#include "greedybreaker/search/exhaustive_search.hpp"
#include "greedybreaker/search/backtracking_search.hpp"

using namespace greedybreaker;

class ActivitySelectionTest : public ::testing::Test {
protected:
    ActivitySelectionProblem problem;
};

TEST_F(ActivitySelectionTest, ValidCandidatePasses) {
    ActivitySelectionCandidate candidate{
        .activities = {
            Activity{.id = 1, .start = 1, .finish = 4},
            Activity{.id = 2, .start = 3, .finish = 5},
            Activity{.id = 3, .start = 0, .finish = 6}
        }
    };
    EXPECT_NO_THROW(problem.validate_candidate(candidate));
}

TEST_F(ActivitySelectionTest, EmptyActivitiesThrows) {
    ActivitySelectionCandidate candidate{.activities = {}};
    EXPECT_THROW(problem.validate_candidate(candidate), InvalidCandidateException);
}

TEST_F(ActivitySelectionTest, InvalidIntervalThrows) {
    ActivitySelectionCandidate candidate{
        .activities = {
            Activity{.id = 1, .start = 5, .finish = 5} // start == finish
        }
    };
    EXPECT_THROW(problem.validate_candidate(candidate), InvalidCandidateException);

    ActivitySelectionCandidate inverted{
        .activities = {
            Activity{.id = 1, .start = 7, .finish = 3} // start > finish
        }
    };
    EXPECT_THROW(problem.validate_candidate(inverted), InvalidCandidateException);
}

TEST_F(ActivitySelectionTest, NegativeTimesThrows) {
    ActivitySelectionCandidate candidate{
        .activities = {
            Activity{.id = 1, .start = -2, .finish = 4}
        }
    };
    EXPECT_THROW(problem.validate_candidate(candidate), InvalidCandidateException);
}

TEST_F(ActivitySelectionTest, DuplicateIdsThrows) {
    ActivitySelectionCandidate candidate{
        .activities = {
            Activity{.id = 1, .start = 1, .finish = 3},
            Activity{.id = 1, .start = 4, .finish = 6}
        }
    };
    EXPECT_THROW(problem.validate_candidate(candidate), InvalidCandidateException);
}

TEST_F(ActivitySelectionTest, CLRSStandardInstanceMatchesKnownOptimal) {
    // Canonical CLRS Example (Section 16.1):
    // Activities:
    // a1: [1, 4], a2: [3, 5], a3: [0, 6], a4: [5, 7], a5: [3, 9], a6: [5, 9],
    // a7: [6, 10], a8: [8, 11], a9: [8, 12], a10: [2, 14], a11: [12, 16]
    ActivitySelectionCandidate candidate{
        .activities = {
            Activity{.id = 1, .start = 1, .finish = 4},
            Activity{.id = 2, .start = 3, .finish = 5},
            Activity{.id = 3, .start = 0, .finish = 6},
            Activity{.id = 4, .start = 5, .finish = 7},
            Activity{.id = 5, .start = 3, .finish = 9},
            Activity{.id = 6, .start = 5, .finish = 9},
            Activity{.id = 7, .start = 6, .finish = 10},
            Activity{.id = 8, .start = 8, .finish = 11},
            Activity{.id = 9, .start = 8, .finish = 12},
            Activity{.id = 10, .start = 2, .finish = 14},
            Activity{.id = 11, .start = 12, .finish = 16}
        }
    };

    auto greedy_sol = problem.greedy_solution(candidate);
    auto optimal_sol = problem.optimal_solution(candidate);

    ASSERT_TRUE(greedy_sol.success);
    ASSERT_TRUE(optimal_sol.success);

    // Both must achieve the optimal maximum count of 4 compatible activities
    EXPECT_EQ(greedy_sol.cost.value(), 4.0);
    EXPECT_EQ(optimal_sol.cost.value(), 4.0);

    // Verify non-overlapping invariant in greedy solution
    for (size_t i = 1; i < greedy_sol.items.size(); ++i) {
        EXPECT_GE(greedy_sol.items[i].start, greedy_sol.items[i - 1].finish);
    }

    // Comparison result must indicate zero counterexample
    auto cmp = problem.compare_solutions(greedy_sol, optimal_sol);
    EXPECT_FALSE(cmp.is_counterexample);
    EXPECT_DOUBLE_EQ(cmp.absolute_gap.value(), 0.0);
    EXPECT_DOUBLE_EQ(cmp.ratio.value(), 1.0);
}

TEST_F(ActivitySelectionTest, TraceIntegrityAndRationales) {
    ActivitySelectionCandidate candidate{
        .activities = {
            Activity{.id = 1, .start = 1, .finish = 3},
            Activity{.id = 2, .start = 2, .finish = 5},
            Activity{.id = 3, .start = 4, .finish = 7}
        }
    };

    auto sol = problem.greedy_solution(candidate);
    EXPECT_EQ(sol.trace.steps.size(), 3);
    EXPECT_EQ(sol.trace.steps[0].selected_choice.value(), 1); // Selected
    EXPECT_FALSE(sol.trace.steps[1].selected_choice.has_value()); // Skipped (conflict)
    EXPECT_EQ(sol.trace.steps[2].selected_choice.value(), 3); // Selected
}

TEST_F(ActivitySelectionTest, ExhaustiveSearchFindsZeroCounterexamples) {
    ExhaustiveSearch engine;
    SearchConfig config;
    config.problem_type = ProblemType::ACTIVITY_SELECTION;
    config.min_activities = 3;
    config.max_activities = 3;
    config.min_time = 0;
    config.max_time = 6;
    config.min_duration = 1;

    SearchResult result = engine.search(config);
    EXPECT_TRUE(result.search_completed);
    EXPECT_FALSE(result.found);
    EXPECT_EQ(result.counterexamples_found, 0);
    EXPECT_GT(result.candidates_evaluated, 100);
}

TEST_F(ActivitySelectionTest, BacktrackingSearchFindsZeroCounterexamples) {
    BacktrackingSearch engine;
    SearchConfig config;
    config.problem_type = ProblemType::ACTIVITY_SELECTION;
    config.min_activities = 3;
    config.max_activities = 3;
    config.min_time = 0;
    config.max_time = 6;
    config.min_duration = 1;

    SearchResult result = engine.search(config);
    EXPECT_TRUE(result.search_completed);
    EXPECT_FALSE(result.found);
    EXPECT_EQ(result.counterexamples_found, 0);
    EXPECT_GT(result.candidates_evaluated, 100);
}

TEST_F(ActivitySelectionTest, SingleActivityAlwaysOptimal) {
    ActivitySelectionCandidate candidate{
        .activities = {
            Activity{.id = 1, .start = 2, .finish = 6}
        }
    };

    auto greedy_sol = problem.greedy_solution(candidate);
    auto optimal_sol = problem.optimal_solution(candidate);

    ASSERT_TRUE(greedy_sol.success);
    ASSERT_TRUE(optimal_sol.success);
    EXPECT_DOUBLE_EQ(greedy_sol.cost.value(), 1.0);
    EXPECT_DOUBLE_EQ(optimal_sol.cost.value(), 1.0);

    auto cmp = problem.compare_solutions(greedy_sol, optimal_sol);
    EXPECT_FALSE(cmp.is_counterexample);
    EXPECT_DOUBLE_EQ(cmp.absolute_gap.value(), 0.0);
    EXPECT_DOUBLE_EQ(cmp.ratio.value(), 1.0);
}

TEST_F(ActivitySelectionTest, AllOverlappingActivitiesPickExactlyOne) {
    // 4 activities that all mutually overlap at t = 5
    ActivitySelectionCandidate candidate{
        .activities = {
            Activity{.id = 1, .start = 1, .finish = 6},
            Activity{.id = 2, .start = 2, .finish = 7},
            Activity{.id = 3, .start = 3, .finish = 8},
            Activity{.id = 4, .start = 4, .finish = 9}
        }
    };

    auto greedy_sol = problem.greedy_solution(candidate);
    auto optimal_sol = problem.optimal_solution(candidate);

    ASSERT_TRUE(greedy_sol.success);
    ASSERT_TRUE(optimal_sol.success);
    EXPECT_DOUBLE_EQ(greedy_sol.cost.value(), 1.0);
    EXPECT_DOUBLE_EQ(optimal_sol.cost.value(), 1.0);
    EXPECT_EQ(greedy_sol.items.size(), 1);
    EXPECT_EQ(greedy_sol.items[0].id, 1); // Earliest finish (finish = 6)

    auto cmp = problem.compare_solutions(greedy_sol, optimal_sol);
    EXPECT_FALSE(cmp.is_counterexample);
    EXPECT_DOUBLE_EQ(cmp.absolute_gap.value(), 0.0);
}

TEST_F(ActivitySelectionTest, IdenticalFinishTimesTieBreaker) {
    // Two activities finishing at 5:
    // a1: [2, 5], a2: [0, 5], and a3: [5, 8]
    ActivitySelectionCandidate candidate{
        .activities = {
            Activity{.id = 1, .start = 2, .finish = 5},
            Activity{.id = 2, .start = 0, .finish = 5},
            Activity{.id = 3, .start = 5, .finish = 8}
        }
    };

    auto greedy_sol = problem.greedy_solution(candidate);
    auto optimal_sol = problem.optimal_solution(candidate);

    ASSERT_TRUE(greedy_sol.success);
    ASSERT_TRUE(optimal_sol.success);
    EXPECT_DOUBLE_EQ(greedy_sol.cost.value(), 2.0);
    EXPECT_DOUBLE_EQ(optimal_sol.cost.value(), 2.0);

    // Tie-breaker sorts finish ASC then start ASC: a2 [0, 5] picked first, then a3 [5, 8]
    ASSERT_EQ(greedy_sol.items.size(), 2);
    EXPECT_EQ(greedy_sol.items[0].id, 2);
    EXPECT_EQ(greedy_sol.items[1].id, 3);

    auto cmp = problem.compare_solutions(greedy_sol, optimal_sol);
    EXPECT_FALSE(cmp.is_counterexample);
}

