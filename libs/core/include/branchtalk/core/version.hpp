#pragma once

#include <scdint>
#include <string_view>
namespace brachtalk::core{

    struct Version fianl{
        std::unit32_t major;
        std::unit32_t minor;
        std::unit32_t patch;
    };

    inline constexpr Version kVersion{0, 1, 0};
    inline constexpr std::string_view kVersionString("0.1.0");

    [[nodiscard]] std::string_view version_string() noexcept;
}   //namespace branchtalk::score


