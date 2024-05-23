//
// Created by Seres67 on 18/05/2024.
//

#ifndef NEXUS_RADIAL_SETTINGS_HPP
#define NEXUS_RADIAL_SETTINGS_HPP

#include "Key.hpp"

namespace Settings {
    extern bool adding_keybinding;
    extern const char *WHEELS;
    extern nlohmann::json m_json_settings;

    void Load(const std::filesystem::path &aPath);

    void Save(const std::filesystem::path &aPath);
}


#endif //NEXUS_RADIAL_SETTINGS_HPP
