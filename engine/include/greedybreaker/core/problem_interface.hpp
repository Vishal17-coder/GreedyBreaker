#pragma once

#include "greedybreaker/core/solution.hpp"
#include "greedybreaker/core/types.hpp"
#include "greedybreaker/core/verification_result.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace greedybreaker::core {

class IProblemDescriptor {
public:
    virtual ~IProblemDescriptor() = default;

    [[nodiscard]] virtual std::string problem_id() const = 0;
    [[nodiscard]] virtual std::string display_name() const = 0;
    [[nodiscard]] virtual std::string description() const = 0;
    [[nodiscard]] virtual ObjectiveDirection objective() const = 0;

    [[nodiscard]] virtual std::string default_strategy() const = 0;
    [[nodiscard]] virtual std::vector<std::string> available_strategies() const = 0;

    [[nodiscard]] virtual std::string default_oracle() const = 0;
    [[nodiscard]] virtual std::vector<std::string> available_oracles() const = 0;
};

template <typename InstanceType>
class IInstanceValidator {
public:
    virtual ~IInstanceValidator() = default;
    [[nodiscard]] virtual bool is_valid(const InstanceType& instance, std::string* error_msg = nullptr) const = 0;
};

} // namespace greedybreaker::core
