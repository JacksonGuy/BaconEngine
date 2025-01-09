#include <iostream>
#include <fstream>

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
        GameManager::lua["GetObjectByID"] = Lua::GetObjectByID;
        GameManager::lua["CreateObjectFromPrefab"] = Lua::CreateObjectFromPrefab;
        GameManager::lua["CreateCopyObject"] = Lua::CreateCopyObject;
        GameManager::lua["DeleteObject"] = Lua::DeleteObject;

        GameManager::lua["TestFunction"] = Lua::TestFunction;
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

    sol::object CreateObjectFromPrefab(std::string path) {
        std::ifstream infile(path);
        nlohmann::json data = nlohmann::json::parse(infile);
        
        if (data["type"] == 1) {
            Entity* entity = new Entity();
            entity->LoadEntityPrefab(data);
            object_references.push_back(entity);
            return sol::make_object(GameManager::lua, entity);
        }
        else if (data["type"] == 2) {
            TextObject* text = new TextObject();
            text->LoadTextObjectPrefab(data);
            object_references.push_back(text);
            return sol::make_object(GameManager::lua, text);
        }
        else {
            GameManager::ConsoleError("Invalid object type");
            return sol::make_object(GameManager::lua, sol::nil);
        }
    }

    sol::object CreateCopyObject(u32 id) {
        GameObject* object = GameManager::FindObjectByID(id);
        if (object == nullptr) {
            GameManager::ConsoleError("Could not find object with ID=" + std::to_string(id));
            return sol::make_object(GameManager::lua, sol::nil);
        } 

        if (object->type == ENTITY) {
            Entity* e = (Entity*)object;
            Entity* newEntity = new Entity(e);
            object_references.push_back(newEntity);
            return sol::make_object(GameManager::lua, newEntity);
        }
        else if (object->type == TEXT) {
            TextObject* text = (TextObject*)object;
            TextObject* newText = new TextObject(text);
            object_references.push_back(newText);
            return sol::make_object(GameManager::lua, newText);
        }
        else if (object->type == CAMERA) {
            GameCamera* camera = (GameCamera*)object;
            GameCamera* newCam = new GameCamera(camera);
            object_references.push_back(newCam);
            return sol::make_object(GameManager::lua, newCam);
        }
        else {
            GameManager::ConsoleError("Invalid object type");
            return sol::make_object(GameManager::lua, sol::nil);
        }
    }

    void DeleteObject(u32 id) {
        GameObject* object = GameManager::FindObjectByID(id);
        if (object == nullptr) {
            GameManager::ConsoleError("Could not find object with ID=" + std::to_string(id));
            return;
        }

        // This is some ugly code.
        // The workaround to this is to make GameObject's
        // destructor virtual, but then there's a lot of code
        // copying going on there. For the time being, this is
        // preferable in my opinion. 

        if (object->type == ENTITY) {
            Entity* e = (Entity*)object;
            delete e;
        }
        else if (object->type == TEXT) {
            TextObject* text = (TextObject*)object;
            delete text;
        }
        else if (object->type == CAMERA) {
            GameCamera* camera = (GameCamera*)object;
            delete camera;
        }
        else {
            delete object;
        }
    }

    u32 GetObjectIDByName(std::string name) {
        for (GameObject* obj : GameManager::GameObjects) {
            if (obj->name == name) return obj->ID;
        }
        GameManager::ConsoleError("Object with name " + name + " does not exist");
        return UINT32_MAX;
    }

    sol::object GetObjectByID(u32 id) {
        GameObject* object = GameManager::FindObjectByID(id);
        if (object == nullptr) {
            GameManager::ConsoleError("Could not find Entity with ID=" + std::to_string(id));
            return sol::make_object(GameManager::lua, sol::nil);
        }

        if (object->type == ENTITY) {
            Entity* e = (Entity*)object;
            object_references.push_back(e);
            return sol::make_object(GameManager::lua, e);
        }
        else if (object->type == TEXT) {
            TextObject* text = (TextObject*)object;
            object_references.push_back(text);
            return sol::make_object(GameManager::lua, text);
        }
        else if (object->type == CAMERA) {
            GameCamera* camera = (GameCamera*)object;
            object_references.push_back(camera);
            return sol::make_object(GameManager::lua, camera);
        }
        else {
            GameManager::ConsoleError("Object with ID=" + std::to_string(id) + " has unknown type");
            return sol::make_object(GameManager::lua, sol::nil);
        }
    }

    void TestFunction() {
        std::cout << "\n" << GameManager::GameObjects.size() << "\n";
        for (GameObject* obj : GameManager::GameObjects) {
            std::cout << obj->name << std::endl;
        }
    }
}