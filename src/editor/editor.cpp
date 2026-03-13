#include "editor.h"

#include <ctime>
#include <filesystem>
#include <fstream>

#include "core/game_state.h"
#include "core/2D/game_state_2d.h"
#include "imgui.h"
#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"

#include "core/globals.h"
#include "core/util.h"
#include "file/file.h"
#include "ui/editor_ui.h"
#include "editor_event.h"

namespace bacon
{
	EditorSnapshot::EditorSnapshot()
	{
		framerate_limit = globals::editor_ref->get_framerate_limit();
		project_title = globals::project_title;
		editor_font_path = globals::editor_font_path;

		if (GameState::game_type == GameState::GameType::GAME_2D)
		{
			gravity = GameState::state_2d->scene->get_gravity();
			physics_steps = GameState::state_2d->scene->physics_steps;
			pixels_per_meter = GameState::state_2d->scene->get_unit_length();
		}
	}

	void EditorSnapshot::apply()
	{
		Editor* editor = globals::editor_ref;

		editor->set_framerate_limit(framerate_limit);
		globals::project_title = project_title;
		globals::editor_font_path = editor_font_path;

		if (GameState::game_type == GameState::GameType::GAME_2D)
		{
			GameState::state_2d->scene->set_gravity(gravity);
			GameState::state_2d->scene->physics_steps = physics_steps;
			GameState::state_2d->scene->set_unit_length(pixels_per_meter);
		}

		ui::SetImGuiStyle(); // For UI font update
	}

	// Editor static variables
	bool Editor::cursor_inside_scene_preview = false;
	GameObject* Editor::copy_object = nullptr;

	Editor::Editor()
	{
		this->screen_width = 1280;
		this->screen_height = 720;
		this->m_framerate_limit = 60;

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
		globals::update_window_title();
		SetExitKey(KEY_NULL); // Removes ESC as exit key

		debug_log("Raylib initialized.");

		// Load config
		this->load_config_file();

		// TODO change default
		ui::init();
	}

	Editor::~Editor()
	{
		if (Editor::copy_object)
		{
			delete Editor::copy_object;
			Editor::copy_object = nullptr;
		}
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
			else if (key == "editor_font_path")
			{
				globals::editor_font_path = value;
			}
		}
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

		if (is_playing && ui::view_properties_object != nullptr)
		{
			ui::view_properties_object->update_ui_buffer();
		}

		rlImGuiEnd();
	}

	void Editor::editor_input_2d()
	{
		static Object2D* click_drag_object = nullptr;
		static Vector2 last_mouse_position = {0.f, 0.f};

		if (GameState::state_2d == nullptr || GameState::state_2d->scene == nullptr)
		{
			return;
		}

		Object2D* inspect_object = (Object2D*)ui::view_properties_object;

		Vector2 mouse_position = (Vector2){
			ui::window_mouse_position.x,
			ui::window_mouse_position.y,
		};
		Vector2 mouse_delta = GetMouseDelta();
		Vector2 abs_mouse_delta = Vector2Subtract(mouse_position, last_mouse_position);
		float mouse_wheel_move = GetMouseWheelMove();

		// Confirm changes on program exit
		if (WindowShouldClose())
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

		// Inspect
		if (Editor::cursor_inside_scene_preview &&
			(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) ||
			IsMouseLeftDoubleClick()))
		{
			// TODO This can be optimized by using iterator over
			// the object allocators. It's verbose, but faster.
			bool found = false;
			for (Object2D* object : GameState::state_2d->scene->get_objects())
			{
				if (object->contains_point(mouse_position))
				{
					ui::view_properties_object = object;
					object->update_ui_buffer();
					found = true;
					break;
				}
			}
			if (!found)
			{
				ui::view_properties_object = nullptr;
			}

			ImGui::SetWindowFocus(NULL);
		}

		// Left click drag object
		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && Editor::cursor_inside_scene_preview)
		{
			if (inspect_object != nullptr)
			{
				if (inspect_object->contains_point(mouse_position))
				{
					click_drag_object = inspect_object;
				}

				if (click_drag_object != nullptr)
				{
					if (mouse_delta.x != 0.f || mouse_delta.y != 0.f)
					{
						Vector2 current = click_drag_object->get_position();
						Vector2 new_pos = Vector2Add(current, abs_mouse_delta);

						click_drag_object->set_position(new_pos);
					}
				}
			}
		}

		// Drag release
		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
		{
			click_drag_object = nullptr;

			if (ui::view_properties_object != nullptr)
			{
				if (ui::inspect_object_copy->get_uuid() == ui::view_properties_object->get_uuid())
				{
					Object2D* inspect_object_copy = (Object2D*)ui::inspect_object_copy;
					float dist = Vector2Distance(
						inspect_object_copy->get_position(),
						inspect_object->get_position());

					if (dist > 0)
					{
						event::ObjectEvent* event =
							new event::ObjectEvent(ui::inspect_object_copy, ui::view_properties_object);
						event::push_event(event);

						ui::inspect_object_copy->copy(*ui::view_properties_object);
						ui::view_properties_object->update_ui_buffer();
						globals::has_unsaved_changes = true;
					}
				}
			}
		}

		// Left click deselect
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && Editor::cursor_inside_scene_preview)
		{
			bool found = false;
			for (Object2D* object : GameState::state_2d->scene->get_objects())
			{
				if (object->contains_point(mouse_position))
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				ui::view_properties_object = nullptr;
				click_drag_object = nullptr;
			}
		}

		// Camera pan
		if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON) && Editor::cursor_inside_scene_preview)
		{
			this->camera.target.x -= (mouse_delta.x *
				(1 / this->camera.zoom) * this->camera_move_speed);
			this->camera.target.y -= (mouse_delta.y *
				(1 / this->camera.zoom) * this->camera_move_speed);
		}

		// Camera zoom
		if (mouse_wheel_move != 0 && Editor::cursor_inside_scene_preview)
		{
			Vector2 screen_mouse = GetWorldToScreen2D(mouse_position, this->camera);

			float scale_factor = mouse_wheel_move * this->camera_zoom_speed;
			float zoom_val = expf(logf(this->camera.zoom) + scale_factor);
			this->camera.zoom = std::clamp(zoom_val, 0.1f, 5.0f);

			// Adjust camera position
			Vector2 post_zoom_mouse = GetScreenToWorld2D(screen_mouse, this->camera);
			Vector2 delta = Vector2Subtract(mouse_position, post_zoom_mouse);
			this->camera.target.x += delta.x;
			this->camera.target.y += delta.y;
		}

		// Delete inspected object
		if (IsKeyPressed(KEY_DELETE) && ui::view_properties_object != nullptr)
		{
			// Create event
			event::ObjectDeleteEvent* event = new event::ObjectDeleteEvent(*ui::view_properties_object);
			event::push_event(event);

			// Delete and remove from scene
			ui::view_properties_object->destroy();
			delete ui::view_properties_object;
			ui::view_properties_object = nullptr;

			globals::has_unsaved_changes = true;
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

			// Undo
			if (IsKeyPressed(KEY_Z))
			{
				event::undo_event();

				if (ui::view_properties_object != nullptr)
				{
					ui::view_properties_object->update_ui_buffer();

					if (ui::inspect_object_copy != nullptr &&
						ui::view_properties_object->get_uuid() == ui::inspect_object_copy->get_uuid())
					{
						ui::inspect_object_copy->copy(*ui::view_properties_object);
					}
				}
			}

			// Redo
			if (IsKeyPressed(KEY_Y))
			{
				event::redo_event();

				if (ui::view_properties_object != nullptr)
				{
					ui::view_properties_object->update_ui_buffer();

					if (ui::inspect_object_copy != nullptr &&
						ui::view_properties_object->get_uuid() == ui::inspect_object_copy->get_uuid())
					{
						ui::inspect_object_copy->copy(*ui::view_properties_object);
					}
				}
			}

			// Copy
			if (IsKeyPressed(KEY_C) && inspect_object != nullptr)
			{
				GameObject* copy = inspect_object->clone();
				copy->clone_children(*inspect_object, false);
				Editor::copy_object = copy;
			}

			// Paste
			if (IsKeyPressed(KEY_V) && Editor::copy_object != nullptr)
			{
				GameObject* new_object = Editor::copy_object->clone_unique();
				new_object->clone_children(*Editor::copy_object, true);
				new_object->add_to_scene();

				if (GameState::game_type == GameState::GameType::GAME_2D)
				{
					Object2D* new_object_2d = (Object2D*)new_object;
					new_object_2d->set_position(mouse_position);
				}

				ui::view_properties_object = new_object;
				new_object->update_ui_buffer();

				// Create event
				event::ObjectCreateEvent* event = new event::ObjectCreateEvent(*new_object);
				event::push_event(event);

				globals::has_unsaved_changes = true;
			}
		}

		last_mouse_position = mouse_position;
	}

	void Editor::start_game()
	{
		if (globals::has_unsaved_changes)
		{
			ui::show_save_confirm_popup = true;
			ui::last_action = ui::LastEditorAction::GAME_PLAY;
			return;
		}

		debug_log("Starting game...");

		this->is_playing = true;

		if (GameState::state_2d != nullptr && GameState::state_2d->scene != nullptr)
		{
			GameState::state_2d->scene->create_physics_bodies();
		}
	}

	void Editor::end_game()
	{
		debug_log("Ending game...");

		this->is_playing = false;

		// Store last object inspected
		std::string inspect_uuid = "";
		if (ui::view_properties_object != nullptr)
		{
			inspect_uuid = ui::view_properties_object->get_uuid().as_string();
		}
		ui::view_properties_object = nullptr;

		// Reload project
		if (globals::is_project_loaded)
		{
			file::load_project(false);
		}

		// Restore inspected object
		if (inspect_uuid.length() > 0)
		{
			GameObject* inspect_object = nullptr;
			if (GameState::state_2d != nullptr && GameState::state_2d->scene != nullptr)
			{
				inspect_object =
					GameState::state_2d->scene->find_object_by_uuid(inspect_uuid);
			}
			ui::view_properties_object = inspect_object;

			if (inspect_object != nullptr)
			{
				inspect_object->update_ui_buffer();
			}
		}
	}

	uint32_t Editor::get_framerate_limit() const
	{
		return m_framerate_limit;
	}

	void Editor::set_framerate_limit(uint32_t limit)
	{
		m_framerate_limit = limit;

		SetTargetFPS(m_framerate_limit);
	}
} // namespace bacon
