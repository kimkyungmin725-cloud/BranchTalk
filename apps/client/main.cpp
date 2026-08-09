#include <branchtalk/core/version.hpp>

namespace branchtalk::client
{
    int run() noexcept
    {
        return core::version_string().empty() ? 1 : 0;
    }
} //namespace branchtalk::client

int main(){
    return branchtalk::client::run();
}

