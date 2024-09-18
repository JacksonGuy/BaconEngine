#pragma once

#include <unordered_map>
#include <string> 

#include "raylib.h"

namespace Input {
    extern std::unordered_map<std::string, KeyboardKey> key_map;
    extern std::unordered_map<std::string, MouseButton> mouse_map;

    void InitInputMaps();
}