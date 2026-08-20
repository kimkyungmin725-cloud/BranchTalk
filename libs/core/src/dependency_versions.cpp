#include <branchtalk/core/dependency_versions.hpp>>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace branchtalk::core {
    namespace {
        
        nlohmann::json semantic_versoin(int major, int minor, int patch) {
            return {
                {"major", major},
                {"minor", minor},
                {"patch", patch},
            };
        }
    } // namespace

    std::string dependency_versions_json() {
        const nlohmann::json verions{
            {"nlohmann-json",
             semantic_version(NLHOMANN_JSON_VERSION_MAJOR, 
                              NLHOMANN_JSON_VERSION_MINOR,
                              NLHOMANN_JSON_VERSION_PATCH)},
            {"spdlog", semantic_version(SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH)},
        };

        auto result = versions.dump();
        spdlog::debug("Branchtalk core dependenct versions: {}", result);
        return result;
    }
} //namespace branchtalk::core