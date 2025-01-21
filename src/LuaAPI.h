#pragma once

#include <string>
#include <vector>

#include <sol/sol.hpp>

#include "GameObject.h"
#include "Entity.h"
#include "TextObject.h"

namespace Lua {
    void RegisterFunctions();
    void RegisterClasses();
    sol::object CreateLuaObject(GameObject* obj);

    // Input
    bool GetKeyDown(std::string key);
    bool GetKeyUp(std::string key);
    bool GetKeyRelease(std::string key);
    bool GetMouseDown(std::string button);
    bool GetMouseUp(std::string button);
    bool GetMouseRelease(std::string button);
    bool GetMouseWheelScroll(std::string direction);
    sol::object GetMousePosition();

    // GameObject management
    sol::object CreateObjectFromPrefab(std::string path);
    sol::object CreateCopyObject(u32 id);
    void DeleteObject(u32 id);
    void ObjectAddChild(u32 parentID, u32 childID);
    void ObjectRemoveChild(u32 parentID, u32 childID);

    // Retrieve Objects
    u32 GetObjectIDByName(std::string name);
    sol::object GetObjectByID(u32 id);
    sol::object GetObjectByName(std::string name);
    std::vector<u32> GetObjectIDs();

    // Physics
    bool CheckCollisionAll(u32 id);
    bool CheckCollision(u32 id1, u32 id2);
    sol::object GetCollisionObject(u32 id);

    // Testing
    void TestFunction(); 
}