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
        std::string m_default_font_path = "";
        std::shared_ptr<Font> m_default_font = nullptr;

		void initialize_game_state()
		{
			// GameState::renderer = nullptr;
			// GameState::m_default_font = {0};
			// GameState::m_default_font_path = "";
		}

		void initialize_renderer(uint32_t width, uint32_t height)
		{
			GameState::renderer = new Renderer(width, height);
		}

		void load_default_font(const std::string& path)
		{
			m_default_font = GameState::assets.load_font(path);

			if (m_default_font->baseSize <= 0)
			{
				m_default_font = {0};
				debug_error("Failed to load default font!");
			}
			else
			{
				m_default_font_path = path;
				debug_log("Default font set.");
			}
		}
	} // namespace GameState
} // namespace bacon
