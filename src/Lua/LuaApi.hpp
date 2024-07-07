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
int get_text(lua_State* L);
int set_text_position(lua_State* L);
int get_text_position(lua_State* L);
int set_text_target(lua_State* L);
int get_text_target(lua_State* L);
int set_text_visible(lua_State* L);
int get_text_visible(lua_State* L);
int set_text_color(lua_State* L);
int get_text_color(lua_State* L);
int set_text_size(lua_State* L);
int get_text_size(lua_State* L);
int create_text(lua_State* L);

// Input
int get_input(lua_State* L);
int get_input_single(lua_State* L);
int get_mouse_input(lua_State* L);
int get_mouse_input_single(lua_State* L);

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
int get_clicked(lua_State* L);
int get_clicked_single(lua_State* L);
int get_sprite(lua_State* L);
int set_sprite(lua_State* L);

int get_entity_position(lua_State* L);
int set_entity_position(lua_State* L);
int get_entity_visible(lua_State* L);
int set_entity_visible(lua_State* L);

int get_entity_by_name(lua_State* L);
int get_entities_by_type(lua_State* L);

int create_entity(lua_State* L);
int check_collision(lua_State* L);
int check_collision_side(lua_State* L);

#endif