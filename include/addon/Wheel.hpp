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
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Wheel, m_name, m_key, m_elements)

    Wheel(std::string name, Key key);

    void add_element(const std::string &name, const std::string &command);

    void add_element(const std::string &name, Key key);

    void open_wheel();
    void close_wheel();

    [[nodiscard]] bool is_open() const;

    std::shared_ptr<Action> get_hovered_element();

    bool render_wheel();

    [[nodiscard]] const std::string &get_wheel_name() const;

    std::vector<Action> get_elements();

    [[nodiscard]] std::vector<Action> get_elements() const;

    [[nodiscard]] const Key &get_key() const;


public:

    std::string m_name;
    Key m_key;
    std::vector<Action> m_elements;
    bool m_wheel_open = false;
    int m_hovered_id = -1;
};


#endif //NEXUS_RADIAL_WHEEL_HPP
