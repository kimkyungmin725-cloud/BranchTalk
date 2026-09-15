#pragma once

#include <branchtalk/core/application_config.hpp>

namespace branchtalk::server
{

    class ServerApp
    {
    public:
        using StopRequested = bool (*)() noexcept;

        ServerApp(core::ApplicationConfig config, StopRequested stop_requested);

        [[nodiscard]] const core::ApplicationConfig &config() const noexcept;
        int run();

    private:
        core::ApplicationConfig config_;
        StopRequested stop_requested_;
    };
} // namespace branchtalk::server