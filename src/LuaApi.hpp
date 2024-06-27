#ifndef LUA_API_H
#define LUA_API_H

extern "C" {
    #include <lauxlib.h>
    #include <lua.h>
    #include <lualib.h>
}

int ConsoleWrite(lua_State* L);

int set_variable(lua_State* L);
int get_variable(lua_State* L);
int set_text(lua_State* L);
int get_input(lua_State* L);

int get_position(lua_State* L);
int set_position(lua_State* L);
int get_velocity(lua_State* L);
int set_velocity(lua_State* L);
int get_acceleration(lua_State* L);
int get_grounded(lua_State* L);

int check_collision(lua_State* L);
int check_collision_side(lua_State* L);

#endif