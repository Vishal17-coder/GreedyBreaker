#pragma once

#include <string>
#include <string_view>
#include <system_error>

namespace greedybreaker::core {

enum class ErrorCode : int {
    SUCCESS = 0,
    NO_COUNTEREXAMPLE_FOUND = 1,
    SEARCH_INCOMPLETE = 2,
    ORACLE_FAILURE = 3,
    INVALID_CONFIGURATION = 4,
    TIMED_OUT = 5,
    INTERNAL_ERROR = 6
};

[[nodiscard]] constexpr std::string_view to_string(ErrorCode code) noexcept {
    switch (code) {
        case ErrorCode::SUCCESS: return "SUCCESS";
        case ErrorCode::NO_COUNTEREXAMPLE_FOUND: return "NO_COUNTEREXAMPLE_FOUND";
        case ErrorCode::SEARCH_INCOMPLETE: return "SEARCH_INCOMPLETE";
        case ErrorCode::ORACLE_FAILURE: return "ORACLE_FAILURE";
        case ErrorCode::INVALID_CONFIGURATION: return "INVALID_CONFIGURATION";
        case ErrorCode::TIMED_OUT: return "TIMED_OUT";
        case ErrorCode::INTERNAL_ERROR: return "INTERNAL_ERROR";
    }
    return "UNKNOWN_ERROR";
}

class Status {
public:
    constexpr Status() noexcept : code_(ErrorCode::SUCCESS) {}
    Status(ErrorCode code, std::string message)
        : code_(code), message_(std::move(message)) {}

    [[nodiscard]] static Status OK() noexcept { return Status(); }
    [[nodiscard]] static Status NoCounterexampleFound(std::string msg = "No counterexample discovered in configured search space") {
        return Status(ErrorCode::NO_COUNTEREXAMPLE_FOUND, std::move(msg));
    }
    [[nodiscard]] static Status SearchIncomplete(std::string msg) {
        return Status(ErrorCode::SEARCH_INCOMPLETE, std::move(msg));
    }
    [[nodiscard]] static Status OracleFailure(std::string msg) {
        return Status(ErrorCode::ORACLE_FAILURE, std::move(msg));
    }
    [[nodiscard]] static Status InvalidConfiguration(std::string msg) {
        return Status(ErrorCode::INVALID_CONFIGURATION, std::move(msg));
    }
    [[nodiscard]] static Status TimedOut(std::string msg = "Execution timed out") {
        return Status(ErrorCode::TIMED_OUT, std::move(msg));
    }
    [[nodiscard]] static Status InternalError(std::string msg) {
        return Status(ErrorCode::INTERNAL_ERROR, std::move(msg));
    }

    [[nodiscard]] bool ok() const noexcept { return code_ == ErrorCode::SUCCESS; }
    [[nodiscard]] ErrorCode code() const noexcept { return code_; }
    [[nodiscard]] const std::string& message() const noexcept { return message_; }

private:
    ErrorCode code_;
    std::string message_;
};

} // namespace greedybreaker::core
