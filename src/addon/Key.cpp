//
// Created by Seres67 on 18/05/2024.
//

#include "addon/Key.hpp"
#include "addon/Utils.hpp"
#include <cstdint>

Key::Key(char key, bool alt, bool ctrl, bool shift) : m_key(key), m_alt(alt), m_ctrl(ctrl), m_shift(shift) {

}

void Key::press(HWND hwnd) const {
    if (m_alt)
        PostMessage(hwnd, WM_KEYDOWN, VK_LMENU, Utils::GetLParam(VK_LMENU, true));
    if (m_shift)
        PostMessage(hwnd, WM_KEYDOWN, VK_LSHIFT, Utils::GetLParam(VK_LSHIFT, true));
    if (m_ctrl)
        PostMessage(hwnd, WM_KEYDOWN, VK_LCONTROL, Utils::GetLParam(VK_LCONTROL, true));
    PostMessage(hwnd, WM_KEYDOWN, m_key, Utils::GetLParam(m_key, true));
    PostMessage(hwnd, WM_KEYUP, m_key, Utils::GetLParam(m_key, false));
    if (m_ctrl)
        PostMessage(hwnd, WM_KEYUP, VK_LCONTROL, Utils::GetLParam(VK_LCONTROL, false));
    if (m_shift)
        PostMessage(hwnd, WM_KEYUP, VK_LSHIFT, Utils::GetLParam(VK_LSHIFT, false));
    if (m_alt)
        PostMessage(hwnd, WM_KEYUP, VK_LMENU, Utils::GetLParam(VK_LMENU, false));
}

char Key::get_key_code() const {
    return m_key;
}

bool Key::alt() const {
    return m_alt;
}

bool Key::ctrl() const {
    return m_ctrl;
}

bool Key::shift() const {
    return m_shift;
}

void Key::from_json(const nlohmann::json &json, Key &key) {
    json["m_key"].get_to<char>(key.m_key);
    json["m_alt"].get_to<bool>(key.m_alt);
    json["m_ctrl"].get_to<bool>(key.m_ctrl);
    json["m_shift"].get_to<bool>(key.m_shift);
}

void Key::to_json(nlohmann::json &json, const Key &key) {
    json["m_key"] = key.m_key;
    json["m_alt"] = key.m_alt;
    json["m_ctrl"] = key.m_ctrl;
    json["m_shift"] = key.m_shift;
}
