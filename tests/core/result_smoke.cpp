#include <branchtalk/core/result.hpp>

#include <string>

namespace
{

    bool success_result_exposes_only_its_value()
    {
        const auto result = branchtalk::core::Result<int>::success(42);
        const auto *value = result.value_if();

        return result.has_value() && static_cast<bool>(result) && value != nullptr && *value == 42 &&
               result.error_if() == nullptr;
    }

    bool failure_result_exposes_code_and_message()
    {
        using branchtalk::core::Error;
        using branchtalk::core::ErrorCode;
        using branchtalk::core::Result;

        const auto result = Result<int>::failure(Error{
            ErrorCode::not_found,
            "requested item was not found",
        });
        const auto *error = result.error_if();

        return !result.has_value() && !static_cast<bool>(result) && result.value_if() == nullptr &&
               error != nullptr && error->code == ErrorCode::not_found &&
               error->message == "requested item was not found";
    }
}

int main()
{
    return success_result_exposes_only_its_value() && failure_result_exposes_code_and_message()
               ? 0
               : 1;
}