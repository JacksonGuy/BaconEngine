#pragma once

#include <stdint.h>

#include <vector>

#include "box2d/box2d.h"
#include "raylib.h"
#include "sol/sol.hpp"

#include "lib/arena.h"
#include "file/asset_manager.h"
#include "core/2D/entity.h"
#include "core/2D/game_object.h"
#include "core/2D/scene_2d.h"
#include "core/2D/text_object.h"
#include "rendering/2D/renderer.h"

namespace bacon
{
	namespace GameState
	{
		extern AssetManager assets;
		extern Scene2D scene;
		extern Renderer* renderer;

		void initialize_renderer(uint32_t width, uint32_t height);
	} // namespace GameState
} // namespace bacon
