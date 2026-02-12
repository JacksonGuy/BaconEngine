#pragma once

#include <stdint.h>

#include "imgui.h"
#include "raylib.h"

#include "core/game_state.h"

namespace bacon
{
	enum class MessageType : uint8_t
	{
		NONE = 0,
		LOG,
		WARNING,
		ERROR
	};

	typedef struct ConsoleMessage
	{
		MessageType type;
		std::string message;
	} ConsoleMessage;

	class Editor
	{
	public:
		uint32_t screen_width;
		uint32_t screen_height;
		uint32_t framerate_limit;

		bool is_playing = false;
		float camera_move_speed = 1.0f;
		float camera_zoom_speed = 0.1f;

		std::string editor_font_path;

		Camera2D camera;

		Editor();
		~Editor() = default;

		void create_config_file();
		void load_config_file();

		void console_log(const char* text);
		void console_warn(const char* text);
		void console_error(const char* text);
		void console_clear();
		const std::vector<ConsoleMessage>& get_console_messages();

		void draw_ui();
		void editor_input();

		void start_game();
		void end_game();

	private:
		std::vector<ConsoleMessage> m_console_messages;
	};
} // namespace bacon
