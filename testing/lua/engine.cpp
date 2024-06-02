#include <iostream>

#include "../include/lua/lauxlib.h"
#include "../include/lua/lua.h"
#include "../include/lua/lualib.h"

int main() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    luaL_dofile(L, "test.lua");

    return 0;
}