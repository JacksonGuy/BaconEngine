#include "editor.h"

#include <ctime>
#include <filesystem>
#include <fstream>

#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"

#include "core/globals.h"
#include "core/util.h"
#include "file/file.h"
#include "ui/editor_ui.h"
#include "editor_event.h"

namespace bacon
{
	Editor::Editor()
	{
		this->screen_width = 1280;
		this->screen_height = 720;
		this->framerate_limit = 60;
		this->editor_font_path = "";

		this->is_playing = false;

		// Editor camera for viewing level
		this->camera = {0};
		this->camera.target = {0, 0};
		this->camera.rotation = 0.0f;
		this->camera.zoom = 1.f;

		// Create window
		debug_log("Initializing Raylib...");

		SetConfigFlags(FLAG_WINDOW_RESIZABLE);
		InitWindow(this->screen_width, this->screen_height, "Test");
		SetTargetFPS(this->framerate_limit);
		globals::update_window_title();
		SetExitKey(KEY_NULL); // Remove ESC as exit key

		debug_log("Raylib initialized.");

		// Load config
		this->load_config_file();

		// Initialize UI elements
		ui::init(800, 600);

		// TODO change default
		GameState::initialize_renderer(800, 600);
	}

	Editor::~Editor()
	{

	}

	void Editor::create_config_file()
	{
		using json = nlohmann::json;
		namespace fs = std::filesystem;

		std::string current_directory = fs::current_path().generic_string();
		std::string path =
			file::relative_path_to_abs(current_directory) + "/config.json";
		std::ofstream outfile(path);
		json data;

		data["engine_version"] = globals::engine_version;
		data["default_font_path"] = "./Roboto-Regular.ttf";

		outfile << std::setw(4) << data;
	}

	void Editor::load_config_file()
	{
		using json = nlohmann::json;
		namespace fs = std::filesystem;

		std::string current_directory = fs::current_path().generic_string();
		std::string path =
			file::relative_path_to_abs(current_directory) + "/config.json";
		std::ifstream infile(path);

		if (!infile)
		{
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
		debug_log("Config - Default Font: %s",
				  globals::editor_default_font_path.c_str());
		GameState::load_default_font(globals::editor_default_font_path);
	}

	void Editor::console_log(const char* text)
	{
		// Get time
		time_t now = time(0);
		char* current_time = ctime(&now);
		std::string time_str = std::string(current_time);
		time_str.pop_back();

		std::string message = "[LOG] (" + time_str + "): " + text + "\n";
		this->m_console_messages.push_back({MessageType::LOG, message});
	}

	void Editor::console_warn(const char* text)
	{
		// Get time
		time_t now = time(0);
		char* current_time = ctime(&now);
		std::string time_str = std::string(current_time);
		time_str.pop_back();

		std::string message = "[WARNING] (" + time_str + "): " + text + "\n";
		this->m_console_messages.push_back({MessageType::WARNING, message});
	}

	void Editor::console_error(const char* text)
	{
		// Get time
		time_t now = time(0);
		char* current_time = ctime(&now);
		std::string time_str = std::string(current_time);
		time_str.pop_back();

		std::string message = "[ERROR] (" + time_str + "): " + text + "\n";
		this->m_console_messages.push_back({MessageType::ERROR, message});
	}

	void Editor::console_clear()
	{
		this->m_console_messages.clear();
	}

	const std::vector<ConsoleMessage>& Editor::get_console_messages()
	{
		return this->m_console_messages;
	}

	void Editor::draw_ui()
	{
		rlImGuiBegin();
		ImGui::DockSpaceOverViewport();

		ui::draw_top_bar(this);
		ui::draw_object_properties();
		ui::draw_object_tree();
		ui::draw_scene_display();
		ui::draw_engine_console(this);
		ui::draw_settings();
		ui::draw_general_info_display(this);

		ui::draw_entity_create();
		ui::draw_text_create();
		ui::draw_camera_create();

		ui::draw_save_confirm_popup(this);
		ui::draw_save_as_popup(this);
		ui::draw_create_project_popup(this);

		globals::update_window_title();

		rlImGuiEnd();
	}

	void Editor::editor_input()
	{
		Vector2 mouse_position = GetMousePosition();
		Vector2 mouse_delta = GetMouseDelta();
		float mouse_wheel_move = GetMouseWheelMove();

		// Confirm changes on program exit
		if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE))
		{
			if (globals::has_unsaved_changes)
			{
				ui::show_save_confirm_popup = true;
				ui::last_action = ui::LastEditorAction::PROGRAM_EXIT;
			}
			else
			{
			    globals::program_running = false;
			}
		}

		// Camera pan
		if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON))
		{
			this->camera.target.x -= (mouse_delta.x * this->camera_move_speed);
			this->camera.target.y -= (mouse_delta.y * this->camera_move_speed);
		}

		// Camera zoom
		if (mouse_wheel_move != 0)
		{
			this->camera.zoom +=
				(mouse_wheel_move * this->camera_zoom_speed);
		}

		if (IsKeyDown(KEY_LEFT_CONTROL))
		{
			// Project save
			if (IsKeyPressed(KEY_S))
			{
				// Save as
				if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
				{
					ui::editor_save_project_as();
				}

				// Normal save
				else
				{
					ui::editor_save_project(this);
				}
			}

			// Open new project
			if (IsKeyPressed(KEY_O))
			{
				ui::editor_open_project(this);
			}

			// Create new project
			if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) &&
				IsKeyPressed(KEY_N))
			{
				ui::editor_new_project();
			}
		}
	}

	void Editor::start_game()
	{
		// TODO handle properly
		if (globals::has_unsaved_changes)
		{
			// file::save_project();
			ui::show_save_confirm_popup = true;
			return;
		}

		this->is_playing = true;

		GameState::scene.create_physics_bodies();
	}

	void Editor::end_game()
	{
		this->is_playing = false;

		std::string inspect_uuid = "";
		if (ui::view_properties_object != nullptr)
		{
			inspect_uuid = ui::view_properties_object->uuid.get_uuid();
		}
		ui::view_properties_object = nullptr;

		if (globals::is_project_loaded)
		{
			file::load_project(false);
		}

		if (inspect_uuid.length() > 0)
		{
			GameObject* inspect_object =
				GameState::scene.find_object_by_uuid(inspect_uuid);
			ui::view_properties_object = inspect_object;
		}
	}
} // namespace bacon
