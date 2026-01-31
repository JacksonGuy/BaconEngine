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

            Font editor_font;

            GameManager manager;
            Camera2D camera;

            Editor();
            ~Editor() = default;

            void ConsoleLog(const char* text);
            void ConsoleWarn(const char* text);
            void ConsoleError(const char* text);
            void ConsoleGameMessage(const char* text);
            void clear_console();

            void draw_ui();

        private:
            ImGuiTextBuffer m_ConsoleBuffer;
            std::vector<std::string> m_ConsoleMessages;
    };
}
