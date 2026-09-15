#pragma once

#include <string_view>

namespace branchtalk::core::logging
{

    enum class LogCategory
    {
        client,
        server,
        database,
        network,
    };

    enum class LogLevel
    {
        trace,
        debug,
        info,
        warning,
        error,
        critical,
        off,
    };

    struct LoggingSettings
    {
        LogLevel level{LogLevel::info};
    };

    class SensitiveValue
    {
    public:
        explicit SensitiveValue(std::string_view) noexcept;

        [[nodiscard]] std::string_view redacted() const noexcept;
    };

    void initialize_logging(LoggingSettings settings);

    [[nodiscard]] std::string_view category_name(LogCategory category) noexcept;
    [[nodiscard]] bool is_log_enabled(LogCategory cateogry, LogLevel level);

    void write_log(LogCategory category, LogLevel level, std::string_view message);
    void write_sensitive(LogCategory category,
                         LogLevel level,
                         std::string_view field_name,
                         const SensitiveValue &value);
} // namespace branchtalk::core::logging