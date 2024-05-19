//
// Created by Seres67 on 19/05/2024.
//

#ifndef NEXUS_RADIAL_ACTION_HPP
#define NEXUS_RADIAL_ACTION_HPP

#include <string>
#include "addon/Key.hpp"

//
//typedef struct {
//    std::string text;
//} mts_clipboard;
//
//typedef struct {
//    Key key;
//} mts_keybinding;

typedef struct ms_action {
    bool type;
    char *action_name;

    union {
        char *clipboard;
        Key key{0};
    };
} mts_action;

#endif //NEXUS_RADIAL_ACTION_HPP
