#include "LuaApi.hpp"

#include "GameManager.hpp" 

int ConsoleWrite(lua_State* L) {
    const char* text = lua_tostring(L, 1);

    GameManager::ConsoleWrite(text);

    return 0;
}

int set_variable(lua_State* L) {
    const char* name = lua_tostring(L, 1);
    if (lua_isnumber(L, 2)) {
        GameManager::current_lua_object->entity_numbers[name] = lua_tonumber(L, 2);
    }
    else if (lua_isstring(L, 2)) {
        std::string str = lua_tostring(L, 2);
        GameManager::current_lua_object->entity_strings[name] = str.data();
    }

    return 0;
}

int get_variable(lua_State* L) {
    const char* name = lua_tostring(L, 1);
    if (GameManager::current_lua_object->entity_numbers.find(name)
        != GameManager::current_lua_object->entity_numbers.end()) {
            lua_pushnumber(L, GameManager::current_lua_object->entity_numbers[name]);
    }
    else if (GameManager::current_lua_object->entity_strings.find(name)
        != GameManager::current_lua_object->entity_strings.end()) {
            char* str = GameManager::current_lua_object->entity_strings[name].data();
            lua_pushstring(L, str);
    }

    return 1;
}

int set_text(lua_State* L) {
    const char* name = lua_tostring(L, 1);
    const char* newtext = lua_tostring(L, 2);

    for (TextObj* text : GameManager::TextObjects) {
        if (text->name == name) {
            text->text.setString(newtext);
            return 0;
        }
    }

    return 0;
}

int get_input(lua_State* L) {
    const char* input = lua_tostring(L, 1);
    if (sf::Keyboard::isKeyPressed(GameManager::key_map[input])) {
        lua_pushboolean(L, 1);
    }
    else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

int get_mouse_input(lua_State* L) {
    const char* input = lua_tostring(L, 1);
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

int get_position(lua_State* L) {
    sf::Vector2f pos = GameManager::current_lua_object->position;
    lua_pushinteger(L, pos.x);
    lua_pushinteger(L, pos.y);
    return 2;
}

int set_position(lua_State* L) {
    double x = lua_tonumber(L, 1);
    double y = lua_tonumber(L, 2);

    GameManager::current_lua_object->position = sf::Vector2f(x, y);

    return 0;
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

int get_velocity(lua_State* L) {
    sf::Vector2f vel = GameManager::current_lua_object->velocity;

    lua_pushnumber(L, vel.x);
    lua_pushnumber(L, vel.y);

    return 2;
}

int set_velocity(lua_State* L) {
    double x = lua_tonumber(L, 1);
    double y = lua_tonumber(L, 2);

    GameManager::current_lua_object->velocity = sf::Vector2f(x, y);

    return 0;
}

int get_acceleration(lua_State* L) {
    sf::Vector2f acc = GameManager::current_lua_object->acceleration;

    lua_pushnumber(L, acc.x);
    lua_pushnumber(L, acc.y);

    return 2;
}

int get_grounded(lua_State* L) {
    lua_pushboolean(L, GameManager::current_lua_object->grounded);
    return 1;
}