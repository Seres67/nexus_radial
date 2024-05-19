//
// Created by Seres67 on 19/05/2024.
//

#include "addon/Utils.hpp"

namespace Utils {
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
}