#include <branchtalk/core/logging.hpp>

#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

#include <array>
#include <memory>
#include <mutex>
#include <string>

namespace branchtalk::core::logging
{
    namespace
    {
        constexpr std::string_view kLogPattern{"[%l] [%n] %v"};
        constexpr std::string_view kRedacted{"<redacted>"};
        constexpr std::array kCategories{
            LogCategory::client,
            LogCategory::server,
            LogCategory::database,
            LogCategory::network,
        };

        std::mutex logging_mutex;
        LogLevel configured_level{LogLevel::info};

        spdlog::level::level_enum to_spdlog_level(LogLevel level) noexcept
        {
            switch (level)
            {
            case LogLevel::trace:
                return spdlog::level::trace;
            case LogLevel::debug:
                return spdlog::level::debug;
            case LogLevel::info:
                return spdlog::level::info;
            case LogLevel::warning:
                return spdlog::level::warn;
            case LogLevel::error:
                return spdlog::level::err;
            case LogLevel::critical:
                return spdlog::level::critical;
            case LogLevel::off:
                return spdlog::level::off;
            }

            return spdlog::level::off;
        }

        std::shared_ptr<spdlog::logger> logger_for_unlocked(LogCategory category)
        {
            const auto name = std::string{category_name(category)};
            auto logger = spdlog::get(name);

            if (!logger)
            {
                logger = spdlog::stdout_logger_mt(name);
                logger->set_pattern(std::string{kLogPattern});
            }

            logger->set_level(to_spdlog_level(configured_level));
            return logger;
        }

        std::shared_ptr<spdlog::logger> logger_for(LogCategory category)
        {
            const std::scoped_lock lock{logging_mutex};
            return logger_for_unlocked(category);
        }
    } // namespace

    SensitiveValue::SensitiveValue(std::string_view) noexcept {}

    std::string_view SensitiveValue::redacted() const noexcept
    {
        return kRedacted;
    }

    void initialize_logging(LoggingSettings settings)
    {
        const std::scoped_lock lock{logging_mutex};
        configured_level = settings.level;

        for (const auto category : kCategories)
        {
            auto logger = logger_for_unlocked(category);
            logger->set_pattern(std::string{kLogPattern});
            logger->set_level(to_spdlog_level(configured_level));
        }
    }

    std::string_view category_name(LogCategory category) noexcept
    {
        switch (category)
        {
        case LogCategory::client:
            return "client";
        case LogCategory::server:
            return "server";
        case LogCategory::database:
            return "database";
        case LogCategory::network:
            return "network";
        }

        return "unknown";
    }

    bool is_log_enabled(LogCategory category, LogLevel level)
    {
        return logger_for(category)->should_log(to_spdlog_level(level));
    }

    void write_log(LogCategory category, LogLevel level, std::string_view message)
    {
        logger_for(category)->log(to_spdlog_level(level), "{}", message);
    }

    void write_sensitive(LogCategory category,
                         LogLevel level,
                         std::string_view field_name,
                         const SensitiveValue &value)
    {
        logger_for(category)->log(to_spdlog_level(level), "{}={}", field_name, value.redacted());
    }
} // namespace branchtalk::core::logging