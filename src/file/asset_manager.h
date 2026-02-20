#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "raylib.h"

namespace bacon
{
	/*
	 * For future reference:
	 * Yes, I understand that Raylib structs are just a handle to GPU memory.
	 * We use shared_ptr instead of copying to prevent undefined behavior.
	 */

	class AssetManager
	{
	public:
		AssetManager() = default;
		~AssetManager();

		std::shared_ptr<Texture2D> load_texture(const std::string& path);
		std::shared_ptr<Font> load_font(const std::string& path);
		const std::unordered_map<std::string, std::shared_ptr<Texture2D>>& get_textures() const;
		const std::unordered_map<std::string, std::shared_ptr<Font>>& get_fonts() const;

	private:
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_textures;
		std::unordered_map<std::string, std::shared_ptr<Font>> m_fonts;
	};
} // namespace bacon
