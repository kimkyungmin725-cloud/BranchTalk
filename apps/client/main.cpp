#include <branchtalk/core/application_config.hpp>
#include <branchtalk/core/logging.hpp>
#include <branchtalk/core/version.hpp>

namespace branchtalk::client
{

    int run()
    {
        const core::ApplicationConfig config;
        core::logging::initialize_logging({config.log_level});
        core::logging::write_log(core::logging::LogCategory::client,
                                 core::logging::LogLevel::info,
                                 "application started");
        return core::version_string().empty() ? 1 : 0;
    }

} // namespace branchtalk::client

int main()
{
    return branchtalk::client::run();
}
