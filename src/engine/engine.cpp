#include "engine.h"

#include <ctime>

namespace bacon {
    Engine::Engine() {
        this->screen_width = 800;
        this->screen_height = 600;
        this->framerate_limit = 60;
        this->engine_font = {0};
    }

    Engine::~Engine() {

    }

    void Engine::ConsoleLog(const char* text) {
        // Get time
        time_t now = time(0);
        char* current_time = ctime(&now);
        std::string time_str = std::string(current_time);
        time_str.pop_back();

        std::string message = "[ENGINE] (" + time_str + "): " + text + "\n";
        this->m_ConsoleMessages.push_back(message);
    }

    void Engine::ConsoleWarn(const char* text) {
        // Get time
        time_t now = time(0);
        char* current_time = ctime(&now);
        std::string time_str = std::string(current_time);
        time_str.pop_back();

        std::string message = "[WARN] (" + time_str + "): " + text + "\n";
        this->m_ConsoleMessages.push_back(message);
    }

    void Engine::ConsoleError(const char* text) {
        // Get time
        time_t now = time(0);
        char* current_time = ctime(&now);
        std::string time_str = std::string(current_time);
        time_str.pop_back();

        std::string message = "[ERROR] (" + time_str + "): " + text + "\n";
        this->m_ConsoleMessages.push_back(message);
    }

    void Engine::ConsoleGameMessage(const char* text) {
        // Get time
        time_t now = time(0);
        char* current_time = ctime(&now);
        std::string time_str = std::string(current_time);
        time_str.pop_back();

        std::string message = std::string(text) + "\n";
        this->m_ConsoleMessages.push_back(message);
    }

    void Engine::clear_console() {
        this->m_ConsoleMessages.clear();
    }
}
