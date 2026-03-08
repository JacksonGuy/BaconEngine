#pragma once

#include <memory>
#include <string>
#include <cstdint>

#include "nfd.h"
#include "raylib.h"

#include "core/game_object.h"

namespace bacon
{
	namespace file
	{
		enum class AssetType : uint32_t
		{
			NONE = 0,
			TEXTURE,
			FONT,
			SOUND,
			MUSIC,
		};

		typedef struct
		{
			AssetType type;
			std::string path;
		} asset_t;

		constexpr nfdfilteritem_t texture_types = {"Images", "png,jpeg,jpg"};
		constexpr nfdfilteritem_t font_types = {"Font", "ttf"};

		nfdresult_t save_project();
		nfdresult_t load_project(bool show_dialog);
		nfdresult_t create_new_project();

		nfdresult_t save_object_prefab(const GameObject& object);
		nfdresult_t load_from_prefab(const std::string& path, GameObject& object);

		asset_t load_asset_nfd(AssetType type);

		std::string abs_path_to_relative(std::string path);
		std::string relative_path_to_abs(std::string path);
	} // namespace file
} // namespace bacon
