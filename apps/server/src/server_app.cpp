#include <branchtalk/server/server_app.hpp>

#include <branchtalk/core/logging.hpp>

#include <chrono>
#include <cstdlib>
#include <thread>
#include <utility>

namespace branchtalk::server
{

    ServerApp::ServerApp(core::ApplicationConfig config, StopRequested stop_requested)
        : config_(std::move(config)), stop_requested_(stop_requested) {}

    const core::ApplicationConfig &ServerApp::config() const noexcept
    {
        return config_;
    }

    int ServerApp::run()
    {
        core::logging::initialize_logging({config_.log_level});
        core::logging::write_log(core::logging::LogCategory::server,
                                 core::logging::LogLevel::info,
                                 "server started");

        while (!stop_requested_())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds{25});
        }

        core::logging::write_log(core::logging::LogCategory::server,
                                 core::logging::LogLevel::info,
                                 "server stopped");

        return EXIT_SUCCESS;
    }
} // namespace branchtalk::server