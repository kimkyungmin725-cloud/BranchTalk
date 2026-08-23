#include <branchtalk/core/logging.hpp>
#include <branchtalk/core/version.hpp>

namespace branchtalk::server
{
    int run()
    {
        core::logging::initialize_logging({core::logging::LogLevel::info});
        core::logging::write_log(core::logging::LogCategory::server,
                                 core::logging::LogLevel::info,
                                 "application started");
        return core::version_string().empty() ? 1 : 0;
    }
} // namespace branchtalk::server

int main()
{
    return branchtalk::server::run();
}