#include "LuaApi.hpp"

#include "../GameManager.hpp"
#include "../Input.hpp"

namespace Lua {
    lua_State* LuaState = luaL_newstate();
    Entity* current_lua_object = nullptr;

    void RegisterFunctions() {
        luaL_openlibs(LuaState);
        lua_register(LuaState, "ConsoleWrite", ConsoleWrite);
        lua_register(LuaState, "get_time_seconds", get_time_seconds);

        lua_register(LuaState, "set_variable", set_variable);
        lua_register(LuaState, "get_variable", get_variable);
        lua_register(LuaState, "get_entity_variable", get_entity_variable);
        lua_register(LuaState, "set_entity_variable", set_entity_variable);

        lua_register(LuaState, "set_text", set_text);
        lua_register(LuaState, "get_text", get_text);
        lua_register(LuaState, "set_text_position", set_text_position);
        lua_register(LuaState, "get_text_position", get_text_position);
        lua_register(LuaState, "set_text_visible", set_text_visible);
        lua_register(LuaState, "get_text_visible", get_text_visible);
        lua_register(LuaState, "set_text_color", set_text_color);
        lua_register(LuaState, "get_text_color", get_text_color);
        lua_register(LuaState, "set_text_size", set_text_size);
        lua_register(LuaState, "get_text_size", get_text_size);
        lua_register(LuaState, "set_text_name", set_text_name);
        lua_register(LuaState, "get_text_name", get_text_name);
        lua_register(LuaState, "create_text", create_text);
        
        lua_register(LuaState, "get_input", get_input);
        lua_register(LuaState, "get_mouse_input", get_mouse_input);
        lua_register(LuaState, "get_input_single", get_input_single);
        lua_register(LuaState, "get_mouse_input_single", get_mouse_input_single);
        
        lua_register(LuaState, "get_position", get_position);
        lua_register(LuaState, "set_position", set_position);
        lua_register(LuaState, "get_velocity", get_velocity);
        lua_register(LuaState, "set_velocity", set_velocity);
        lua_register(LuaState, "get_acceleration", get_acceleration);
        lua_register(LuaState, "set_acceleration", set_acceleration);
        lua_register(LuaState, "get_grounded", get_grounded);
        lua_register(LuaState, "set_visible", set_visible);
        lua_register(LuaState, "get_visible", get_visible);
        lua_register(LuaState, "get_clicked", get_clicked);
        lua_register(LuaState, "get_clicked_single", get_clicked_single);
        lua_register(LuaState, "get_sprite", get_sprite);
        lua_register(LuaState, "set_sprite", set_sprite);
        lua_register(LuaState, "get_name", get_name);
        lua_register(LuaState, "set_name", set_name);
        lua_register(LuaState, "get_tag", get_tag);
        lua_register(LuaState, "set_tag", set_tag);
        
        lua_register(LuaState, "get_entity_by_name", get_entity_by_name);
        lua_register(LuaState, "get_entities_by_tag", get_entities_by_tag);
        lua_register(LuaState, "get_entity_list", get_entity_list);

        lua_register(LuaState, "get_entity_position", get_entity_position);
        lua_register(LuaState, "set_entity_position", set_entity_position);
        lua_register(LuaState, "get_entity_visible", get_entity_visible);
        lua_register(LuaState, "set_entity_visible", set_entity_visible);
        lua_register(LuaState, "get_entity_name", get_entity_name);
        lua_register(LuaState, "set_entity_name", set_entity_name);

        lua_register(LuaState, "create_entity", create_entity);
        lua_register(LuaState, "check_collision", check_collision);
        lua_register(LuaState, "check_collision_side", check_collision_side);
        lua_register(LuaState, "get_collision_entities", get_collision_entities);
        lua_register(LuaState, "delete_entity", delete_entity);
        
        lua_register(LuaState, "play_sound", play_sound);
        lua_register(LuaState, "play_music", play_music);
        lua_register(LuaState, "pause_music", pause_music);
        lua_register(LuaState, "stop_music", stop_music);
        lua_register(LuaState, "set_master_volume", set_master_volume);
        lua_register(LuaState, "set_effects_volume", set_effects_volume);
        lua_register(LuaState, "set_music_volume", set_music_volume);

        lua_register(LuaState, "set_camera", set_camera);
        lua_register(LuaState, "get_current_camera_ID", get_current_camera_ID);
        lua_register(LuaState, "get_current_camera_name", get_current_camera_name);

        lua_register(LuaState, "save_game", save_game);
        lua_register(LuaState, "load_game", load_game);
    }

    void RunLuaUpdates() {
        for (Entity* e : GameManager::Entities) {
            current_lua_object = e;
            for (ScriptItem script : e->lua_scripts) {
                if (luaL_dofile(Lua::LuaState, script.path.c_str()) != LUA_OK) {
                    std::string error = lua_tostring(LuaState, -1);
                    GameManager::ConsoleWrite("[ERROR] Lua Script failed: " + error);
                }
            }
        }
        current_lua_object = nullptr; // Just in case
    }

    int ConsoleWrite(lua_State* L) {
        if (lua_isstring(L, 1)) {
            const char* text = lua_tostring(L, 1);
            GameManager::ConsoleWrite(text);
        }
        else if (lua_isnumber(L, 1)) {
            double num = lua_tonumber(L, 1);
            GameManager::ConsoleWrite(std::to_string(num));
        }
        else if (lua_isnil(L, 1)) {
            GameManager::ConsoleWrite("Nil");
        }
        else {
            GameManager::ConsoleWrite("[ERROR] Can't write value to console");
        }

        return 0;
    }

    int get_time_seconds(lua_State* L) {
        float time = GameManager::clock.getElapsedTime().asSeconds();
        // GameManager::clock.restart();
        lua_pushnumber(L, time);
        return 1;
    }


    int get_input(lua_State* L) {
        const char* input = lua_tostring(L, 1);
        if (!GameManager::windowHasFocus) {
            lua_pushboolean(L, 0);
            return 1;
        }

        // if (sf::Keyboard::isKeyPressed(Input::key_map[input])) {
        if (GameManager::keypresses[Input::key_map[input]]) {
            lua_pushboolean(L, 1);
        }
        else {
            lua_pushboolean(L, 0);
        }

        return 1;
    }

    int get_input_single(lua_State* L) {
        const char* input = lua_tostring(L, 1);
        sf::Keyboard::Key key = Input::key_map[input];

        if (!GameManager::windowHasFocus) {
            lua_pushboolean(L, 0);
            return 1;
        }

        if (key == GameManager::lastKeyboardInput) {
            lua_pushboolean(L, 1);
            GameManager::lastKeyboardInput = sf::Keyboard::Key(-1);
        }
        else {
            lua_pushboolean(L, 0);
        }
        return 1;
    }

    int get_mouse_input(lua_State* L) {
        const char* input = lua_tostring(L, 1);

        if (!GameManager::windowHasFocus) {
            lua_pushboolean(L, 0);
            return 1;
        }

        if (sf::Mouse::isButtonPressed(Input::mouse_map[input])) {
            lua_pushboolean(L, 1);
        }
        else {
            // if (input == "MOUSE_SCROLL_UP") {
            //     // Something?
            // }

            lua_pushboolean(L, 0);
        }

        return 1;
    }

    int get_mouse_input_single(lua_State* L) {
        const char* input = lua_tostring(L, 1);
        sf::Mouse::Button button = Input::mouse_map[input];
        
        if (!GameManager::windowHasFocus) {
            lua_pushboolean(L, 0);
            return 1;
        }

        if (button == GameManager::lastMouseInput) {
            lua_pushboolean(L, 1);
            GameManager::lastMouseInput = sf::Mouse::Button(-1);
        }
        else {
            lua_pushboolean(L, 0);
        }

        return 1;
    }

    int get_mouse_position(lua_State* L) {
        sf::Vector2f pos = GameManager::mousePos;
        lua_pushnumber(L, pos.x);
        lua_pushnumber(L, pos.y);
        return 2;
    }

    int check_collision(lua_State* L) {
        bool collision = GameManager::checkCollision(*current_lua_object);
        if (collision) {
            lua_pushboolean(L, 1);
        }
        else {
            lua_pushboolean(L, 0);
        }

        return 1;
    }

    int check_collision_side(lua_State* L) {
        std::string side = lua_tostring(L, 1);
        bool collision = false;

        if (side == "TOP") {
            collision = GameManager::checkCollisionSide(*current_lua_object, "TOP");
        }
        else if (side == "BOTTOM") {
            collision = GameManager::checkCollisionSide(*current_lua_object, "BOTTOM");
        }
        else if (side == "LEFT") {
            collision = GameManager::checkCollisionSide(*current_lua_object, "LEFT");
        }
        else if (side == "RIGHT") {
            collision = GameManager::checkCollisionSide(*current_lua_object, "RIGHT");
        }
        else {
            GameManager::ConsoleWrite("[ERROR] Not a valid side!");
            return 0;
        }

        if (collision) {
            lua_pushboolean(L, 1);
        }
        else {
            lua_pushboolean(L, 0);
        }

        return 1;
    }

    int get_collision_entities(lua_State* L) {
        std::vector<Entity*> entities = GameManager::getCollidingWith(*current_lua_object);

        if (entities.size() > 0) {
            lua_createtable(L, 0, entities.size());
            for (size_t i = 0; i < entities.size(); i++) {
                Entity* e = entities[i];
                
                lua_pushinteger(L, i+1);
                lua_pushinteger(L, e->ID);
                lua_settable(L, -3);
            }
            return 1;
        }
        else {
            return 0;
        }
    }

    int get_entity_by_name(lua_State* L) {
        std::string name = lua_tostring(L, 1);

        for (Entity* e : GameManager::Entities) {
            if (e->name == name) {
                lua_pushnumber(L, e->ID);
                return 1;
            }
        }

        return 0;
    }

    int get_entities_by_tag(lua_State* L) {
        std::string tag = lua_tostring(L, 1);

        std::vector<Entity*> found = GameManager::FindEntitiesByTag(tag);
        if (found.size() > 0) {
            lua_createtable(L, 0, found.size());
            for (size_t i = 0; i < found.size(); i++) {
                Entity* obj = found[i];
                int id = obj->ID;

                lua_pushinteger(L, i+1);
                lua_pushinteger(L, id);
                lua_settable(L, -3);
            }
            return 1;
        }
        else {
            return 0;
        }
    }

    int get_entity_list(lua_State* L) {
        lua_createtable(L, 0, GameManager::Entities.size());
        for (size_t i = 0; i < GameManager::Entities.size(); i++) {
            Entity* e = GameManager::Entities[i];

            lua_pushinteger(L, i+1);
            lua_pushnumber(L, e->ID);
            lua_settable(L, -3);
        }
        return 1;
    }

    int set_camera(lua_State* L) { 
        if (lua_isstring(L, 1)) {
            std::string name = lua_tostring(L, 1);

            for (Camera* camera : GameManager::Cameras) {
                if (camera->name == name) {
                    GameManager::camera = camera;
                    break;
                }
            }
        }
        else if (lua_isnumber(L, 1)) {
            unsigned int id = lua_tonumber(L, 1);

            for (Camera* camera : GameManager::Cameras) {
                if (camera->ID == id) {
                    GameManager::camera = camera;
                    break;
                }
            }
        }

        return 0;
    }

    int get_current_camera_ID(lua_State* L) {
        lua_pushnumber(L, GameManager::camera->ID);
        return 1;
    }

    int get_current_camera_name(lua_State* L) {
        lua_pushstring(L, GameManager::camera->name.data());
        return 1;
    }

    int save_game(lua_State* L) {
        std::string file = lua_tostring(L, 1);
        File::save(file);
        return 0;
    }

    int load_game(lua_State* L) {
        std::string file = lua_tostring(L, 1);
        bool result = File::load(file);
        if (result == false) {
            GameManager::ConsoleWrite("[ERROR] Failed to load save file!");
        }
        return 0;
    }
}