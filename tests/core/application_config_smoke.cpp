#include <branchtalk/core/application_config.hpp>

#include <array>
#include <filesystem>
#include <string>
#include <string_view>
#include <utility>

namespace
{

    using branchtalk::core::ApplicationConfig;
    using branchtalk::core::ErrorCode;
    using branchtalk::core::load_application_config;
    using branchtalk::core::logging::LogLevel;

    bool default_config_is_ready_to_use()
    {
        const ApplicationConfig config;
        return config.server_address == "127.0.0.1:8080" && config.log_level == LogLevel::info &&
               config.data_path == std::filesystem::path{"data"};
    }

    bool valid_file_overrides_defaults(const std::filesystem::path &fixture_directory)
    {
        const auto result = load_application_config(fixture_directory / "valid.json");
        const auto *config = result.value_if();

        return config != nullptr && config->server_address == "chat.example.test:4242" &&
               config->log_level == LogLevel::debug &&
               config->data_path == std::filesystem::path{"var/branchtalk"};
    }

    bool partial_file_preserves_other_defaults(const std::filesystem::path &fixture_directory)
    {
        const auto result = load_application_config(fixture_directory / "partial.json");
        const auto *config = result.value_if();

        return config != nullptr && config->server_address == "127.0.0.1:8080" &&
               config->log_level == LogLevel::warning &&
               config->data_path == std::filesystem::path{"data"};
    }

    bool failure_mentions(const std::filesystem::path &file_path,
                          ErrorCode expected_code,
                          std::string_view expected_text)
    {
        const auto result = load_application_config(file_path);
        const auto *error = result.error_if();

        return error != nullptr && error->code == expected_code &&
               error->message.find(expected_text) != std::string::npos;
    }

    bool invalid_files_return_clear_errors(const std::filesystem::path &fixture_directory)
    {
        constexpr std::array cases{
            std::pair{std::string_view{"invalid-server-address.json"},
                      std::string_view{"server_address"}},
            std::pair{std::string_view{"invalid-log-level.json"}, std::string_view{"log_level"}},
            std::pair{std::string_view{"invalid-data-path.json"}, std::string_view{"data_path"}},
            std::pair{std::string_view{"unknown-field.json"}, std::string_view{"unexpected"}},
            std::pair{std::string_view{"invalid-json.json"}, std::string_view{"JSON"}},
        };

        for (const auto &[file_name, field_name] : cases)
        {
            if (!failure_mentions(fixture_directory / file_name,
                                  ErrorCode::invalid_argument,
                                  field_name))
            {
                return false;
            }
        }

        return true;
    }

    bool missing_file_is_distinguishable(const std::filesystem::path &fixture_directory)
    {
        return failure_mentions(fixture_directory / "missing.json",
                                ErrorCode::not_found,
                                "not found");
    }
} // namespace

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        return 2;
    }

    const std::filesystem::path fixture_directory{argv[1]};
    return default_config_is_ready_to_use() && valid_file_overrides_defaults(fixture_directory) &&
                   partial_file_preserves_other_defaults(fixture_directory) &&
                   invalid_files_return_clear_errors(fixture_directory) &&
                   missing_file_is_distinguishable(fixture_directory)
               ? 0
               : 1;
}