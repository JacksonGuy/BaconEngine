#include "Sound.hpp"
#include "GameManager.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>

namespace Sound {

std::unordered_map<std::string, sf::SoundBuffer*> soundBuffers;
std::unordered_map<std::string, sf::Music*> musicPlayers;
std::vector<SoundObject> soundPlayers;
float masterVolume = 100.f;
float effectsVolume = 100.f;
float musicVolume = 100.f;

/**
 * @brief Loads a new sound effect
 * 
 * @param file the path to the sound file
 * @return sf::SoundBuffer* raw sound buffer
 */
sf::SoundBuffer* load_sound(std::string file) {
    // Search for sound buffer first
    if (Sound::soundBuffers.find(file) == Sound::soundBuffers.end()) {
        // We need to load in a new sound
        sf::SoundBuffer* buffer = new sf::SoundBuffer;
        if (!buffer->loadFromFile(file)) {
            GameManager::ConsoleWrite("[ERROR] Could not load sound: " + file);
            return nullptr;
        }
        Sound::soundBuffers[file] = buffer;
    }

    return Sound::soundBuffers[file];
}

/**
 * @brief Play a sound file 
 * 
 * @param file the path to the sound file
 */
void play_sound(std::string file) {
    std::string absPath = File::toAbsolute(file);
    sf::SoundBuffer* sound = Sound::load_sound(absPath);
    if (sound != nullptr) {
        // Create player and play sound
        sf::Sound* player = new sf::Sound();
        player->setBuffer(*sound);
        player->setVolume(effectsVolume);
        player->play();

        // Create SoundObject for management
        Sound::SoundObject obj;
        obj.player = player;
        obj.file = absPath;
        Sound::soundPlayers.push_back(obj);
    }
}

/**
 * @brief Deletes SFML Sound objects that aren't playing
 */
void clean_sounds() {
    // WARNING: Cursed Lambda function
    // Avoid eye contact!!
    Sound::soundPlayers.erase(std::remove_if(
        Sound::soundPlayers.begin(), Sound::soundPlayers.end(),
        [](SoundObject sound) {
            if (sound.player->getStatus() == sf::SoundSource::Stopped) {
                delete(sound.player);
                return true;
            }
            return false;
        }
    ), Sound::soundPlayers.end());
}

/**
 * @brief Loads a new music file
 * 
 * @param file the path to the music file
 * @return sf::Music* SFML music object
 */
sf::Music* load_music(std::string file) {
    // Convert file path
    std::string absPath = File::toAbsolute(file); 
    
    // Search for music player
    if (Sound::musicPlayers.find(absPath) == Sound::musicPlayers.end()) {
        sf::Music* player = new sf::Music();
        if (!player->openFromFile(absPath)) {
            GameManager::ConsoleWrite("[ERROR] Failed to load music: " + file);
            return nullptr;
        }
        player->setVolume(musicVolume);
        Sound::musicPlayers[absPath] = player;
    }

    return Sound::musicPlayers[absPath];
}

/**
 * @brief Deletes unused SFML music objects
 */
void clean_music() {
    for (auto it = Sound::musicPlayers.begin(); it != Sound::musicPlayers.end(); it++) {
        if (it->second->getStatus() == sf::Music::Stopped) {
            delete(it->second);
            Sound::musicPlayers.erase(it);
        }
    }
}

/**
 * @brief Stops all playing sounds and deletes them
 */
void stop_sounds() {
    // Stop Sounds
    for (SoundObject sound : Sound::soundPlayers) {
        sound.player->stop();
    }

    // Stop Music
    for (auto it = Sound::musicPlayers.begin(); it != Sound::musicPlayers.end(); it++) {
        it->second->stop();
    }

    // Clean
    clean_sounds();
    clean_music();
}

/**
 * @brief Set the Master Volume
 * 
 * @param volume volume ranging from 0-100 
 */
void setMasterVolume(float volume) {
    masterVolume = volume;
    if (masterVolume > 100.f) masterVolume = 100.f;
    if (masterVolume < 0.f) masterVolume = 0.f;

    masterVolume = round(masterVolume);

    // Adjust sounds and music volume
    setEffectsVolume(effectsVolume);
    setMusicVolume(musicVolume);
}

/**
 * @brief Set the volume of effects
 * 
 * @param volume volume ranging from 0-100
 */
void setEffectsVolume(float volume) {
    effectsVolume = volume;
    if (effectsVolume > 100.f) effectsVolume = 100.f;
    if (effectsVolume < 0.f) effectsVolume = 0.f;

    float finalVol = (effectsVolume / 100.f) * (masterVolume / 100.f);
    finalVol = round(finalVol * 100);

    for (SoundObject sound : soundPlayers) {
        sound.player->setVolume(finalVol);
    }
}

/**
 * @brief Set the volume of music
 * 
 * @param volume volume ranging from 0-100
 */
void setMusicVolume(float volume) {
    musicVolume = volume;
    if (musicVolume > 100.f) musicVolume = 100.f;
    if (musicVolume < 0.f) musicVolume = 0.f;

    float finalVol = (musicVolume / 100.f) * (masterVolume / 100.f);
    finalVol = round(finalVol * 100);

    for (auto it = musicPlayers.begin(); it != musicPlayers.end(); it++) {
        it->second->setVolume(finalVol);
    }
}

}