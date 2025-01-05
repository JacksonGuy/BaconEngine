#pragma once

#include <string>
#include <vector>

#include <sol/sol.hpp>

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

    // GameObject management
    sol::object CreateObjectFromPrefab(std::string path);
    sol::object CreateCopyObject(u32 id);
    void DeleteObject(u32 id); 

    // Engine
    u32 GetObjectIDByName(std::string name);
    sol::object GetObjectByID(u32 id);
}