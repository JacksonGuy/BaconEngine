#include "LuaApi.hpp"

#include "../GameManager.hpp"

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

int get_position(lua_State* L) {
    sf::Vector2f pos = GameManager::current_lua_object->position;
    lua_pushinteger(L, pos.x);
    lua_pushinteger(L, pos.y);
    return 2;
}

int set_position(lua_State* L) {
    double x = lua_tonumber(L, 1);
    double y = lua_tonumber(L, 2);

    GameManager::current_lua_object->SetPosition(sf::Vector2f(x, y));

    return 0;
}

int set_entity_variable(lua_State* L) {
    std::string varName = lua_tostring(L, 2);
    Entity* target = nullptr; 
    if (lua_isnumber(L, 1)) {
        target = GameManager::FindEntityByID(lua_tonumber(L, 1));
    }
    else if (lua_isstring(L, 1)) {
        target = GameManager::FindEntityByName(lua_tostring(L, 1));
    }

    // If we found entity
    if (target != nullptr) {
        if (lua_isnumber(L, 3)) {
            target->entity_numbers[varName] = lua_tonumber(L, 3);
        }
        else if (lua_isstring(L, 3)) {
            target->entity_strings[varName] = lua_tostring(L, 3);
        }
    }

    return 0;
}

int get_entity_variable(lua_State* L) {
    std::string varName = lua_tostring(L, 2);
    Entity* target = nullptr;
    if (lua_isnumber(L, 1)) {
        target = GameManager::FindEntityByID(lua_tonumber(L, 1));
    }
    else if (lua_isstring(L, 1)) {
        target = GameManager::FindEntityByName(lua_tostring(L, 1));
    }

    if (target != nullptr) {
        if (target->entity_numbers.find(varName) != target->entity_numbers.end()) {
            lua_pushnumber(L, target->entity_numbers[varName]);
        }
        else if (target->entity_strings.find(varName) != target->entity_strings.end()) {
            lua_pushstring(L, target->entity_strings[varName].c_str());
        }
        return 1;
    }

    return 0;
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

int get_entity_position(lua_State* L) {
    int id = lua_tointeger(L, 1);

    Entity* e = GameManager::FindEntityByID(id);
    if (e != nullptr) {
        lua_pushnumber(L, e->position.x);
        lua_pushnumber(L, e->position.y);
        return 2;
    }
    return 0;
}

int set_entity_position(lua_State* L) {
    int id = lua_tointeger(L, 1);
    Entity* e = GameManager::FindEntityByID(id);

    if (e != nullptr) {
        int x = lua_tonumber(L, 2);
        int y = lua_tonumber(L, 3);
        e->SetPosition(sf::Vector2f(x,y));
    }
    return 0;
}

int create_entity(lua_State* L) {
    std::string prefab = lua_tostring(L, 1);
    int id = loadPrefab(prefab);
    lua_pushinteger(L, id);
    return 1;
}
int set_visible(lua_State* L) {
    bool v = lua_toboolean(L, 1);
    GameManager::current_lua_object->isVisible = v;
    return 0;
}

int get_visible(lua_State* L) {
    bool v = GameManager::current_lua_object->isVisible;
    lua_pushboolean(L, v);
    return 1;
}

int set_entity_visible(lua_State* L) {
    int id = lua_tonumber(L, 1);
    bool val = lua_toboolean(L, 2);

    Entity* e = GameManager::FindEntityByID(id);
    e->isVisible = val;

    return 0;
}

int get_entity_visible(lua_State* L) {
    int id = lua_tonumber(L, 1);

    Entity* e = GameManager::FindEntityByID(id);
    lua_pushboolean(L, e->isVisible);

    return 1;
}

int get_clicked(lua_State* L) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (GameManager::current_lua_object->rect.contains(GameManager::mousePos)) {
            lua_pushboolean(L, 1);
            return 1;
        }
    }
    lua_pushboolean(L, 0);
    return 1;
}

int get_clicked_single(lua_State* L) {
    if (GameManager::lastMouseInput == sf::Mouse::Button::Left) {
        if (GameManager::current_lua_object->rect.contains(GameManager::mousePos)) {
            lua_pushboolean(L, 1);
            GameManager::lastMouseInput = sf::Mouse::Button(-1);
            return 1;
        }
    }
    lua_pushboolean(L, 0);
    return 1;
}

int get_sprite(lua_State* L) {
    lua_pushstring(L, GameManager::current_lua_object->texturePath.data());
    return 1;
}

int set_sprite(lua_State* L) {
    std::string path = lua_tostring(L, 1);
    GameManager::current_lua_object->SetSprite(path);
    return 0;
}