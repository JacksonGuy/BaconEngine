#pragma once

#include <string>
#include <vector>

#include "GameObject.h"
#include "Entity.h"
#include "TextObject.h"

namespace Lua {
    extern std::vector<GameObject*> object_references;

    void RegisterFunctions();
    void RegisterClasses();

    // Input
    bool GetKeyDown(std::string key);
    bool GetKeyUp(std::string key);
    bool GetKeyRelease(std::string key);
    bool GetMouseDown(std::string button);
    bool GetMouseUp(std::string button);
    bool GetMouseRelease(std::string button);

    // Engine
    u32 GetObjectIDByName(std::string name);
    Entity* GetEntityByID(u32 id);
    TextObject* GetTextByID(u32 id);
}