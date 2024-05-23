//
// Created by Seres67 on 19/05/2024.
//

#ifndef NEXUS_RADIAL_ACTION_HPP
#define NEXUS_RADIAL_ACTION_HPP

#include <string>
#include <utility>
#include <variant>
#include "addon/Key.hpp"

struct Action {
    Action(const std::string &action_name, const std::string &command) : m_type(false), m_action_name(action_name), m_action(command) {

    }

    Action(const std::string &action_name, Key key) : m_type(true), m_action_name(action_name), m_action(key) {

    }
//
//    Action(Action &action) {
//        type = action.type;
//        action_name = action.action_name;
//        if (type)
//            u.key = action.u.key;
//        else
//            u.clipboard = action.u.clipboard;
//    }
//
//    Action(Action &&action) noexcept {
//        m_type = action.m_type;
//        m_action_name = action.m_action_name;
//        m_action = action.m_action;
//    }


    bool m_type;
    std::string m_action_name;
    std::variant<Key, std::string> m_action;
};


#endif //NEXUS_RADIAL_ACTION_HPP