#include "editor.h"

#include <ctime>

#include "rlImGui.h"
#include "imgui.h"

#include "ui/editor_ui.h"

namespace bacon {
    Editor::Editor() {
        this->screen_width = 1280;
        this->screen_height = 720;
        this->framerate_limit = 60;
        this->editor_font = {0};

        // Editor camera for viewing level
        this->camera = {0};
        this->camera.target = {
            0,
            0
        };
        this->camera.rotation = 0.0f;
        this->camera.zoom = 1.f;

        this->inspect_object = nullptr;

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

        ui::draw_top_bar();
        ui::draw_object_tree(this->inspect_object);
        ui::draw_scene_display(this->manager.get_renderer());
        ui::draw_engine_console();
        ui::draw_settings();
        ui::draw_general_info_display();

        if (ui::show_test == true)
        {
            ImGui::Begin("Test Menu", &ui::show_test, ImGuiWindowFlags_MenuBar);
                if (ImGui::BeginMenuBar())
                {
                    if (ImGui::BeginMenu("File"))
                    {
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenuBar();
                }
            ImGui::End();
        }

        rlImGuiEnd();
    }
}
