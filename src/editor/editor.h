#pragma once

#include "imgui.h"
#include "raylib.h"

#include "core/2D/game_manager.h"

namespace bacon {
    class Editor {
        public:
            uint32_t screen_width;
            uint32_t screen_height;
            uint32_t framerate_limit;

            bool is_playing = false;

            std::string editor_font_path;

            GameManager manager;
            Camera2D camera;

            Editor();
            ~Editor() = default;

            void create_config_file();
            void load_config_file();

            void ConsoleLog(const char* text);
            void ConsoleWarn(const char* text);
            void ConsoleError(const char* text);
            void ConsoleGameMessage(const char* text);
            void clear_console();

            void draw_ui();

            void start_game();
            void end_game();

        private:
            ImGuiTextBuffer m_ConsoleBuffer;
            std::vector<std::string> m_ConsoleMessages;
    };
}
