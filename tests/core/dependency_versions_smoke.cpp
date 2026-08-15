#include <branchtalk/core/dependency_versions.hpp>

#include <string_view>

int main() {
    constexpr std::string_view kExpectedVersions = 
        R"({"nlohmann-json":{"major":3,"minor":12,"patch":0}, "spdlog":{"major":1,"minor":17,"patch":0}})";
    
    return branchtalk::core::dependency_versions_json() == kExpectedVersions ? 0 : 1;
}