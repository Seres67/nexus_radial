//
// Created by Seres67 on 19/05/2024.
//

#ifndef NEXUS_RADIAL_WHEEL_HPP
#define NEXUS_RADIAL_WHEEL_HPP

#include <string>
#include <vector>
#include "Action.hpp"

class Wheel {
public:
    Wheel(const char *name, Key key);

    void add_element(mts_action name);

    void open_wheel();
    void close_wheel();

    bool is_open() const;

    mts_action get_hovered_element() const;

    bool render_wheel();

    const char *get_wheel_name() const;

    std::vector<mts_action> get_elements();

    Key get_key() const;

private:
    const char *m_name;
    Key m_key;
    std::vector<mts_action> m_elements;
    bool m_wheel_open = false;
    int m_hovered_id = -1;
};


#endif //NEXUS_RADIAL_WHEEL_HPP
