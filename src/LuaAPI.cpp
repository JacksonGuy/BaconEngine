#include <iostream>

#include "raylib.h"

#include "LuaAPI.h"
#include "Input.h"
#include "GameManager.h"

namespace Lua {
    bool GetKeyDown(std::string key) {
        return IsKeyDown(Input::key_map[key]);
    }

    bool GetKeyUp(std::string key) {
        return IsKeyUp(Input::key_map[key]);
    }

    bool GetMouseDown(std::string key) {
        return IsMouseButtonDown(Input::mouse_map[key]);
    }

    bool GetMouseUp(std::string key) {
        return IsMouseButtonUp(Input::mouse_map[key]);
    }

    void ConsoleWrite(std::string text) {
        GameManager::ConsoleMessage(text);
    }
}