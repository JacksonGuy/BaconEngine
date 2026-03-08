#pragma once

#include "file/asset_manager_2d.h"
#include "core/2D/scene_2d.h"
#include "rendering/2D/renderer_2d.h"

namespace bacon
{
	class GameState2D
	{
	public:
		AssetManager2D* assets;
		Scene2D* scene;
		Renderer2D* renderer;

		GameState2D();
		~GameState2D();

		void cleanup();
	};
}
