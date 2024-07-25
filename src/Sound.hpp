#ifndef SOUND_H
#define SOUDN_H

#include <vector>
#include <unordered_map> 
#include <queue>
#include <SFML/Audio.hpp>

namespace Sound {
    struct SoundObject {
        sf::Sound* player;
        std::string file;
    };

    extern std::unordered_map<std::string, sf::SoundBuffer*> soundBuffers;
    extern std::unordered_map<std::string, sf::Music*> musicPlayers;
    extern std::vector<SoundObject> soundPlayers;

    extern float masterVolume;
    extern float effectsVolume;
    extern float musicVolume;

    sf::SoundBuffer* load_sound(std::string file);
    void play_sound(std::string file);
    void clean_sounds();

    sf::Music* load_music(std::string file);
    void clean_music();

    void stop_sounds();
    void setMasterVolume(float volume);
    void setEffectsVolume(float volume);
    void setMusicVolume(float volume);
}

#endif