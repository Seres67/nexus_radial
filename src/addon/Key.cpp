//
// Created by Seres67 on 18/05/2024.
//

#include "addon/Key.hpp"
#include <cstdint>

Key::Key(char key, bool alt, bool ctrl, bool shift) : m_key(key), m_alt(alt), m_ctrl(ctrl), m_shift(shift) {

}


LPARAM GetLParam(std::uint32_t key, bool down) {
    std::uint64_t lParam;
    lParam = down ? 0 : 1; // transition state
    lParam = lParam << 1;
    lParam += down ? 0 : 1; // previous key state
    lParam = lParam << 1;
    lParam += 0; // context code
    lParam = lParam << 1;
    lParam = lParam << 4;
    lParam = lParam << 1;
    lParam = lParam << 8;
    lParam += MapVirtualKeyA(key, MAPVK_VK_TO_VSC);
    lParam = lParam << 16;
    lParam += 1;

    return lParam;
}

void Key::press(HWND hwnd) {
    if (m_alt)
        PostMessage(hwnd, WM_KEYDOWN, VK_LMENU, GetLParam(VK_LMENU, true));
    if (m_shift)
        PostMessage(hwnd, WM_KEYDOWN, VK_LSHIFT, GetLParam(VK_LSHIFT, true));
    if (m_ctrl)
        PostMessage(hwnd, WM_KEYDOWN, VK_LCONTROL, GetLParam(VK_LCONTROL, true));
    PostMessage(hwnd, WM_KEYDOWN, m_key, GetLParam(m_key, true));
    PostMessage(hwnd, WM_KEYUP, m_key, GetLParam(m_key, false));
    if (m_ctrl)
        PostMessage(hwnd, WM_KEYUP, VK_LCONTROL, GetLParam(VK_LCONTROL, false));
    if (m_shift)
        PostMessage(hwnd, WM_KEYUP, VK_LSHIFT, GetLParam(VK_LSHIFT, false));
    if (m_alt)
        PostMessage(hwnd, WM_KEYUP, VK_LMENU, GetLParam(VK_LMENU, false));
}
