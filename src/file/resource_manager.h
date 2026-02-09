#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "raylib.h"

namespace bacon
{
    class ResourceManager
    {
         public:
        ResourceManager() = default;
        ~ResourceManager();

        std::shared_ptr<Texture2D> load_texture(const char* path);
        std::shared_ptr<Font> load_font(const std::string& path);

         private:
        std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_textures;
        std::unordered_map<std::string, std::shared_ptr<Font>> m_fonts;
    };
} // namespace bacon
