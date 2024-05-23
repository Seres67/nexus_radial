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
    char command[20]{0};
    std::vector<Wheel> wheels;
    int wheel_to_add_element_to = -1;
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
//        Settings::Load(m_addon_path);
        m_button_pressed = false;
    }

    void unload() {
        APIDefs->DeregisterRender(Addon::render);
        APIDefs->DeregisterRender(Addon::render_options);
        APIDefs->DeregisterWndProc(Addon::wndproc);

        MumbleLink = nullptr;
        NexusLink = nullptr;
        for (auto &wheel: wheels) {
            nlohmann::json a = wheel;
            Settings::m_json_settings[Settings::WHEELS].emplace_back(a);
        }
        Settings::Save(m_addon_path);
    }

    void execute_keybinding(const char *str, std::size_t len) {
        std::thread([str, len]() {
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
            for (std::size_t i = 0; i < len; ++i)
                PostMessage(m_window, WM_CHAR, (WPARAM) str[i], 0);
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
                    std::shared_ptr<Action> ele = wheel.get_hovered_element();
                    if (ele == nullptr)
                        continue;
                    if (std::holds_alternative<Key>(ele->m_action)) {
                        Key key = std::get<Key>(ele->m_action);
                        key.press(m_window);
                    } else if (std::holds_alternative<std::string>(ele->m_action)) {
                        std::string str = std::get<std::string>(ele->m_action);
                        execute_keybinding(_strdup(str.c_str()), str.length());
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
                    if (ImGui::Button("Press to bind a key")) {
                        m_key = Key{0};
                        m_waiting_for_wheel_keybinding = true;
                    }
                } else if (m_waiting_for_wheel_keybinding) {
                    ImGui::Text("Waiting for keys");
                } else {
                    if (ImGui::Button("Press to bind a key")) {
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
                if (ImGui::Button("Press to bind a key")) {
                    m_key = Key{0};
                    m_waiting_for_wheel_keybinding = true;
                }
            } else if (m_waiting_for_wheel_keybinding) {
                ImGui::Text("Waiting for keys");
            } else {
                if (ImGui::Button("Press to bind a key")) {
                    m_waiting_for_wheel_keybinding = true;
                }
            }
            if (ImGui::Button("Confirm##confirm_element")) {
                if (wheel_to_add_element_to != -1)
                    wheels[wheel_to_add_element_to].add_element(element_name, m_key);
                m_key = Key{0};
                wheel_to_add_element_to = -1;
                memset(element_name, 0, 20);
                ImGui::CloseCurrentPopup();
                m_popup_open = false;
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupModal("Add wheel clipboard##element_modal")) {
            ImGui::InputText("Name##element_name", element_name, 20);
            ImGui::InputText("Command to type in chat##command", command, 20);
            if (ImGui::Button("Confirm##confirm_element")) {
                if (wheel_to_add_element_to != -1)
                    wheels[wheel_to_add_element_to].add_element(element_name, command);
                ImGui::CloseCurrentPopup();
                m_popup_open = false;
                memset(element_name, 0, 20);
                memset(command, 0, 20);
                wheel_to_add_element_to = -1;
            }
            ImGui::EndPopup();
        }
        for (int i = 0; i < wheels.size(); ++i) {
            ImGui::NewLine();
            ImGui::Text("%s", wheels[i].get_wheel_name().c_str());
            if (ImGui::Button((std::string("Add keybinding##") + wheels[i].get_wheel_name() + "_add_keybinding").c_str())) {
                ImGui::OpenPopup("Add wheel keybinding##element_modal");
                wheel_to_add_element_to = i;
                m_popup_open = true;
            }
            ImGui::SameLine();
            if (ImGui::Button((std::string("Add clipboard##") + wheels[i].get_wheel_name() + "_add_clipboard").c_str())) {
                ImGui::OpenPopup("Add wheel clipboard##element_modal");
                wheel_to_add_element_to = i;
                m_popup_open = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Set keybinding")) {

            }
            for (auto &element: wheels[i].get_elements()) {
                ImGui::Text("%s", element.m_action_name.c_str());
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
//        for (auto &wheel: wheels) {
//            ImGui::NewLine();
//            ImGui::Text("%s", wheel.get_wheel_name().c_str());
//            if (ImGui::Button((std::string("Add keybinding##") + wheel.get_wheel_name() + "_add_keybinding").c_str())) {
//                ImGui::OpenPopup("Add wheel keybinding##element_modal");
//                wheel_to_add_element_to = &wheel;
//                m_popup_open = true;
//            }
//            ImGui::SameLine();
//            if (ImGui::Button((std::string("Add clipboard##") + wheel.get_wheel_name() + "_add_clipboard").c_str())) {
//                ImGui::OpenPopup("Add wheel clipboard##element_modal");
//                wheel_to_add_element_to = &wheel;
//                m_popup_open = true;
//            }
//            ImGui::SameLine();
//            if (ImGui::Button("Set keybinding")) {
//
//            }
//            for (auto &element: wheel.get_elements()) {
//                ImGui::Text("%s", element.action_name.c_str());
//                ImGui::SameLine();
//                if (ImGui::Button("Add keybinding")) {
//                    Settings::adding_keybinding = true;
//                }
//                ImGui::SameLine();
//                if (ImGui::Button("Delete element")) {
//                    Settings::adding_keybinding = true;
//                }
//            }
//        }
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