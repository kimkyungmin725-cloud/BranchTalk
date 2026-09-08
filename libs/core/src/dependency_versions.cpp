#include <branchtalk/core/dependency_versions.hpp>

#include <nlohmann/json.hpp>
#include <spdlog/version.h>

namespace branchtalk::core
{
    namespace
    {

        nlohmann::json semantic_version(int major, int minor, int patch)
        {
            return {
                {"major", major},
                {"minor", minor},
                {"patch", patch},
            };
        }

    } // namespace

    std::string dependency_versions_json()
    {
        const nlohmann::json versions{
            {"nlohmann-json",
             semantic_version(NLOHMANN_JSON_VERSION_MAJOR,
                              NLOHMANN_JSON_VERSION_MINOR,
                              NLOHMANN_JSON_VERSION_PATCH)},
            {"spdlog", semantic_version(SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH)},
        };

        return versions.dump();
    }

} // namespace branchtalk::core