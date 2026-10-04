cmake_minimum_required(VERSION 3.20)
project(GreedyBreakerV2 VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Warning flags
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include(FetchContent)

# nlohmann/json for robust JSON serialization
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.zip
)
FetchContent_MakeAvailable(nlohmann_json)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Core Static Library
add_library(greedybreaker_core STATIC
    src/core/types.cpp
    src/core/verification_result.cpp
    src/problems/coin_change.cpp
    src/problems/knapsack_01.cpp
    src/problems/activity_selection.cpp
    src/problems/fractional_knapsack.cpp
    src/problems/huffman_coding.cpp
    src/search/search_space.cpp
    src/search/search_config.cpp
    src/search/search_engine.cpp
    src/minimization/minimizer.cpp
    src/forensics/forensics.cpp
)

target_include_directories(greedybreaker_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(greedybreaker_core PUBLIC
    nlohmann_json::nlohmann_json
)

# Standalone CLI Executable for Automated Subprocess Invocation & Testing
add_executable(greedybreaker_cli
    src/main.cpp
)
target_link_libraries(greedybreaker_cli PRIVATE
    greedybreaker_core
)

if(WIN32)
    add_custom_command(
        TARGET greedybreaker_cli POST_BUILD
        COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/sign_binary.ps1" "$<TARGET_FILE:greedybreaker_cli>"
        COMMENT "Signing greedybreaker_cli with local certificate for Smart App Control"
    )
endif()

# Enable Testing
enable_testing()
add_subdirectory(tests)
