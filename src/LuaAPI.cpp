#include <iostream>
#include <fstream>

#include "raylib.h"

#include "LuaAPI.h"
#include "Input.h"
#include "GameManager.h"
#include "Audio.h"

#define SOL_ALL_SAFETIES_ON 1 

namespace Lua {
    void RegisterFunctions() {
        GameManager::lua["GetKeyDown"] = Lua::GetKeyDown;
        GameManager::lua["GetKeyUp"] = Lua::GetKeyUp;
        GameManager::lua["GetKeyRelease"] = Lua::GetKeyRelease;
        GameManager::lua["GetMouseDown"] = Lua::GetMouseDown;
        GameManager::lua["GetMouseUp"] = Lua::GetMouseUp;
        GameManager::lua["GetMouseRelease"] = Lua::GetMouseRelease;
        GameManager::lua["GetMouseWheelScroll"] = Lua::GetMouseWheelScroll;
        GameManager::lua["GetMousePosition"] = Lua::GetMousePosition;

        GameManager::lua["ConsoleWrite"] = GameManager::ConsoleGameMessage;
        GameManager::lua["GetObjectIDByName"] = Lua::GetObjectIDByName;
        GameManager::lua["GetObjectByID"] = Lua::GetObjectByID;
        GameManager::lua["GetObjectByName"] = Lua::GetObjectByName;
        GameManager::lua["GetObjectIDs"] = Lua::GetObjectIDs;
        GameManager::lua["CreateObjectFromPrefab"] = Lua::CreateObjectFromPrefab;
        GameManager::lua["CreateCopyObject"] = Lua::CreateCopyObject;
        GameManager::lua["DeleteObject"] = Lua::DeleteObject;
        GameManager::lua["ObjectAddChild"] = Lua::ObjectAddChild;
        GameManager::lua["ObjectRemoveChild"] = Lua::ObjectRemoveChild;

        GameManager::lua["CheckCollision"] = sol::overload(
            Lua::CheckCollisionAll, Lua::CheckCollision);
        GameManager::lua["GetCollisionObject"] = Lua::GetCollisionObject;

        GameManager::lua["PlaySound"] = sol::overload(
            sol::resolve<void(std::string)>(Audio::b_PlaySound),
            sol::resolve<void(std::string, f32, f32, f32)>(Audio::b_PlaySound)
        );
        GameManager::lua["PlayMusic"] = sol::overload(
            sol::resolve<void(std::string)>(Audio::b_PlayMusic),
            sol::resolve<void(std::string, f32, f32, f32)>(Audio::b_PlayMusic)
        );
        GameManager::lua["IsMusicPlaying"] = [](std::string name) -> bool {
            if (Audio::music_list.find(name) != Audio::music_list.end()) {
                return IsMusicStreamPlaying(Audio::music_list[name].music);
            }
            return false;
        };
        GameManager::lua["PauseMusic"] = [](std::string name) {
            if (Audio::music_list.find(name) != Audio::music_list.end()) {
                PauseMusicStream(Audio::music_list[name].music);
            }
        };
        GameManager::lua["ResumeMusic"] = [](std::string name) {
            if (Audio::music_list.find(name) != Audio::music_list.end()) {
                ResumeMusicStream(Audio::music_list[name].music);
            }
        };
        GameManager::lua["StopMusic"] = [](std::string name) {
            if (Audio::music_list.find(name) != Audio::music_list.end()) {
                StopMusicStream(Audio::music_list[name].music);
            }
        };

        GameManager::lua["SetMasterVolume"] = [](f32 volume) {
            f32 finalVol = volume;
            if (finalVol > 1.0) finalVol = 1.0;
            if (finalVol < 0.0) finalVol = 0;
            Audio::masterVolume = finalVol;
        };
        GameManager::lua["SetEffectsVolume"] = [](f32 volume) {
            f32 finalVol = volume;
            if (finalVol > 1.0) finalVol = 1.0;
            if (finalVol < 0.0) finalVol = 0;
            Audio::effectVolume = finalVol;
        };
        GameManager::lua["SetMusicVolume"] = [](f32 volume) {
            f32 finalVol = volume;
            if (finalVol > 1.0) finalVol = 1.0;
            if (finalVol < 0.0) finalVol = 0;
            Audio::musicVolume = finalVol;
        };

        GameManager::lua["TestFunction"] = Lua::TestFunction;
    }

    void RegisterClasses() {
        sol::usertype<Vector2> vector2_type = GameManager::lua.new_usertype<Vector2>(
            "Vector2",
            sol::constructors<Vector2()>()
        );
        vector2_type["x"] = &Vector2::x;
        vector2_type["y"] = &Vector2::y;

        sol::usertype<Color> color_type = GameManager::lua.new_usertype<Color>(
            "Color",
            sol::constructors<Color()>()
        );
        color_type["r"] = &Color::r;
        color_type["g"] = &Color::g;
        color_type["b"] = &Color::b;
        color_type["a"] = &Color::a;

        sol::usertype<Entity> entity_type = GameManager::lua.new_usertype<Entity>(
            "Entity",
            sol::constructors<Entity()>(),
            sol::base_classes, sol::bases<GameObject>()
        );
        entity_type["ID"] = sol::readonly(&Entity::ID);
        entity_type["name"] = &Entity::name;
        entity_type["tag"] = &Entity::tag;
        entity_type["position"] = &Entity::position;
        entity_type["size"] = &Entity::size;
        entity_type["rotation"] = &Entity::rotation;
        entity_type["isVisible"] = &Entity::isVisible;
        entity_type["layer"] = sol::readonly(&Entity::layer);
        entity_type["texture"] = sol::readonly(&Entity::texturePath);
        entity_type["solid"] = &Entity::solid;
        entity_type["physicsObject"] = &Entity::physicsObject;
        entity_type["velocity"] = &Entity::velocity;
        entity_type["SetLayer"] = &Entity::SetLayer;
        entity_type["SetTexture"] = &Entity::SetTexture;
        entity_type["GetVariable"] = &Entity::GetVariable;
        entity_type["SetVariable"] = sol::overload(
            sol::resolve<void(std::string, f64)>(&Entity::SetVariable),
            sol::resolve<void(std::string, std::string)>(&Entity::SetVariable),
            sol::resolve<void(std::string, bool)>(&Entity::SetVariable)
        );
        entity_type["GetChildrenIDs"] = [](Entity* e) -> std::vector<u32> {
            std::vector<u32> ids;
            for (GameObject* child : e->children) {
                ids.push_back(child->ID);
            }
            return ids;
        };

        sol::usertype<TextObject> textobject_type = GameManager::lua.new_usertype<TextObject>(
            "TextObject",
            sol::constructors<TextObject()>(),
            sol::base_classes, sol::bases<GameObject>()
        );
        textobject_type["ID"] = sol::readonly(&TextObject::ID);
        textobject_type["name"] = &TextObject::name;
        textobject_type["tag"] = &TextObject::tag;
        textobject_type["position"] = &TextObject::position;
        textobject_type["size"] = &TextObject::size;
        textobject_type["rotation"] = &TextObject::rotation;
        textobject_type["isVisible"] = &TextObject::isVisible;
        textobject_type["layer"] = sol::readonly(&TextObject::layer);
        textobject_type["text"] = sol::readonly(&TextObject::text);
        textobject_type["color"] = &TextObject::color;
        textobject_type["fontSize"] = sol::readonly(&TextObject::fontSize);
        textobject_type["charSpacing"] = sol::readonly(&TextObject::charSpacing);
        textobject_type["SetLayer"] = &TextObject::SetLayer;
        textobject_type["SetText"] = [](TextObject* obj, std::string text) {
            obj->text = text;
            obj->CalculateSize();
        };
        textobject_type["SetSpacing"] = [](TextObject* obj, i32 spacing) {
            obj->charSpacing = spacing;
            obj->CalculateSize();
        };
        textobject_type["SetFont"] = &TextObject::SetFont;

        sol::usertype<GameCamera> camera_type = GameManager::lua.new_usertype<GameCamera>(
            "GameCamera",
            sol::constructors<GameCamera()>(),
            sol::base_classes, sol::bases<GameObject>()
        );
        camera_type["ID"] = sol::readonly(&GameCamera::ID);
        camera_type["name"] = &GameCamera::name;
        camera_type["tag"] = &GameCamera::tag;
        camera_type["position"] = sol::readonly(&GameCamera::position);
        camera_type["size"] = &GameCamera::size;
        camera_type["rotation"] = &GameCamera::rotation;
        camera_type["isVisible"] = &GameCamera::isVisible;
        camera_type["layer"] = sol::readonly(&GameCamera::layer);
        camera_type["SetLayer"] = &GameCamera::SetLayer;
        camera_type["SetPosition"] = [](GameCamera* cam, f32 x, f32 y) {
            cam->SetPosition({x, y});
        };
        camera_type["MoveCamera"] = [](GameCamera* cam, f32 dx, f32 dy) {
            cam->MoveCamera({dx, dy});
        };
        camera_type["SetActive"] = &GameCamera::SetActive;
    }

    sol::object CreateLuaObject(GameObject* obj) {
        if (obj == nullptr) {
            GameManager::ConsoleError("Could not create Lua object: object does not exist");
            return sol::make_object(GameManager::lua, sol::nil);
        }

        if (obj->type == ENTITY) {
            Entity* e = (Entity*)obj;
            return sol::make_object(GameManager::lua, e);
        }
        else if (obj->type == TEXT) {
            TextObject* text = (TextObject*)obj;
            return sol::make_object(GameManager::lua, text);
        }
        else if (obj->type == CAMERA) {
            GameCamera* cam = (GameCamera*)obj;
            return sol::make_object(GameManager::lua, cam);
        }
        else {
            GameManager::ConsoleError("Could not create Lua object: object has unknown type");
            return sol::make_object(GameManager::lua, sol::nil);
        }
    }


    // ----------------------------------------------------------------


    // Input
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

    bool GetMouseWheelScroll(std::string direction) {
        float result = GameManager::MouseWheelDirection;
        
        if (result == 1 && direction == "UP") {
            return true;
        }
        else if (result == -1 && direction == "DOWN") {
            return true;
        }
        else {
            return false;
        }
    }

    sol::object GetMousePosition() {
        return sol::make_object(GameManager::lua, GameManager::WorldMousePosition);
    }


    // ----------------------------------------------------------------


    sol::object CreateObjectFromPrefab(std::string path) {
        std::ifstream infile(path);
        nlohmann::json data = nlohmann::json::parse(infile);
        
        if (data["type"] == 1) {
            Entity* entity = new Entity();
            entity->LoadEntityPrefab(data);
            return sol::make_object(GameManager::lua, entity);
        }
        else if (data["type"] == 2) {
            TextObject* text = new TextObject();
            text->LoadTextObjectPrefab(data);
            return sol::make_object(GameManager::lua, text);
        }
        else if (data["type"] == 3) {
            GameCamera* cam = new GameCamera();
            cam->LoadCameraPrefab(data);
            return sol::make_object(GameManager::lua, cam);
        }
        else {
            GameManager::ConsoleError("Invalid object type");
            return sol::make_object(GameManager::lua, sol::nil);
        }
    }

    sol::object CreateCopyObject(u32 id) {
        if (id >= GameManager::GameObjects.size()) {
            GameManager::ConsoleError("Object with ID=" + std::to_string(id)
                + " does not exist");
            return sol::make_object(GameManager::lua, sol::nil);
        }

        GameObject* object = GameManager::GameObjects[id];

        if (object == nullptr) {
            GameManager::ConsoleError("Could not find object with ID=" + std::to_string(id));
            return sol::make_object(GameManager::lua, sol::nil);
        } 

        if (object->type == ENTITY) {
            Entity* e = (Entity*)object;
            Entity* newEntity = new Entity(e);
            return sol::make_object(GameManager::lua, newEntity);
        }
        else if (object->type == TEXT) {
            TextObject* text = (TextObject*)object;
            TextObject* newText = new TextObject(text);
            return sol::make_object(GameManager::lua, newText);
        }
        else if (object->type == CAMERA) {
            GameCamera* camera = (GameCamera*)object;
            GameCamera* newCam = new GameCamera(camera);
            return sol::make_object(GameManager::lua, newCam);
        }
        else {
            GameManager::ConsoleError("Invalid object type");
            return sol::make_object(GameManager::lua, sol::nil);
        }
    }

    void DeleteObject(u32 id) {
        if (id >= GameManager::GameObjects.size()) {
            GameManager::ConsoleError("Object with ID=" + std::to_string(id)
                + " does not exist");
            return;
        }

        GameObject* object = GameManager::GameObjects[id];
        
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
    
    void ObjectAddChild(u32 parentID, u32 childID) {
        // Make sure objects with the given IDs exist
        if (parentID >= GameManager::GameObjects.size()) {
            GameManager::ConsoleError("Parent ID=" + std::to_string(parentID) + " does not exist");
            return;
        }
        if (childID >= GameManager::GameObjects.size()) {
            GameManager::ConsoleError("Child ID=" + std::to_string(childID) + " does not exist");
            return;
        }
        
        // Get Objects
        GameObject* object = GameManager::GameObjects[parentID];
        GameObject* child = GameManager::GameObjects[childID];

        // Remove from current parent if it has one
        if (child->parent != nullptr) {
            GameObject* parent = child->parent;
            for (size_t i = 0; i < parent->children.size(); i++) {
                if (parent->children[i]->ID == child->ID) {
                    parent->children.erase(parent->children.begin() + i);
                    break;
                }
            }
        }
        child->parent = nullptr;
        
        // Add to new parent
        object->children.push_back(child);
        child->parent = object;
    }

    void ObjectRemoveChild(u32 parentID, u32 childID) {
        // Make sure objects with the given IDs exist
        if (parentID >= GameManager::GameObjects.size()) {
            GameManager::ConsoleError("Parent ID=" + std::to_string(parentID) + " does not exist");
            return;
        }
        if (childID >= GameManager::GameObjects.size()) {
            GameManager::ConsoleError("Child ID=" + std::to_string(childID) + " does not exist");
            return;
        }

        // Get Objects
        GameObject* parent = GameManager::GameObjects[parentID];
        GameObject* child = GameManager::GameObjects[childID];

        // Remove child from parent
        bool found = false;
        for (size_t i = 0; i < parent->children.size(); i++) {
            GameObject* obj = parent->children[i];
            if (obj->ID == child->ID) {
                parent->children.erase(parent->children.begin() + i);
                found = true;
                break;
            }
        }
        // If the user made a mistake, and the given parentID is not actually
        // the parent of childID, we don't want to change anything.
        if (found) {
            child->parent = nullptr;
        }
        else {
            GameManager::ConsoleError("Object with ID=" + std::to_string(childID) + 
                " is not a child of object with ID=" + std::to_string(parentID));
        }
    }


    // ----------------------------------------------------------------


    u32 GetObjectIDByName(std::string name) {
        for (GameObject* obj : GameManager::GameObjects) {
            if (obj->name == name) return obj->ID;
        }
        GameManager::ConsoleError("Object with name " + name + " does not exist");
        return UINT32_MAX;
    }

    sol::object GetObjectByID(u32 id) {
        if (id >= GameManager::GameObjects.size()) {
            GameManager::ConsoleError("Object with ID=" + std::to_string(id)
                + " does not exist");
            return sol::make_object(GameManager::lua, sol::nil);
        }

        GameObject* object = GameManager::GameObjects[id];
        return Lua::CreateLuaObject(object); 
    }

    sol::object GetObjectByName(std::string name) {
        for (GameObject* object : GameManager::GameObjects) {
            if (object->name != name) continue;
            
            sol::object luaObject = Lua::CreateLuaObject(object);
            return luaObject;
        }
        GameManager::ConsoleError("Could not find object with name " + name);
        return sol::make_object(GameManager::lua, sol::nil);
    }

    std::vector<u32> GetObjectIDs() {
        std::vector<u32> ids;

        for (GameObject* obj : GameManager::GameObjects) {
            ids.push_back(obj->ID);
        }

        return ids;
    }

    
    // ----------------------------------------------------------------


    bool CheckCollisionAll(u32 id) {
        if (id >= GameManager::GameObjects.size()) {
            GameManager::ConsoleError("Object with ID=" + std::to_string(id)
                + " does not exist");
            return false;
        }

        GameObject* obj = GameManager::GameObjects[id];
        
        if (obj == nullptr) {
            GameManager::ConsoleError("No object with ID=" + std::to_string(id) + " exists");
            return false;
        }

        Rectangle objRect = {obj->position.x, obj->position.y, obj->size.x, obj->size.y};

        for (GameObject* other : GameManager::GameObjects) {
            if (other->ID == id) continue;
            if (other->type == TEXT || other->type == CAMERA) continue;

            Rectangle otherRect = {other->position.x, other->position.y, other->size.x, other->size.y};
            if (CheckCollisionRecs(objRect, otherRect)) {
                return true;
            }
        }
        return false;
    }

    bool CheckCollision(u32 id1, u32 id2) {
        if (id1 >= GameManager::GameObjects.size()) {
            GameManager::ConsoleError("Object with ID=" + std::to_string(id1)
                + " does not exist");
            return false;
        }
        if (id2 >= GameManager::GameObjects.size()) {
            GameManager::ConsoleError("Object with ID=" + std::to_string(id1)
                + " does not exist");
            return false;
        }

        GameObject* obj1 = GameManager::GameObjects[id1];
        if (obj1 == nullptr) {
            GameManager::ConsoleError("No object with ID=" + std::to_string(id1) + " exists");
            return false;
        }

        GameObject* obj2 = GameManager::GameObjects[id2];
        if (obj2 == nullptr) {
            GameManager::ConsoleError("No object with ID=" + std::to_string(id2) + " exists");
            return false;
        }

        Rectangle r1 = {obj1->position.x, obj1->position.y, obj1->size.x, obj1->size.y};
        Rectangle r2 = {obj2->position.x, obj2->position.y, obj2->size.x, obj2->size.y};

        return CheckCollisionRecs(r1, r2);
    }

    sol::object GetCollisionObject(u32 id) {
        if (id >= GameManager::GameObjects.size()) {
            GameManager::ConsoleError("Object with ID=" + std::to_string(id)
                + " does not exist");
            return sol::make_object(GameManager::lua, sol::nil);
        }

        GameObject* obj = GameManager::GameObjects[id];
        if (obj == nullptr) {
            GameManager::ConsoleError("No object with ID=" + std::to_string(id) + " exists");
            return sol::make_object(GameManager::lua, sol::nil);
        }

        Rectangle r1 = {obj->position.x, obj->position.y, obj->size.x, obj->size.y}; 

        for (GameObject* other : GameManager::GameObjects) {
            if (other->ID == id) continue;
            if (other->type == TEXT || other->type == CAMERA) continue; 

            Rectangle r2 = {other->position.x, other->position.y, other->size.x, other->size.y};
            if (CheckCollisionRecs(r1, r2)) {
                if (other->type == ENTITY) {
                    Entity* e = (Entity*)other;
                    return sol::make_object(GameManager::lua, e);
                }
                else if (other->type == TEXT) {
                    TextObject* text = (TextObject*)other;
                    return sol::make_object(GameManager::lua, text);
                }
                else if (other->type == CAMERA) {
                    GameCamera* cam = (GameCamera*)other;
                    return sol::make_object(GameManager::lua, cam);
                }
                else {
                    GameManager::ConsoleError("Object with ID=" + 
                        std::to_string(other->ID) + "has unknown type");
                    return sol::make_object(GameManager::lua, sol::nil);
                }
            }
        }
        return sol::make_object(GameManager::lua, sol::nil);
    }

    
    // ----------------------------------------------------------------


    void TestFunction() {
        Entity* player = GameManager::Entities[0];
        player->variables["health"].numval++; 
    }
}