#pragma once

#include <branchtalk/core/logging.hpp>
#include <branchtalk/core/result.hpp>

#include <filesystem>
#include <string>

namespace branchtalk::core
{

    struct ApplicationConfig
    {
        std::string server_address{"127.0.0.1:8080"};
        logging::LogLevel log_level{logging::LogLevel::info};
        std::filesystem::path data_path{"data"};
    };

    [[nodiscard]] Result<ApplicationConfig> load_application_config(
        const std::filesystem::path &file_path);

} // namespace branchtalk::core