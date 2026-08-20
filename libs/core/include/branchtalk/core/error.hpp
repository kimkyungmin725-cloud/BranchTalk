#pragma

#include <string>

namespace branchtalk::core {

    enum class ErrorCode {
        invalid_argument,
        not_found,
        conflict,
        operation_failed,
    };

    struct Error
    {
        ErrorCode code;
        std::string message;
    };

} //namespace branchtalk::core