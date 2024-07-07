#include "LuaApi.hpp"

#include "../GameManager.hpp"

// set_text(id, "string")
int set_text(lua_State* L) {
    const int id = lua_tonumber(L, 1);
    const char* newtext = lua_tostring(L, 2);

    TextObj* text = GameManager::FindTextByID(id);
    if (text != nullptr) {
        text->text.setString(newtext);
    }

    return 0;
}

// local text = get_text(id)
int get_text(lua_State* L) {
    const int id = lua_tonumber(L, 1);

    TextObj* text = GameManager::FindTextByID(id);
    if (text != nullptr) {
        const std::string str = text->text.getString().toAnsiString();
        lua_pushstring(L, str.c_str());
        return 1;
    }

    return 0;
}

// set_text_position(id, x, y)
int set_text_position(lua_State* L) {
    const int id = lua_tonumber(L, 1);
    const int x = lua_tonumber(L, 2);
    const int y = lua_tonumber(L, 3);

    TextObj* text = GameManager::FindTextByID(id);
    if (text != nullptr) {
        text->text.setPosition(sf::Vector2f(x, y));
        text->position = sf::Vector2f(x,y);
    }

    return 0;
}

// local x, y = get_text_position(id)
int get_text_position(lua_State* L) {
    const int id = lua_tonumber(L, 1);

    TextObj* text = GameManager::FindTextByID(id);
    if (text != nullptr) {
        sf::Vector2f pos = text->text.getPosition();
        lua_pushnumber(L, pos.x);
        lua_pushnumber(L, pos.y);
        return 2;
    }

    return 0;
}

// set_text_target(id, entity)
int set_text_target(lua_State* L) {
    const int textID = lua_tonumber(L, 1);
    const int entityID = lua_tonumber(L, 2);

    TextObj* text = GameManager::FindTextByID(textID);
    Entity* e = GameManager::FindEntityByID(entityID);

    if (text != nullptr && e != nullptr) {
        text->target = e;
        text->target_id = entityID; 
    }

    return 0;
}

// local entity = get_text_target(id)
int get_text_target(lua_State* L) {
    const int id = lua_tonumber(L, 1);

    TextObj* text = GameManager::FindTextByID(id);
    if (text != nullptr) {
        lua_pushnumber(L, text->target->ID);
        return 1;
    }

    return 0;
}

// set_text_visible(id, bool)
int set_text_visible(lua_State* L) {
    const int id = lua_tonumber(L, 1);
    const bool val = lua_toboolean(L, 2);

    TextObj* text = GameManager::FindTextByID(id);
    if (text != nullptr) {
        text->isVisible = val;
    }

    return 0;
}

// local vis = get_text_visible(id)
int get_text_visible(lua_State* L) {
    const int id = lua_tonumber(L, 1);

    TextObj* text = GameManager::FindTextByID(id);
    if (text != nullptr) {
        lua_pushboolean(L, text->isVisible);
        return 1;
    }

    return 0;
}

// set_text_color(id, r, g, b, a)
int set_text_color(lua_State* L) {
    const int id = lua_tonumber(L, 1);
    const int red = lua_tonumber(L, 2);
    const int green = lua_tonumber(L, 3);
    const int blue = lua_tonumber(L, 4);
    const int alpha = lua_tonumber(L, 5);

    TextObj* text = GameManager::FindTextByID(id);
    if (text != nullptr) {
        text->text.setColor(sf::Color(red, green, blue, alpha));
    }

    return 0;
}

// local r,g,b,a = get_text_color(id)
int get_text_color(lua_State* L) {
    const int id = lua_tonumber(L, 1);

    TextObj* text = GameManager::FindTextByID(id);
    if (text != nullptr) {
        sf::Color color = text->text.getColor();
        lua_pushnumber(L, color.r);
        lua_pushnumber(L, color.g);
        lua_pushnumber(L, color.b);
        lua_pushnumber(L, color.a);
        return 4;
    }

    return 0;
}

// set_text_size(id, size)
int set_text_size(lua_State* L) {
    const int id = lua_tonumber(L, 1);
    const int size = lua_tonumber(L, 2);

    TextObj* text = GameManager::FindTextByID(id);
    if (text != nullptr) {
        text->text.setCharacterSize(size);
    }

    return 0;
}

// local size = get_text_size(id)
int get_text_size(lua_State* L) {
    const int id = lua_tonumber(L, 1);

    TextObj* text = GameManager::FindTextByID(id);
    if (text != nullptr) {
        lua_pushnumber(L, text->text.getCharacterSize());
        return 1;
    }

    return 0;
}

// textID = create_text(string)
int create_text(lua_State* L) {
    const char* str = lua_tostring(L, 1);

    TextObj* text = new TextObj();
    lua_pushnumber(L, text->ID);
    return 1;
}