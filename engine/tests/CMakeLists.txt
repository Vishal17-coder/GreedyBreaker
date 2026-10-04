include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(greedybreaker_tests
    test_metrics.cpp
    test_traces.cpp
    test_coin_change.cpp
    test_search_common.cpp
    test_exhaustive_search.cpp
    test_backtracking_search.cpp
    test_randomized_search.cpp
    test_heuristic_search.cpp
    test_activity_selection.cpp
    test_knapsack_01.cpp
    test_knapsack_fractional.cpp
    test_problem_registry.cpp
    test_json_cli.cpp
)

target_link_libraries(greedybreaker_tests PRIVATE
    greedybreaker_core
    GTest::gtest_main
)

if(WIN32)
    add_custom_command(
        TARGET greedybreaker_tests POST_BUILD
        COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_SOURCE_DIR}/scripts/sign_binary.ps1" "$<TARGET_FILE:greedybreaker_tests>"
        COMMENT "Signing greedybreaker_tests with local developer certificate for Smart App Control"
    )
endif()

include(GoogleTest)
gtest_discover_tests(greedybreaker_tests
    DISCOVERY_MODE PRE_TEST
)
