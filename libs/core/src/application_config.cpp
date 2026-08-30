#include <branchtalk/core/application_config.hpp>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace branchtalk::core
{
    namespace
    {

        using Json = nlohmann::json;

        constexpr std::array<std::string_view, 3> kSupportedFields{
            "server_address",
            "log_level",
            "data_path",
        };

        Result<ApplicationConfig> invalid_config(std::string message)
        {
            return Result<ApplicationConfig>::failure(Error{
                ErrorCode::invalid_argument,
                std::move(message),
            });
        }

        bool has_non_whitespace(std::string_view value)
        {
            return std::any_of(value.begin(), value.end(), [](unsigned char character)
                               { return std::isspace(character) == 0; });
        }

        std::optional<logging::LogLevel> parse_log_level(std::string_view value)
        {
            using logging::LogLevel;

            constexpr std::array levels{
                std::pair{std::string_view{"trace"}, LogLevel::trace},
                std::pair{std::string_view{"debug"}, LogLevel::debug},
                std::pair{std::string_view{"info"}, LogLevel::info},
                std::pair{std::string_view{"warning"}, LogLevel::warning},
                std::pair{std::string_view{"error"}, LogLevel::error},
                std::pair{std::string_view{"critical"}, LogLevel::critical},
                std::pair{std::string_view{"off"}, LogLevel::off},
            };

            const auto match = std::find_if(levels.begin(), levels.end(), [value](const auto &item)
                                            { return item.first == value; });
            return match == levels.end() ? std::nullopt
                                         : std::optional<LogLevel>{match->second};
        }

        Result<ApplicationConfig> parse_application_config(const Json &document)
        {
            if (!document.is_object())
            {
                return invalid_config("configuration root must be a JSON object");
            }

            for (const auto &[field_name, field_value] : document.items())
            {
                static_cast<void>(field_value);
                if (std::find(kSupportedFields.begin(), kSupportedFields.end(), field_name) ==
                    kSupportedFields.end())
                {
                    return invalid_config("unsupported configuration field: " + field_name);
                }
            }

            ApplicationConfig config;

            if (const auto server = document.find("server_address"); server != document.end())
            {
                if (!server->is_string())
                {
                    return invalid_config("server_address must be a string");
                }

                config.server_address = server->get<std::string>();
                if (!has_non_whitespace(config.server_address))
                {
                    return invalid_config("server_address must not be empty");
                }
            }

            if (const auto level = document.find("log_level"); level != document.end())
            {
                if (!level->is_string())
                {
                    return invalid_config("log_level must be a string");
                }

                const auto parsed_level = parse_log_level(level->get_ref<const std::string &>());
                if (!parsed_level)
                {
                    return invalid_config(
                        "log_level must be one of: trace, debug, info, warning, error, critical, off");
                }
                config.log_level = *parsed_level;
            }

            if (const auto path = document.find("data_path"); path != document.end())
            {
                if (!path->is_string())
                {
                    return invalid_config("data_path must be a string");
                }

                const auto path_text = path->get<std::string>();
                if (!has_non_whitespace(path_text))
                {
                    return invalid_config("data_path must not be empty");
                }
                config.data_path = path_text;
            }

            return Result<ApplicationConfig>::success(std::move(config));
        }

    } // namespace

    Result<ApplicationConfig> load_application_config(const std::filesystem::path &file_path)
    {
        std::ifstream input{file_path};
        if (!input.is_open())
        {
            std::error_code exists_error;
            const bool exists = std::filesystem::exists(file_path, exists_error);
            const auto code = !exists_error && !exists ? ErrorCode::not_found
                                                       : ErrorCode::operation_failed;
            const auto reason = code == ErrorCode::not_found ? "configuration file was not found: "
                                                             : "configuration file could not be opened: ";
            return Result<ApplicationConfig>::failure(Error{
                code,
                reason + file_path.string(),
            });
        }

        Json document;
        try
        {
            input >> document;
        }
        catch (const Json::parse_error &error)
        {
            return invalid_config(std::string{"configuration JSON is invalid: "} + error.what());
        }

        return parse_application_config(document);
    }

} // namespace branchtalk::core