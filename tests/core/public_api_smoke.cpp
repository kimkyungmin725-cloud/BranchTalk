#include <branchtalk/core/application_config.hpp>
#include <branchtalk/core/dependency_versions.hpp>
#include <branchtalk/core/error.hpp>
#include <branchtalk/core/logging.hpp>
#include <branchtalk/core/result.hpp>
#include <branchtalk/core/version.hpp>

#include <filesystem>
#include <utility>

int main()
{
    using branchtalk::core::ApplicationConfig;
    using branchtalk::core::Error;
    using branchtalk::core::ErrorCode;
    using branchtalk::core::Result;
    using branchtalk::core::logging::LogCategory;
    using branchtalk::core::logging::LogLevel;

    ApplicationConfig config;
    config.data_path = std::filesystem::path{"foundation-smoke-data"};

    auto result = Result<ApplicationConfig>::success(std::move(config));
    const auto *value = result.value_if();
    const Error sample_error{ErrorCode::operation_failed, "foundation smoke failure"};

    branchtalk::core::logging::initialize_logging({LogLevel::info});

    const bool public_contracts_are_ready =
        value != nullptr && value->server_address == "127.0.0.1:8080" &&
        value->data_path == std::filesystem::path{"foundation-smoke-data"} &&
        sample_error.code == ErrorCode::operation_failed && !sample_error.message.empty() &&
        !branchtalk::core::version_string().empty() &&
        !branchtalk::core::dependency_versions_json().empty() &&
        branchtalk::core::logging::is_log_enabled(LogCategory::client, LogLevel::info);

    return public_contracts_are_ready ? 0 : 1;
}