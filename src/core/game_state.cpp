#include "game_state.h"

#include "core/util.h"

namespace bacon
{
	namespace GameState
	{
		void initialize_game_state()
		{
			GameState::renderer = nullptr;
			GameState::m_default_font = {0};
			GameState::m_default_font_path = "";
		}

		void initialize_renderer(uint32_t width, uint32_t height)
		{
			GameState::renderer = new Renderer(width, height);
		}

		void load_default_font(const std::string& path)
		{
			m_default_font = GameState::resources.load_font(path);

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
