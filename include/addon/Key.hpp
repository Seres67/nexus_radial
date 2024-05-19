//
// Created by Seres67 on 18/05/2024.
//

#ifndef NEXUS_RADIAL_KEY_HPP
#define NEXUS_RADIAL_KEY_HPP
#include <windows.h>

class Key {
public:
    explicit Key(char key, bool alt = false, bool ctrl = false, bool shift = false);
    void press(HWND hwnd) const;

    char get_key_code() const;

    bool alt() const;

    bool ctrl() const;

    bool shift() const;

private:
    char m_key;
    bool m_alt;
    bool m_ctrl;
    bool m_shift;
};


#endif //NEXUS_RADIAL_KEY_HPP
