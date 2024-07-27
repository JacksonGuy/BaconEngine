#include "LuaApi.hpp"
#include "../Sound.hpp"

namespace Lua {
    int play_sound(lua_State* L) {
        std::string file = lua_tostring(L, 1);
        Sound::play_sound(file);
        return 0;
    }

    int play_music(lua_State* L) {
        std::string file = lua_tostring(L, 1);
        sf::Music* player = Sound::load_music(file);
        if (player == nullptr) return 0;
        if (player->getStatus() != sf::SoundSource::Playing) {
            player->play();
        }
        return 0;
    }

    int pause_music(lua_State* L) {
        std::string file = lua_tostring(L, 1);
        sf::Music* player = Sound::load_music(file);
        if (player == nullptr) return 0;
        player->pause();
        return 0;
    }

    int stop_music(lua_State* L) {
        std::string file = lua_tostring(L, 1);
        sf::Music* player = Sound::load_music(file);
        if (player == nullptr) return 0;
        player->stop();
        return 0; 
    }
}