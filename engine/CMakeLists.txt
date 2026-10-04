cmake_minimum_required(VERSION 3.20)
project(GreedyBreakerEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.zip
)
FetchContent_MakeAvailable(nlohmann_json)

# Core static library
add_library(greedybreaker_core STATIC
    src/core/metrics.cpp
    src/core/trace.cpp
    src/core/complexity.cpp
    src/core/json_serialization.cpp
    src/problems/coin_change.cpp
    src/problems/activity_selection.cpp
    src/problems/knapsack.cpp
    src/search/candidate_generators.cpp
    src/search/search_config.cpp
    src/search/search_engine.cpp
    src/search/exhaustive_search.cpp
    src/search/backtracking_search.cpp
    src/search/randomized_search.cpp
    src/search/heuristic_search.cpp
)

target_include_directories(greedybreaker_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(greedybreaker_core PUBLIC nlohmann_json::nlohmann_json)

# Standalone CLI executable for engine validation
add_executable(greedybreaker_demo
    src/main.cpp
)
target_link_libraries(greedybreaker_demo PRIVATE greedybreaker_core)

# Authoritative CLI executable for JSON IPC and direct investigation
add_executable(greedybreaker_cli
    src/cli_main.cpp
)
target_link_libraries(greedybreaker_cli PRIVATE greedybreaker_core)

if(WIN32)
    add_custom_command(
        TARGET greedybreaker_demo POST_BUILD
        COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/sign_binary.ps1" "$<TARGET_FILE:greedybreaker_demo>"
        COMMENT "Signing greedybreaker_demo with local developer certificate for Smart App Control"
    )
    add_custom_command(
        TARGET greedybreaker_cli POST_BUILD
        COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/sign_binary.ps1" "$<TARGET_FILE:greedybreaker_cli>"
        COMMENT "Signing greedybreaker_cli with local developer certificate for Smart App Control"
    )
endif()

# Tests
enable_testing()
add_subdirectory(tests)
