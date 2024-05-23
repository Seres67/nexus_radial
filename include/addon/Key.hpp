//
// Created by Seres67 on 18/05/2024.
//

#ifndef NEXUS_RADIAL_KEY_HPP
#define NEXUS_RADIAL_KEY_HPP
#include <windows.h>
#include <nlohmann/json.hpp>

class Key {
public:
    explicit Key(char key, bool alt = false, bool ctrl = false, bool shift = false);
    void press(HWND hwnd) const;

    [[nodiscard]] char get_key_code() const;

    [[nodiscard]] bool alt() const;

    [[nodiscard]] bool ctrl() const;

    [[nodiscard]] bool shift() const;

    static void from_json(const nlohmann::json &json, Key &key);

    static void to_json(nlohmann::json &json, const Key &key);

private:
    char m_key;
    bool m_alt;
    bool m_ctrl;
    bool m_shift;
};


#endif //NEXUS_RADIAL_KEY_HPP
