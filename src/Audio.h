#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include "raylib.h"
#include "util.h"

struct MusicAsset {
    Music music;
    std::string path;
};

namespace Audio {
    extern std::vector<Sound> sound_list;
    extern std::unordered_map<std::string, MusicAsset> music_list;

    bool IsSoundValid(Sound sound);
    bool IsMusicValid(Music music);

    void b_PlaySound(std::string path);
    void b_PlaySound(std::string path, f32 volume, f32 pitch, f32 pan);
    void b_PlayMusic(std::string path);
    void b_PlayMusic(std::string path, f32 volume, f32 pitch, f32 pan);
}