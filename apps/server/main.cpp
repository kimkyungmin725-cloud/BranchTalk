#include <branchtalk/core/version.hpp>

namespace branchtalk::server
{
    int run() noexcept
    {
        return core::version_string().empty() ? 1 : 0;
    }
} //namespace branchtalk::server

int main(){
    return branchtalk::server::run();
}