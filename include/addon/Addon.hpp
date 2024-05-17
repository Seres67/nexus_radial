#ifndef NEXUS_TEMPLATE_ADDON_HPP
#define NEXUS_TEMPLATE_ADDON_HPP

#include <filesystem>
#include "nexus/Nexus.h"
#include "Shared.hpp"

class Addon {
public:
    static void load(AddonAPI *aApi);

    static void unload();

    static void render();

    static void render_options();

private:
    static std::filesystem::path AddonPath;
    static std::filesystem::path SettingsPath;
    //TODO: change name
    static constexpr std::string_view m_addon_name = "template";
};


#endif //NEXUS_TEMPLATE_ADDON_HPP
