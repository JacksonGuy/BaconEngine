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

    int set_master_volume(lua_State* L) {
        float volume = lua_tonumber(L, 1);
        Sound::setMasterVolume(volume);
        return 0;
    }

    int set_effects_volume(lua_State* L) {
        float volume = lua_tonumber(L, 1);
        Sound::setEffectsVolume(volume);
        return 0;
    }

    int set_music_volume(lua_State* L) {
        float volume = lua_tonumber(L, 1);
        Sound::setMusicVolume(volume);
        return 0;
    }
}