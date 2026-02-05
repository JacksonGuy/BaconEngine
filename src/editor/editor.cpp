#include "editor.h"

#include <ctime>
#include <filesystem>
#include <fstream>

#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"

#include "core/globals.h"
#include "ui/editor_ui.h"
#include "file/file.h"
#include "core/util.h"

namespace bacon {
    Editor::Editor() {
        this->screen_width = 1280;
        this->screen_height = 720;
        this->framerate_limit = 60;
        this->editor_font_path = "";

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
        debug_log("Initializing Raylib...");
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(this->screen_width, this->screen_height, "Test");
        SetTargetFPS(this->framerate_limit);
        debug_log("Raylib initialized.");

        // Load config
        this->load_config_file();

        // Initialize UI elements
        ui::init(800, 600);

        // TODO change default
        this->manager.initialize_renderer(800, 600);
    }

    void Editor::create_config_file() {
        using json = nlohmann::json;
        namespace fs = std::filesystem;

        std::string current_directory = fs::current_path().generic_string();
        std::string path = file::relative_path_to_abs(current_directory) + "/config.json";
        std::ofstream outfile(path);
        json data;

        data["engine_version"] = globals::engine_version;
        data["default_font_path"] = "./Roboto-Regular.ttf";

        outfile << std::setw(4) << data;
    }

    void Editor::load_config_file() {
        using json = nlohmann::json;
        namespace fs = std::filesystem;

        std::string current_directory = fs::current_path().generic_string();
        std::string path = file::relative_path_to_abs(current_directory) + "/config.json";
        std::ifstream infile(path);

        if (!infile) {
            create_config_file();
            infile = std::ifstream(path);
        }

        json data = json::parse(infile);
        for (json::iterator it = data.begin(); it != data.end(); it++)
        {
            std::string key = it.key();
            auto value = *it;

            if (key == "engine_version")
            {
                globals::engine_version = value;
            }
            else if (key == "default_font_path")
            {
                globals::editor_default_font_path = value;
            }
        }

        // Load default font for game manager
        debug_log("Config - Default Font: %s", globals::editor_default_font_path.c_str());
        this->manager.load_default_font(globals::editor_default_font_path.c_str());
    }

    void Editor::console_log(const char* text) {
        // Get time
        time_t now = time(0);
        char* current_time = ctime(&now);
        std::string time_str = std::string(current_time);
        time_str.pop_back();

        std::string message = "[LOG] (" + time_str + "): " + text + "\n";
        this->m_console_messages.push_back({MessageType::LOG, message});
    }

    void Editor::console_warn(const char* text) {
        // Get time
        time_t now = time(0);
        char* current_time = ctime(&now);
        std::string time_str = std::string(current_time);
        time_str.pop_back();

        std::string message = "[WARNING] (" + time_str + "): " + text + "\n";
        this->m_console_messages.push_back({MessageType::WARNING, message});
    }

    void Editor::console_error(const char* text) {
        // Get time
        time_t now = time(0);
        char* current_time = ctime(&now);
        std::string time_str = std::string(current_time);
        time_str.pop_back();

        std::string message = "[ERROR] (" + time_str + "): " + text + "\n";
        this->m_console_messages.push_back({MessageType::ERROR, message});
    }

    const std::vector<ConsoleMessage>& Editor::get_console_messages()
    {
        return this->m_console_messages;
    }

    void Editor::draw_ui() {
        rlImGuiBegin();
        ImGui::DockSpaceOverViewport();

        ui::draw_top_bar(this->manager);
        ui::draw_object_properties();
        ui::draw_object_tree(this->manager);
        ui::draw_scene_display(this->manager.get_renderer());
        ui::draw_engine_console(this);
        ui::draw_settings();
        ui::draw_general_info_display(this);

        ui::draw_entity_create(this->manager);
        ui::draw_text_create(this->manager);
        ui::draw_camera_create(this->manager);

        rlImGuiEnd();
    }

    void Editor::editor_input()
    {
        Vector2 mouse_position = GetMousePosition();
        Vector2 mouse_delta = GetMouseDelta();
        float mouse_wheel_move = GetMouseWheelMove();

        if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON))
        {
            this->camera.target.x -= (mouse_delta.x * this->camera_move_speed);
            this->camera.target.y -= (mouse_delta.y * this->camera_move_speed);
        }

        if (IsKeyDown(KEY_LEFT_CONTROL))
        {
            if (mouse_wheel_move != 0)
            {
                this->camera.zoom += (mouse_wheel_move * this->camera_zoom_speed);
            }
        }
    }

    void Editor::start_game() {
        this->is_playing = true;

        this->manager.create_physics_bodies();

        if (globals::has_changes_made)
        {
            file::save_project(this->manager);
        }
    }

    void Editor::end_game() {
        this->is_playing = false;

        std::string inspect_uuid = "";
        if (ui::view_properties_object != nullptr) {
            inspect_uuid = ui::view_properties_object->uuid.get_uuid();
        }
        ui::view_properties_object = nullptr;

        this->manager.reset();
        if (globals::is_project_loaded)
        {
            file::load_project(this->manager, false);
        }

        if (inspect_uuid.length() > 0) {
            GameObject* inspect_object = manager.find_object_by_uuid(inspect_uuid);
            ui::view_properties_object = inspect_object;
        }
    }
}
