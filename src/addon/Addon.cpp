#include "addon/Addon.hpp"
#include "imgui/imgui.h"

std::filesystem::path Addon::AddonPath;
std::filesystem::path Addon::SettingsPath;

void Addon::load(AddonAPI *aApi) {
    APIDefs = aApi;
    ImGui::SetCurrentContext(static_cast<ImGuiContext *>(APIDefs->ImguiContext));
    ImGui::SetAllocatorFunctions(
            (void *(*)(size_t, void *)) APIDefs->ImguiMalloc,
            (void (*)(void *, void *)) APIDefs->ImguiFree); // on imgui 1.80+

    MumbleLink = (Mumble::Data *) APIDefs->GetResource("DL_MUMBLE_LINK");
    NexusLink = (NexusLinkData *) APIDefs->GetResource("DL_NEXUS_LINK");

    APIDefs->RegisterRender(ERenderType_Render, Addon::render);
    APIDefs->RegisterRender(ERenderType_OptionsRender, Addon::render_options);

    AddonPath = APIDefs->GetAddonDirectory(m_addon_name.data());
    SettingsPath = APIDefs->GetAddonDirectory((std::string(m_addon_name.data()) + "/settings.json").c_str());
    std::filesystem::create_directory(AddonPath);
}

void Addon::unload() {
    APIDefs->DeregisterRender(Addon::render);
    APIDefs->DeregisterRender(Addon::render_options);
    MumbleLink = nullptr;
    NexusLink = nullptr;
}

void Addon::render() {
    ImGui::PushFont(static_cast<ImFont *>(NexusLink->Font));
    if (ImGui::Begin(m_addon_name.data(), nullptr, 0)) {
        //TODO: heres the main stuff
    }
    ImGui::PopFont();
    ImGui::End();
}

void Addon::render_options() {
    ImGui::TextDisabled("Widget");
}
