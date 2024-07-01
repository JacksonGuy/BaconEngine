#ifndef LUA_API_H
#define LUA_API_H

extern "C" {
    #include <lauxlib.h>
    #include <lua.h>
    #include <lualib.h>
}

// Misc Engine Tools
int ConsoleWrite(lua_State* L);

// Text
int set_text(lua_State* L);

// Input
int get_input(lua_State* L);
int get_mouse_input(lua_State* L);

// Entity Custom Variables
int set_variable(lua_State* L);
int get_variable(lua_State* L);
int set_entity_variable(lua_State* L);
int get_entity_variable(lua_State* L);

// Entity Standard Variables
int get_position(lua_State* L);
int set_position(lua_State* L);
int get_velocity(lua_State* L);
int set_velocity(lua_State* L);
int get_acceleration(lua_State* L);
int get_grounded(lua_State* L);
int set_visible(lua_State* L);
int get_visible(lua_State* L);

int get_entity_position(lua_State* L);
int set_entity_position(lua_State* L);
int get_entity_visible(lua_State* L);
int set_entity_visible(lua_State* L);

// Get Entity
int get_entity_by_name(lua_State* L);
int get_entities_by_type(lua_State* L);

// Entity Tools
int create_entity(lua_State* L);//
int check_collision(lua_State* L);
int check_collision_side(lua_State* L);

#endif