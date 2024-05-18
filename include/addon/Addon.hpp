#ifndef NEXUS_TEMPLATE_ADDON_HPP
#define NEXUS_TEMPLATE_ADDON_HPP

#include <filesystem>
#include "nexus/Nexus.h"
#include "Shared.hpp"

namespace Addon {
    void load(AddonAPI *aApi);

    void unload();

    void render();

    void render_options();

    unsigned int wndproc(HWND hWnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam);
}


#endif //NEXUS_TEMPLATE_ADDON_HPP
