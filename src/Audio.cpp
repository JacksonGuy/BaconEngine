#include "Audio.h"
#include "GameManager.h"

namespace Audio {
    std::vector<Sound> sound_list;
    std::unordered_map<std::string, MusicAsset> music_list;

    bool IsSoundValid(Sound sound) {
        return ((sound.frameCount > 0) &&       // Validate frame count
            (sound.stream.buffer != NULL) &&    // Validate stream buffer
            (sound.stream.sampleRate > 0) &&    // Validate sample rate is supported
            (sound.stream.sampleSize > 0) &&    // Validate sample size is supported
            (sound.stream.channels > 0));       // Validate number of channels supported
    }

    bool IsMusicValid(Music music) {
        return ((music.ctxData != NULL) &&          // Validate context loaded
                (music.frameCount > 0) &&           // Validate audio frame count
                (music.stream.sampleRate > 0) &&    // Validate sample rate is supported
                (music.stream.sampleSize > 0) &&    // Validate sample size is supported
                (music.stream.channels > 0));       // Validate number of channels supported
    }

    void b_PlaySound(std::string path) {
        std::string absPath = GameManager::projectEntryPath + "/" + path;
        Sound sound = LoadSound(absPath.c_str());
        sound_list.push_back(sound);
        PlaySound(sound);
    }

    void b_PlaySound(std::string path, f32 volume, f32 pitch, f32 pan) {
        std::string absPath = GameManager::projectEntryPath + "/" + path;
        Sound sound = LoadSound(absPath.c_str());

        SetSoundVolume(sound, volume);
        SetSoundPitch(sound, pitch);
        SetSoundPan(sound, pan);

        PlaySound(sound);
    }

    void b_PlayMusic(std::string path) {
        std::string absPath = GameManager::projectEntryPath + "/" + path;

        if (music_list.find(path) == music_list.end()) {
            MusicAsset music = {
                LoadMusicStream(absPath.c_str()),
                path
            };
            music_list[path] = music;
            
            PlayMusicStream(music.music);
        }
        else {
            MusicAsset& music = music_list[path];
            if (IsAudioStreamPlaying(music.music.stream)) return;
            PlayMusicStream(music.music);
        }
    }

    void b_PlayMusic(std::string path, f32 volume, f32 pitch, f32 pan) {
        std::string absPath = GameManager::projectEntryPath + "/" + path;

        if (music_list.find(path) == music_list.end()) {
            MusicAsset music = {
                LoadMusicStream(absPath.c_str()),
                path
            };
            music_list[path] = music;

            SetMusicVolume(music.music, volume);
            SetMusicPitch(music.music, pitch);
            SetMusicPan(music.music, pan);
            
            PlayMusicStream(music.music);
        }
        else {
            MusicAsset& music = music_list[path];

            SetMusicVolume(music.music, volume);
            SetMusicPitch(music.music, pitch);
            SetMusicPan(music.music, pan);

            if (IsAudioStreamPlaying(music.music.stream)) return;
            PlayMusicStream(music.music);
        }
    }
}