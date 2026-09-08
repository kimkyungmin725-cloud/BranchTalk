#include <branchtalk/core/logging.hpp>

#include <string_view>

namespace
{
    bool categories_have_stable_names()
    {
        using branchtalk::core::logging::category_name;
        using branchtalk::core::logging::LogCategory;

        return category_name(LogCategory::client) == "client" &&
               category_name(LogCategory::server) == "server" &&
               category_name(LogCategory::database) == "database" &&
               category_name(LogCategory::network) == "network";
    }

    bool level_setting_can_be_changed()
    {
        using namespace branchtalk::core::logging;

        initialize_logging(LoggingSettings{LogLevel::info});
        const bool info_setting_applied =
            !is_log_enabled(LogCategory::client, LogLevel::debug) &&
            is_log_enabled(LogCategory::client, LogLevel::info);

        initialize_logging(LoggingSettings{LogLevel::debug});
        const bool debug_setting_applied = is_log_enabled(LogCategory::client, LogLevel::debug);

        return info_setting_applied && debug_setting_applied;
    }
} // namespace

int main()
{
    using namespace branchtalk::core::logging;

    initialize_logging(LoggingSettings{LogLevel::debug});
    write_log(LogCategory::client, LogLevel::info, "client category ready");
    write_log(LogCategory::server, LogLevel::info, "server category ready");
    write_log(LogCategory::database, LogLevel::info, "database category ready");
    write_log(LogCategory::network, LogLevel::info, "network category ready");

    constexpr std::string_view kSecret{"database-secret"};
    write_sensitive(LogCategory::database,
                    LogLevel::info,
                    "credential",
                    SensitiveValue{kSecret});

    return categories_have_stable_names() && level_setting_can_be_changed() ? 0 : 1;
}