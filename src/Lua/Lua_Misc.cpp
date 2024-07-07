#include "LuaApi.hpp"

#include "../GameManager.hpp"

int ConsoleWrite(lua_State* L) {
    const char* text = lua_tostring(L, 1);

    GameManager::ConsoleWrite(text);

    return 0;
}


int get_input(lua_State* L) {
    const char* input = lua_tostring(L, 1);
    if (!GameManager::windowHasFocus) {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (sf::Keyboard::isKeyPressed(GameManager::key_map[input])) {
        lua_pushboolean(L, 1);
    }
    else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

int get_input_single(lua_State* L) {
    const char* input = lua_tostring(L, 1);
    sf::Keyboard::Key key = GameManager::key_map[input];

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

    if (sf::Mouse::isButtonPressed(GameManager::mouse_map[input])) {
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
    sf::Mouse::Button button = GameManager::mouse_map[input];
    
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

int check_collision(lua_State* L) {
    bool collision = GameManager::checkCollision(*GameManager::current_lua_object);
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
        collision = GameManager::checkCollisionSide(GameManager::current_lua_object->topRect);
    }
    else if (side == "BOTTOM") {
        collision = GameManager::checkCollisionSide(GameManager::current_lua_object->bottomRect);
    }
    else if (side == "LEFT") {
        collision = GameManager::checkCollisionSide(GameManager::current_lua_object->leftRect);
    }
    else if (side == "RIGHT") {
        collision = GameManager::checkCollisionSide(GameManager::current_lua_object->rightRect);
    }

    if (collision) {
        lua_pushboolean(L, 1);
    }
    else {
        lua_pushboolean(L, 0);
    }

    return 1;
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

int get_entities_by_type(lua_State* L) {
    std::string type = lua_tostring(L, 1);

    std::vector<Entity*> found = GameManager::FindEntitiesByType(type);
    if (found.size() > 0) {
        lua_createtable(L, 0, found.size());
        for (size_t i = 0; i < found.size(); i++) {
            Entity* e = found[i];
            int id = e->ID;

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