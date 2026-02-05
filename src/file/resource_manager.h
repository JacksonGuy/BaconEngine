#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "raylib.h"

namespace bacon {
    class ResourceManager {
        public:
            ResourceManager() = default;
            ~ResourceManager();

            std::shared_ptr<Texture2D> load_texture(const char* path);
            std::shared_ptr<Font> load_font(const std::string& path);

        private:
            std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_textures;
            std::unordered_map<std::string, std::shared_ptr<Font>> m_fonts;
    };
}
