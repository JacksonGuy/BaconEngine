#include "game_state.h"

#include "core/2D/scene_2d.h"
#include "core/util.h"
#include "file/asset_manager.h"

namespace bacon
{
	namespace GameState
	{
		AssetManager assets;
		Scene2D scene;
		Renderer* renderer = nullptr;

		void initialize_renderer(uint32_t width, uint32_t height)
		{
			GameState::renderer = new Renderer(width, height);
		}
	} // namespace GameState
} // namespace bacon
