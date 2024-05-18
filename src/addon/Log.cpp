//
// Created by Seres67 on 19/05/2024.
//

#include "addon/Log.hpp"
#include "addon/Shared.hpp"

void Log::debug(const char *msg) {
    APIDefs->Log(ELogLevel_DEBUG, "radial", msg);
}

void Log::info(const char *msg) {
    APIDefs->Log(ELogLevel_INFO, "radial", msg);
}
