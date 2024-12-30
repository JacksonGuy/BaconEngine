#include <iostream>

#include "raylib.h"

#include "LuaAPI.h"
#include "Input.h"
#include "GameManager.h"

namespace Lua {
    std::vector<GameObject*> object_references;

    void RegisterFunctions() {
        GameManager::lua["GetKeyDown"] = Lua::GetKeyDown;
        GameManager::lua["GetKeyUp"] = Lua::GetKeyUp;
        GameManager::lua["GetKeyRelease"] = Lua::GetKeyRelease;
        GameManager::lua["GetMouseDown"] = Lua::GetMouseDown;
        GameManager::lua["GetMouseUp"] = Lua::GetMouseUp;
        GameManager::lua["GetMouseRelease"] = Lua::GetMouseRelease;

        GameManager::lua["ConsoleWrite"] = GameManager::ConsoleMessage;
        GameManager::lua["GetObjectIDByName"] = Lua::GetObjectIDByName;
        GameManager::lua["GetEntityByID"] = Lua::GetEntityByID;
        GameManager::lua["GetTextByID"] = Lua::GetTextByID;
    }

    void RegisterClasses() {
        sol::usertype<Vector2> vector2_type = GameManager::lua.new_usertype<Vector2>(
            "Vector2",
            sol::constructors<Vector2()>()
        );
        vector2_type["x"] = &Vector2::x;
        vector2_type["y"] = &Vector2::y;

        sol::usertype<Entity> entity_type = GameManager::lua.new_usertype<Entity>(
            "Entity",
            sol::constructors<Entity()>(),
            sol::base_classes, sol::bases<GameObject>()
        );
        entity_type["ID"] = &Entity::ID;
        entity_type["name"] = &Entity::name;
        entity_type["tag"] = &Entity::tag;
        entity_type["position"] = &Entity::position;
        entity_type["size"] = &Entity::size;
        entity_type["rotation"] = &Entity::rotation;
        entity_type["isVisible"] = &Entity::isVisible;
        entity_type["layer"] = &Entity::layer;
        entity_type["texture"] = &Entity::texturePath;
        entity_type["solid"] = &Entity::solid;
        entity_type["physicsObject"] = &Entity::physicsObject;
        entity_type["velocity"] = &Entity::velocity;

        sol::usertype<TextObject> textobject_type = GameManager::lua.new_usertype<TextObject>(
            "TextObject",
            sol::constructors<TextObject()>(),
            sol::base_classes, sol::bases<GameObject>()
        );
        textobject_type["ID"] = &TextObject::ID;
        textobject_type["name"] = &TextObject::name;
        textobject_type["tag"] = &TextObject::tag;
        textobject_type["position"] = &TextObject::position;
        textobject_type["size"] = &TextObject::size;
        textobject_type["rotation"] = &TextObject::rotation;
        textobject_type["isVisible"] = &TextObject::isVisible;
        textobject_type["layer"] = &TextObject::layer;
        textobject_type["text"] = &TextObject::text;
        textobject_type["fontSize"] = &TextObject::fontSize;
        textobject_type["charSpacing"] = &TextObject::charSpacing;
    }

    bool GetKeyDown(std::string key) {
        return IsKeyDown(Input::key_map[key]);
    }

    bool GetKeyUp(std::string key) {
        return IsKeyUp(Input::key_map[key]);
    }

    bool GetKeyRelease(std::string key) {
        return IsKeyReleased(Input::key_map[key]);
    }

    bool GetMouseDown(std::string button) {
        return IsMouseButtonDown(Input::mouse_map[button]);
    }

    bool GetMouseUp(std::string button) {
        return IsMouseButtonUp(Input::mouse_map[button]);
    }

    bool GetMouseRelease(std::string button) {
        return IsMouseButtonReleased(Input::mouse_map[button]);
    }

    u32 GetObjectIDByName(std::string name) {
        for (GameObject* obj : GameManager::GameObjects) {
            if (obj->name == name) return obj->ID;
        }
        return -1;
    }

    Entity* GetEntityByID(u32 id) {
        GameObject* object = GameManager::FindObjectByID(id);
        if (object == nullptr) return nullptr;
        else if (object->type != ENTITY) return nullptr;
        else {
            object_references.push_back(object);
            return (Entity*)object;
        }
    }

    TextObject* GetTextByID(u32 id) {
        GameObject* object = GameManager::FindObjectByID(id);
        if (object == nullptr) return nullptr;
        else if (object->type != TEXT) return nullptr;
        else {
            object_references.push_back(object);
            return (TextObject*)object;
        }
    }
}