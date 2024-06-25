#include "LuaApi.hpp"

#include "GameManager.hpp" 

int ConsoleWrite(lua_State* L) {
    const char* text = lua_tostring(L, 1);

    GameManager::ConsoleWrite(text);

    return 0;
}