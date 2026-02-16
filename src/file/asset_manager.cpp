#include "asset_manager.h"

#include <memory>

#include "raylib.h"

#include "core/util.h"

namespace bacon
{
    AssetManager::~AssetManager()
    {
        // Unload textures
        for (auto it = m_textures.begin(); it != m_textures.end(); it++)
        {
            std::shared_ptr<Texture2D> texture = it->second;
            UnloadTexture(*texture);
        }
        m_textures.clear();

        // Unload fonts
        for (auto it = m_fonts.begin(); it != m_fonts.end(); it++)
        {
            std::shared_ptr<Font> font = it->second;
            UnloadFont(*font);
        }
        m_fonts.clear();
    }

    std::shared_ptr<Texture2D> AssetManager::load_texture(const std::string& path)
    {
        if (m_textures.find(path) != m_textures.end())
        {
            return m_textures[path];
        }
        else
        {
            Texture2D new_texture = LoadTexture(path.c_str());
            if (new_texture.id <= 0)
            {
                debug_error("Failed to load texture: %s", path);
                return {0};
            }
            m_textures[path] = std::make_shared<Texture2D>(new_texture);
            return m_textures[path];
        }
    }

    std::shared_ptr<Font> AssetManager::load_font(const std::string& path)
    {
        if (m_fonts.find(path) != m_fonts.end())
        {
            return m_fonts[path];
        }
        else
        {
            Font new_font = LoadFont(path.c_str());
            if (new_font.baseSize <= 0)
            {
                debug_error("Failed to load font: %s", path);
                return {0};
            }
            m_fonts[path] = std::make_shared<Font>(new_font);
            return m_fonts[path];
        }
    }

    const std::unordered_map<std::string, std::shared_ptr<Texture2D>>&
    AssetManager::get_textures() const
    {
        return m_textures;
    }

    const std::unordered_map<std::string, std::shared_ptr<Font>>&
    AssetManager::get_fonts() const
    {
        return m_fonts;
    }
} // namespace bacon
