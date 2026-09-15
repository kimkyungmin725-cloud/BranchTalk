#include <branchtalk/core/application_config.hpp>
#include <branchtalk/core/error.hpp>
#include <branchtalk/core/logging.hpp>
#include <branchtalk/core/result.hpp>
#include <branchtalk/server/server_app.hpp>

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

namespace branchtalk::server
{
    namespace
    {
        struct ServerOptions
        {
            core::ApplicationConfig config;
            bool smoke_test{false};
        };

        std::atomic_flag shutdown_requested;

        void handle_shutdown_signal(int)
        {
            shutdown_requested.test_and_set(std::memory_order_relaxed);
        }

        bool is_shutdown_requested() noexcept
        {
            return shutdown_requested.test(std::memory_order_relaxed);
        }

        class SignalHandlers
        {
        public:
            SignalHandlers() = default;
            SignalHandlers(const SignalHandlers &) = delete;
            SignalHandlers &operator=(const SignalHandlers &) = delete;

            ~SignalHandlers()
            {
                if (installed_)
                {
                    static_cast<void>(std::signal(SIGINT, previous_interrupt_));
                    static_cast<void>(std::signal(SIGTERM, previous_terminate_));
                }
            }

            [[nodiscard]] bool install() noexcept
            {
                previous_interrupt_ = std::signal(SIGINT, handle_shutdown_signal);
                if (previous_interrupt_ == SIG_ERR)
                {
                    return false;
                }

                previous_terminate_ = std::signal(SIGTERM, handle_shutdown_signal);
                if (previous_terminate_ == SIG_ERR)
                {
                    static_cast<void>(std::signal(SIGINT, previous_interrupt_));
                    return false;
                }

                installed_ = true;
                return true;
            }

        private:
            using SignalHandler = void (*)(int);

            SignalHandler previous_interrupt_{SIG_DFL};
            SignalHandler previous_terminate_{SIG_DFL};
            bool installed_{false};
        };

        core::Result<ServerOptions> parse_options(int argc, char *argv[])
        {
            if (argc == 1)
            {
                return core::Result<ServerOptions>::success(ServerOptions{});
            }

            if (argc != 2)
            {
                return core::Result<ServerOptions>::failure(core::Error{
                    core::ErrorCode::invalid_argument,
                    "usage: branchtalk_server [config.json | --smoke-test]",
                });
            }

            if (std::string_view{argv[1]} == "--smoke-test")
            {
                ServerOptions options;
                options.smoke_test = true;
                return core::Result<ServerOptions>::success(std::move(options));
            }

            auto config_result = core::load_application_config(argv[1]);
            if (auto *config = config_result.value_if())
            {
                return core::Result<ServerOptions>::success(ServerOptions{
                    std::move(*config),
                    false,
                });
            }

            return core::Result<ServerOptions>::failure(std::move(*config_result.error_if()));
        }

        void write_server_error(std::string_view message)
        {
            core::logging::initialize_logging({});
            const std::string log_message = "server startup error: " + std::string{message};
            core::logging::write_log(core::logging::LogCategory::server,
                                     core::logging::LogLevel::error,
                                     log_message);
        }
    } // namespace

    int run(int argc, char *argv[])
    {
        auto options_result = parse_options(argc, argv);
        auto *options = options_result.value_if();
        if (options == nullptr)
        {
            write_server_error(options_result.error_if()->message);
            return EXIT_FAILURE;
        }

        shutdown_requested.clear(std::memory_order_relaxed);

        SignalHandlers signal_handlers;
        if (!signal_handlers.install())
        {
            write_server_error("could not install shutdown signal handlers");
            return EXIT_FAILURE;
        }

        ServerApp application{std::move(options->config), is_shutdown_requested};
        std::jthread smoke_shutdown;
        if (options->smoke_test)
        {
            smoke_shutdown = std::jthread([]
                                          {
                std::this_thread::sleep_for(std::chrono::milliseconds{25});
                if (std::raise(SIGTERM) != 0) {
                    shutdown_requested.test_and_set(std::memory_order_relaxed);
                } });
        }

        return application.run();
    }

} // namespace branchtalk::server

int main(int argc, char *argv[])
{
    return branchtalk::server::run(argc, argv);
}