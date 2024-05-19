#include <thread>
#include "addon/Addon.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "addon/Settings.hpp"
#include "addon/Log.hpp"
#include "addon/Wheel.hpp"
#include "addon/Utils.hpp"


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
    bool m_waiting_for_wheel_keybinding = false;
    bool m_control_down = false;
    bool m_alt_down = false;
    bool m_shift_down = false;
    Key m_key{0};

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

    void execute_keybinding(mts_action ele) {
        std::thread([ele]() {
            using namespace std::chrono_literals;
            Key enter{VK_RETURN};
            if (m_control_down)
                PostMessage(m_window, WM_KEYUP, VK_LCONTROL, Utils::GetLParam(VK_LCONTROL, false));
            if (m_alt_down)
                PostMessage(m_window, WM_KEYUP, VK_LMENU, Utils::GetLParam(VK_LMENU, false));
            if (m_shift_down)
                PostMessage(m_window, WM_KEYUP, VK_LSHIFT, Utils::GetLParam(VK_LSHIFT, false));
            enter.press(m_window);
            std::this_thread::sleep_for(20ms);
            for (int i = 0; i < strlen(ele.clipboard); ++i)
                PostMessage(m_window, WM_CHAR, (WPARAM) ele.clipboard[i], 0);
            enter.press(m_window);
            if (m_control_down)
                PostMessage(m_window, WM_KEYDOWN, VK_LCONTROL, Utils::GetLParam(VK_LCONTROL, true));
            if (m_alt_down)
                PostMessage(m_window, WM_KEYDOWN, VK_LMENU, Utils::GetLParam(VK_LMENU, true));
            if (m_shift_down)
                PostMessage(m_window, WM_KEYDOWN, VK_LSHIFT, Utils::GetLParam(VK_LSHIFT, true));
        }).detach();
    }

    void render() {
        ImGui::PushFont(static_cast<ImFont *>(NexusLink->Font));
        for (auto &wheel: wheels) {
            if (m_button_pressed && !wheel.is_open() && !m_popup_open)
                wheel.open_wheel();
            if (wheel.render_wheel()) {
                if (!m_button_pressed && wheel.is_open()) {
                    wheel.close_wheel();
                    auto ele = wheel.get_hovered_element();
                    if (ele.action_name) {
                        Log::debug(ele.clipboard);
                        if (ele.type)
                            execute_keybinding(ele);
                        else
                            ele.key.press(m_window);
                    }
                }
                ImGui::EndPopup();
            }
        }
        ImGui::PopFont();
    }

    void render_options() {
        if (ImGui::BeginPopupModal("Add wheel##wheel_modal")) {
            ImGui::Text("Wheel name");
            ImGui::InputText("", wheel_name, 20);
            if (m_waiting_for_wheel_keybinding) {

            } else {
                if (m_key.get_key_code()) {
                    std::string str;
                    if (m_key.ctrl())
                        str += "CTRL+";
                    if (m_key.alt())
                        str += "ALT+";
                    if (m_key.shift())
                        str += "SHIFT+";
                    char c = MapVirtualKeyA(m_key.get_key_code(), MAPVK_VK_TO_CHAR);
                    str += c;
                    ImGui::Text("%s", str.c_str());
                    if (ImGui::Button("Wheel keybinding")) {
                        m_key = Key{0};
                        m_waiting_for_wheel_keybinding = true;
                    }
                } else {
                    if (ImGui::Button("Wheel keybinding")) {
                        m_waiting_for_wheel_keybinding = true;
                    }
                }
            }
            if (ImGui::Button("Confirm##confirm_wheel")) {
                wheels.emplace_back(wheel_name, m_key);
                m_key = Key{0};
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
        if (ImGui::BeginPopupModal("Add wheel keybinding##element_modal")) {
            ImGui::Text("Keybinding name");
            ImGui::InputText("", element_name, 20);
            if (m_key.get_key_code()) {
                std::string str;
                if (m_key.ctrl())
                    str += "CTRL+";
                if (m_key.alt())
                    str += "ALT+";
                if (m_key.shift())
                    str += "SHIFT+";
                char c = MapVirtualKeyA(m_key.get_key_code(), MAPVK_VK_TO_CHAR);
                str += c;
                ImGui::Text("%s", str.c_str());
                if (ImGui::Button("Keybinding")) {
                    m_key = Key{0};
                    m_waiting_for_wheel_keybinding = true;
                }
            } else {
                if (ImGui::Button("Keybinding")) {
                    m_waiting_for_wheel_keybinding = true;
                }
            }
            if (ImGui::Button("Confirm##confirm_element")) {
                mts_action action = {.type = false, .action_name = _strdup(element_name), .key = m_key};
                if (wheel_to_add_element_to)
                    wheel_to_add_element_to->add_element(action);
                m_key = Key{0};
                wheel_to_add_element_to = nullptr;
                memset(element_name, 0, 20);
                ImGui::CloseCurrentPopup();
                m_popup_open = false;
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupModal("Add wheel clipboard##element_modal")) {
            ImGui::Text("Command name");
            ImGui::InputText("", element_name, 20);
            if (ImGui::Button("Confirm##confirm_element")) {
                mts_action clipboard = {.type = true, .action_name = _strdup(element_name), .clipboard = _strdup(element_name)};
                if (wheel_to_add_element_to)
                    wheel_to_add_element_to->add_element(clipboard);
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
            if (ImGui::Button((std::string("Add keybinding##") + wheel.get_wheel_name() + "_add_keybinding").c_str())) {
                ImGui::OpenPopup("Add wheel keybinding##element_modal");
                wheel_to_add_element_to = &wheel;
                m_popup_open = true;
            }
            ImGui::SameLine();
            if (ImGui::Button((std::string("Add clipboard##") + wheel.get_wheel_name() + "_add_clipboard").c_str())) {
                ImGui::OpenPopup("Add wheel clipboard##element_modal");
                wheel_to_add_element_to = &wheel;
                m_popup_open = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Set keybinding")) {

            }
            for (auto &element: wheel.get_elements()) {
                ImGui::Text("%s", element.action_name);
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
        if (m_waiting_for_wheel_keybinding) {
            switch (uMsg) {
                case WM_KEYDOWN:
                    if (wParam == VK_LCONTROL || wParam == VK_CONTROL || wParam == VK_RCONTROL) {
                        m_control_down = true;
                        break;
                    } else if (wParam == VK_LSHIFT || wParam == VK_SHIFT || wParam == VK_RSHIFT) {
                        m_shift_down = true;
                        break;
                    } else if (wParam == VK_LMENU || wParam == VK_MENU || wParam == VK_RMENU) {
                        m_alt_down = true;
                        break;
                    } else {
                        m_key = Key{static_cast<char>(wParam), m_alt_down, m_control_down, m_shift_down};
                        m_waiting_for_wheel_keybinding = false;
                    }
                    break;
                case WM_KEYUP:
                    if (wParam == VK_LCONTROL || wParam == VK_CONTROL || wParam == VK_RCONTROL) {
                        m_control_down = false;
                        break;
                    } else if (wParam == VK_LSHIFT || wParam == VK_SHIFT || wParam == VK_RSHIFT) {
                        m_shift_down = false;
                        break;
                    } else if (wParam == VK_LMENU || wParam == VK_MENU || wParam == VK_RMENU) {
                        m_alt_down = false;
                        break;
                    }
                default:
                    break;
            }
        }
        if (Settings::adding_keybinding) {
            Settings::adding_keybinding = false;
        } else {
            switch (uMsg) {
                case WM_KEYDOWN:
                    if (wParam == VK_LCONTROL || wParam == VK_CONTROL || wParam == VK_RCONTROL) {
                        m_control_down = true;
                        break;
                    } else if (wParam == VK_LSHIFT || wParam == VK_SHIFT || wParam == VK_RSHIFT) {
                        m_shift_down = true;
                        break;
                    } else if (wParam == VK_LMENU || wParam == VK_MENU || wParam == VK_RMENU) {
                        m_alt_down = true;
                        break;
                    }
                    for (auto &wheel: wheels) {
                        Key wheel_key = wheel.get_key();
                        if (wheel_key.get_key_code() == wParam && wheel_key.alt() == m_alt_down && wheel_key.ctrl() == m_control_down &&
                            wheel_key.shift() == m_shift_down) {
                            m_button_pressed = true;
                        }
                    }
                    break;
                case WM_KEYUP:
                    if (wParam == VK_LCONTROL || wParam == VK_CONTROL || wParam == VK_RCONTROL) {
                        m_control_down = false;
                        break;
                    } else if (wParam == VK_LSHIFT || wParam == VK_SHIFT || wParam == VK_RSHIFT) {
                        m_shift_down = false;
                        break;
                    } else if (wParam == VK_LMENU || wParam == VK_MENU || wParam == VK_RMENU) {
                        m_alt_down = false;
                        break;
                    }
                    for (auto &wheel: wheels) {
                        Key wheel_key = wheel.get_key();
                        if (wheel_key.get_key_code() == wParam) {
                            m_button_pressed = false;
                        }
                    }
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