#include <branchtalk/server/server_app.hpp>

#include <branchtalk/core/logging.hpp>

#include <cstdlib>
#include <filesystem>
#include <string>

namespace
{

    int stop_poll_cout = 0;

    bool stop_after_first_wait() noexcept
    {
        stop_poll_cout++;
        return stop_poll_cout > 1;
    }

} // namespace

int main()
{
    branchtalk::core::ApplicationConfig config;
    config.server_address = "server.test:9000";
    config.log_level = branchtalk::core::logging::LogLevel::debug;
    config.data_path = std::filesystem::path{"server-test-data"};

    branchtalk::server::ServerApp application{config, stop_after_first_wait};
    const auto &injected_config = application.config();
    const bool config_is_injected = injected_config.server_address == config.server_address &&
                                    injected_config.log_level == config.log_level &&
                                    injected_config.data_path == config.data_path;

    return config_is_injected && application.run() == EXIT_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}