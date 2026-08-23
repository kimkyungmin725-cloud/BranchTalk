#include <branchtalk/core/logging.hpp>
#include <branchtalk/core/version.hpp>

namespace branchtalk::client
{
    int run()
    {
        core::logging::initialize_logging({core::logging::LogLevel::info});
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