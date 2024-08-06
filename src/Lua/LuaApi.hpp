#ifndef LUA_API_H
#define LUA_API_H

extern "C" {
    #include <lauxlib.h>
    #include <lua.h>
    #include <lualib.h>
}

#include "../Entity.hpp"

namespace Lua {
    extern lua_State* LuaState;
    extern Entity* current_lua_object;
    void RegisterFunctions();
    void RunLuaUpdates();

    // Misc Engine Tools
    int ConsoleWrite(lua_State* L);
    int get_time_seconds(lua_State* L);

    // Text
    int set_text(lua_State* L);
    int get_text(lua_State* L);
    int set_text_position(lua_State* L);
    int get_text_position(lua_State* L);
    int set_text_visible(lua_State* L);
    int get_text_visible(lua_State* L);
    int set_text_color(lua_State* L);
    int get_text_color(lua_State* L);
    int set_text_size(lua_State* L);
    int get_text_size(lua_State* L);
    int get_text_name(lua_State* L);
    int set_text_name(lua_State* L);
    int create_text(lua_State* L);

    // Input
    int get_input(lua_State* L);
    int get_input_single(lua_State* L);
    int get_mouse_input(lua_State* L);
    int get_mouse_input_single(lua_State* L);
    int get_mouse_position(lua_State* L);

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
    int get_name(lua_State* L);
    int set_name(lua_State* L);
    int set_width(lua_State* L);
    int get_width(lua_State* L);
    int set_height(lua_State* L);
    int get_height(lua_State* L);
    int get_tag(lua_State* L);
    int set_tag(lua_State* L);

    // Get variable from another Entity
    int get_entity_position(lua_State* L);
    int set_entity_position(lua_State* L);
    int get_entity_visible(lua_State* L);
    int set_entity_visible(lua_State* L);
    int get_entity_name(lua_State* L);
    int set_entity_name(lua_State* L);
    int get_entity_velocity(lua_State* L);
    int set_entity_velocity(lua_State* L);
    int set_entity_width(lua_State* L);
    int get_entity_width(lua_State* L);
    int set_entity_height(lua_State* L);
    int get_entity_height(lua_State* L);

    int get_entity_list(lua_State* L);
    int get_entity_by_name(lua_State* L);
    int get_objects_by_tag(lua_State* L);

    int create_entity(lua_State* L);
    int check_collision(lua_State* L);
    int check_collision_side(lua_State* L);
    int delete_entity(lua_State* L);
    int delete_entity_id(lua_State* L);

    // Camera
    int set_camera(lua_State* L);
    int get_current_camera_ID(lua_State* L);
    int get_current_camera_name(lua_State* L);

    // Sound
    int play_sound(lua_State* L);
    int play_music(lua_State* L);
    int pause_music(lua_State* L);
    int stop_music(lua_State* L);
    int set_master_volume(lua_State* L);
    int set_effects_volume(lua_State* L);
    int set_music_volume(lua_State* L);
    
    // IO
    int save_game(lua_State* L);
    int load_game(lua_State* L);
}

#endif