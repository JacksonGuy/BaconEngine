#pragma once

#include "imgui.h"
#include "raylib.h"

#include "core/game_manager.h"

namespace bacon {
    class Engine {
        public:
            uint32_t screen_width;
            uint32_t screen_height;
            uint32_t framerate_limit;

            Font engine_font;

            GameManager manager;
            Camera2D* camera;

            Engine();
            ~Engine();

            void ConsoleLog(const char* text);
            void ConsoleWarn(const char* text);
            void ConsoleError(const char* text);
            void ConsoleGameMessage(const char* text);
            void clear_console();

        private:
            ImGuiTextBuffer m_ConsoleBuffer;
            std::vector<std::string> m_ConsoleMessages;
    };
}
