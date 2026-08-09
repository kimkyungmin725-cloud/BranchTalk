#include <branchtalk/core/version.hpp>

namespace branchtalk::core
{
    std::string_view version_string() noexcept
    {
        return kVersionString;
    }
} // namespace branchtalk::core