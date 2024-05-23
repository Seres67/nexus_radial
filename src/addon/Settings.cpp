//
// Created by Seres67 on 18/05/2024.
//

#include <mutex>
#include <fstream>
#include "addon/Settings.hpp"
#include "nlohmann/json.hpp"
#include "addon/Log.hpp"
#include "addon/Shared.hpp"

using json = nlohmann::json;

namespace Settings {
    nlohmann::json m_json_settings;
    bool adding_keybinding = false;
    std::mutex m_mutex;

    const char *WHEELS = "Wheels";

    void Load(const std::filesystem::path &aPath) {
        m_json_settings = json::object();
        if (!std::filesystem::exists(aPath)) {
            return;
        }

        {
            std::lock_guard lock(m_mutex);
            try {
                std::ifstream file(aPath);
                if (file.is_open()) {
                    m_json_settings = json::parse(file);
                    file.close();
                }
            } catch (json::parse_error &ex) {
                APIDefs->Log(
                        ELogLevel_WARNING, "radial",
                        "Keyboard Overlay: Settings.json could not be parsed.");
                APIDefs->Log(ELogLevel_WARNING, "radial", ex.what());
            }
        }
    }

    void Save(const std::filesystem::path &aPath) {
        if (m_json_settings.is_null()) {
            Log::debug("settings is null");
            return;
        }
        {
            std::lock_guard lock(m_mutex);
            std::ofstream file(aPath);
            if (file.is_open()) {
                file << m_json_settings.dump(1, '\t') << std::endl;
                file.close();
            }
        }
    }
}