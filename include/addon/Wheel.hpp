//
// Created by Seres67 on 19/05/2024.
//

#ifndef NEXUS_RADIAL_WHEEL_HPP
#define NEXUS_RADIAL_WHEEL_HPP

#include <string>
#include <vector>

class Wheel {
public:
    Wheel(const char *name);

    void add_element(const std::string &name);

    void open_wheel();
    void close_wheel();

    bool is_open() const;

    bool render_wheel();

    const char *get_wheel_name() const;

    std::vector<std::string> get_elements();

private:
    const char *m_name;
    std::vector<std::string> m_elements;
    bool m_wheel_open = false;
    int m_hovered_id = -1;
};


#endif //NEXUS_RADIAL_WHEEL_HPP
