#pragma once

#include <string>

namespace Lua {
    // Input
    bool GetKeyDown(std::string key);
    bool GetKeyUp(std::string key);
    bool GetMouseDown(std::string button);
    bool GetMouseUp(std::string button);

    // Engine
    void ConsoleWrite(std::string text);
}