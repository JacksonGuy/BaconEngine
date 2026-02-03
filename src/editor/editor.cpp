#include "editor.h"

#include <ctime>

#include "core/globals.h"
#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"

#include "ui/editor_ui.h"
#include "file/file.h"

namespace bacon {
    Editor::Editor() {
        this->screen_width = 1280;
        this->screen_height = 720;
        this->framerate_limit = 60;
        this->editor_font = {0};

        this->is_playing = false;

        // Editor camera for viewing level
        this->camera = {0};
        this->camera.target = {
            0,
            0
        };
        this->camera.rotation = 0.0f;
        this->camera.zoom = 1.f;

        // Create window
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(this->screen_width, this->screen_height, "Test");
        SetTargetFPS(this->framerate_limit);
        ui::init(800, 600);

        // TODO change default
        this->manager.initialize_renderer(800, 600);
    }

    void Editor::ConsoleLog(const char* text) {
        // Get time
        time_t now = time(0);
        char* current_time = ctime(&now);
        std::string time_str = std::string(current_time);
        time_str.pop_back();

        std::string message = "[ENGINE] (" + time_str + "): " + text + "\n";
        this->m_ConsoleMessages.push_back(message);
    }

    void Editor::ConsoleWarn(const char* text) {
        // Get time
        time_t now = time(0);
        char* current_time = ctime(&now);
        std::string time_str = std::string(current_time);
        time_str.pop_back();

        std::string message = "[WARN] (" + time_str + "): " + text + "\n";
        this->m_ConsoleMessages.push_back(message);
    }

    void Editor::ConsoleError(const char* text) {
        // Get time
        time_t now = time(0);
        char* current_time = ctime(&now);
        std::string time_str = std::string(current_time);
        time_str.pop_back();

        std::string message = "[ERROR] (" + time_str + "): " + text + "\n";
        this->m_ConsoleMessages.push_back(message);
    }

    void Editor::ConsoleGameMessage(const char* text) {
        // Get time
        time_t now = time(0);
        char* current_time = ctime(&now);
        std::string time_str = std::string(current_time);
        time_str.pop_back();

        std::string message = std::string(text) + "\n";
        this->m_ConsoleMessages.push_back(message);
    }

    void Editor::clear_console() {
        this->m_ConsoleMessages.clear();
    }

    void Editor::draw_ui() {
        rlImGuiBegin();
        ImGui::DockSpaceOverViewport();

        ui::draw_top_bar(this->manager);
        ui::draw_object_properties();
        ui::draw_object_tree(this->manager);
        ui::draw_scene_display(this->manager.get_renderer());
        ui::draw_engine_console();
        ui::draw_settings();
        ui::draw_general_info_display(this);

        ui::draw_entity_create(this->manager);
        ui::draw_text_create(this->manager);
        ui::draw_camera_create(this->manager);

        rlImGuiEnd();
    }

    void Editor::start_game() {
        this->is_playing = true;

        file::save_project(this->manager);
    }

    void Editor::end_game() {
        this->is_playing = false;

        file::load_project(this->manager);
    }
}
