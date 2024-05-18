#include "addon/Addon.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "addon/Settings.hpp"
#include "addon/Log.hpp"
#include "addon/Wheel.hpp"


namespace Addon {
    std::filesystem::path m_addon_path;
    std::filesystem::path m_settings_path;
    bool m_button_pressed;
    bool m_popup_open = false;
    HWND m_window = nullptr;
    char wheel_name[20]{0};
    char element_name[20]{0};
    std::vector<Wheel> wheels;
    Wheel *wheel_to_add_element_to;

    void load(AddonAPI *aApi) {
        APIDefs = aApi;
        ImGui::SetCurrentContext(static_cast<ImGuiContext *>(APIDefs->ImguiContext));
        ImGui::SetAllocatorFunctions(
                (void *(*)(size_t, void *)) APIDefs->ImguiMalloc,
                (void (*)(void *, void *)) APIDefs->ImguiFree); // on imgui 1.80+

        MumbleLink = (Mumble::Data *) APIDefs->GetResource("DL_MUMBLE_LINK");
        NexusLink = (NexusLinkData *) APIDefs->GetResource("DL_NEXUS_LINK");

        APIDefs->RegisterRender(ERenderType_Render, Addon::render);
        APIDefs->RegisterRender(ERenderType_OptionsRender, Addon::render_options);

        APIDefs->RegisterWndProc(Addon::wndproc);

        m_addon_path = APIDefs->GetAddonDirectory("radial");
        m_settings_path = APIDefs->GetAddonDirectory("radial/settings.json");
        std::filesystem::create_directory(m_addon_path);
        m_button_pressed = false;
    }

    void unload() {
        APIDefs->DeregisterRender(Addon::render);
        APIDefs->DeregisterRender(Addon::render_options);
        APIDefs->DeregisterWndProc(Addon::wndproc);

        MumbleLink = nullptr;
        NexusLink = nullptr;
    }

    void render() {
        ImGui::PushFont(static_cast<ImFont *>(NexusLink->Font));
        for (auto &wheel: wheels) {
            if (m_button_pressed && !wheel.is_open() && !m_popup_open)
                wheel.open_wheel();
            if (wheel.render_wheel()) {
                if (!m_button_pressed && wheel.is_open())
                    wheel.close_wheel();
                ImGui::EndPopup();
            }
        }
        ImGui::PopFont();
    }

    void render_options() {
        if (ImGui::BeginPopupModal("Add wheel##wheel_modal")) {
            ImGui::Text("Wheel name");
            ImGui::InputText("wheel name", wheel_name, 20);
            if (ImGui::Button("Confirm##confirm_wheel")) {
                wheels.emplace_back(wheel_name);
                memset(wheel_name, 0, 20);
                ImGui::CloseCurrentPopup();
                m_popup_open = false;
            }
            ImGui::EndPopup();
        }
        if (ImGui::Button("Add wheel##wheel_button")) {
            ImGui::OpenPopup("Add wheel##wheel_modal");
            m_popup_open = true;
        }
        if (ImGui::BeginPopupModal("Add wheel element##element_modal")) {
            ImGui::Text("Element name");
            ImGui::InputText("element name", element_name, 20);
            if (ImGui::Button("Confirm##confirm_element")) {
                if (wheel_to_add_element_to)
                    wheel_to_add_element_to->add_element(element_name);
                wheel_to_add_element_to = nullptr;
                memset(element_name, 0, 20);
                ImGui::CloseCurrentPopup();
                m_popup_open = false;

            }
            ImGui::EndPopup();
        }

        for (auto &wheel: wheels) {
            ImGui::NewLine();
            ImGui::Text("%s", wheel.get_wheel_name());
            if (ImGui::Button((std::string("Add element##") + wheel.get_wheel_name() + "_add_element").c_str())) {
                ImGui::OpenPopup("Add wheel element##element_modal");
                wheel_to_add_element_to = &wheel;
                m_popup_open = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Set keybinding")) {

            }
            for (auto &element: wheel.get_elements()) {
                ImGui::Text("%s", element.c_str());
                ImGui::SameLine();
                if (ImGui::Button("Add keybinding")) {
                    Settings::adding_keybinding = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("Delete element")) {
                    Settings::adding_keybinding = true;
                }
            }
        }
    }

    unsigned int wndproc(HWND hWnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam) {
        if (!m_window)
            m_window = hWnd;
        if (Settings::adding_keybinding) {
            Settings::adding_keybinding = false;
        } else {
            switch (uMsg) {
                case WM_KEYDOWN:
                    if (wParam == 'C')
                        m_button_pressed = true;
                    break;
                case WM_KEYUP:
                    if (wParam == 'C')
                        m_button_pressed = false;
                    break;
                case WM_LBUTTONDBLCLK:
                case WM_LBUTTONUP:
                case WM_LBUTTONDOWN:
                case WM_RBUTTONDBLCLK:
                case WM_RBUTTONUP:
                case WM_RBUTTONDOWN:
                    m_button_pressed = false;
                default:
                    break;
            }
        }
        return uMsg;
    }
}