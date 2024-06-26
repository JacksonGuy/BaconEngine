#ifndef LUA_API_H
#define LUA_API_H

extern "C" {
    #include <lauxlib.h>
    #include <lua.h>
    #include <lualib.h>
}

int ConsoleWrite(lua_State* L);
int change_variable(lua_State* L);
int get_variable(lua_State* L);

#endif