#include "LuaApi.hpp"

#include "GameManager.hpp" 

int ConsoleWrite(lua_State* L) {
    const char* text = lua_tostring(L, 1);

    GameManager::ConsoleWrite(text);

    return 0;
}

int change_variable(lua_State* L) {
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