#pragma once

#include <cstdint>
#include <string_view>
namespace branchtalk::core
{

    struct Version final
    {
        std::uint32_t major;
        std::uint32_t minor;
        std::uint32_t patch;
    };

    inline constexpr Version kVersion{0, 1, 0};
    inline constexpr std::string_view kVersionString{"0.1.0"};

    [[nodiscard]] std::string_view version_string() noexcept;
} // namespace branchtalk::core