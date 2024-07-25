#include "Sound.hpp"
#include "GameManager.hpp"

#include <algorithm>
#include <cmath>

namespace Sound {

std::unordered_map<std::string, sf::SoundBuffer*> soundBuffers;
std::unordered_map<std::string, sf::Music*> musicPlayers;
std::vector<SoundObject> soundPlayers;
float masterVolume = 100.f;
float effectsVolume = 100.f;
float musicVolume = 100.f;

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

void play_sound(std::string file) {
    sf::SoundBuffer* sound = Sound::load_sound(file);
    if (sound != nullptr) {
        // Create player and play sound
        sf::Sound* player = new sf::Sound();
        player->setBuffer(*sound);
        player->setVolume(effectsVolume);
        player->play();

        // Create SoundObject for management
        Sound::SoundObject obj;
        obj.player = player;
        obj.file = file;
        Sound::soundPlayers.push_back(obj);
    }
}

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

sf::Music* load_music(std::string file) {
    // Search for music player
    if (Sound::musicPlayers.find(file) == Sound::musicPlayers.end()) {
        sf::Music* player = new sf::Music();
        if (!player->openFromFile(file)) {
            GameManager::ConsoleWrite("[ERROR] Failed to load music: " + file);
            return nullptr;
        }
        player->setVolume(musicVolume);
        Sound::musicPlayers[file] = player;
    }

    return Sound::musicPlayers[file];
}

void clean_music() {
    for (auto it = Sound::musicPlayers.begin(); it != Sound::musicPlayers.end(); it++) {
        if (it->second->getStatus() == sf::Music::Stopped) {
            delete(it->second);
            Sound::musicPlayers.erase(it);
        }
    }
}

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

void setMasterVolume(float volume) {
    masterVolume = volume;
    if (masterVolume > 100.f) masterVolume = 100.f;
    if (masterVolume < 0.f) masterVolume = 0.f;

    masterVolume = round(masterVolume);

    // Adjust sounds and music volume
    setEffectsVolume(effectsVolume);
    setMusicVolume(musicVolume);
}

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