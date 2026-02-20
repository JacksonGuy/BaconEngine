#include "editor_ui.h"

#include "core/game_state.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include "raylib.h"
#include "rlImGui.h"

#include "core/2D/text_object.h"
#include "core/globals.h"
#include "core/util.h"
#include "editor/editor.h"
#include "editor/editor_event.h"
#include "editor/ui/imgui_extras.h"
#include "file/file.h"

namespace bacon
{
	namespace ui
	{
		void init(uint32_t width, uint32_t height)
		{
			rlImGuiSetup(true);
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigDockingWithShift = true;

			global_window_flags = ImGuiWindowFlags_None;
			if (!move_windows)
			{
				global_window_flags |= ImGuiWindowFlags_NoMove;
			}

			SetImGuiStyle();
		}

		void set_input_buffers()
		{
			// Settings window buffers
			settings::project_title = globals::project_title;
			settings::gravity = GameState::scene.get_gravity();
			settings::physics_steps = GameState::scene.physics_steps;
			settings::pixels_per_meter = GameState::scene.get_unit_length();
		}

		void draw_top_bar(Editor* editor)
		{
			ImGui::BeginMainMenuBar();
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Project", "Ctrl-Shift-N"))
				{
					ui::editor_new_project();
				}

				if (ImGui::MenuItem("Save Project", "Ctrl-S"))
				{
					ui::editor_save_project(editor);
				}

				if (ImGui::MenuItem("Save Project As...", "Ctrl-Shift-S"))
				{
					ui::editor_save_project_as();
				}

				if (ImGui::MenuItem("Open Project", "Ctrl-O"))
				{
					ui::editor_open_project(editor);
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Settings"))
				{
					ui::show_settings = true;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "Ctrl-Z"))
				{
					event::undo_event();
				}

				if (ImGui::MenuItem("Redo", "Ctrl-Y"))
				{
					event::redo_event();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Create"))
			{
				ImGui::MenuItem("Entity", NULL, &show_entity_create);
				ImGui::MenuItem("Text", NULL, &show_text_create);
				ImGui::MenuItem("Camera", NULL, &show_camera_create);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Testing"))
			{
				if (ImGui::MenuItem("Testing"))
				{
					ui::show_save_confirm_popup = true;
				}

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		void draw_object_properties()
		{
			ImGui::Begin("Properties", &show_object_properties, global_window_flags);
			if (view_properties_object != nullptr)
			{
				view_properties_object->draw_properties_editor();
			}
			ImGui::End();
		}

		void draw_object_tree()
		{
			auto parentFlags = ImGuiTreeNodeFlags_OpenOnArrow |
							   ImGuiTreeNodeFlags_OpenOnDoubleClick |
							   ImGuiTreeNodeFlags_DefaultOpen;

			ImGui::Begin("Objects", &show_object_tree, global_window_flags);
			ImGui::SetNextItemOpen(true);

			if (ImGui::TreeNodeEx("Scene", parentFlags))
			{
				// Handle drag-drop onto root scene node
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OBJECT_UUID"))
					{
						// Find object
						UUID source_uuid = *(UUID*)payload->Data;
						GameObject* object = GameState::scene.find_object_by_uuid(source_uuid);

						if (object != nullptr)
						{
							// Remove from current parent if object has one
							GameObject* parent = object->get_parent();
							if (parent != nullptr)
							{
								parent->remove_child(object);
								object->set_parent(nullptr);
							}

							event::TreeEvent* event = new event::TreeEvent();
							event->object = object;
							event->old_parent = parent;
							event->new_parent = nullptr;

							event::push_event(event);
							globals::has_unsaved_changes = true;
						}
					}

					ImGui::EndDragDropTarget();
				}

				for (GameObject* obj : GameState::scene.get_objects())
				{
					if (obj->get_parent() == nullptr)
					{
						game_object_tree_recurse(obj);
					}
				}

				ImGui::TreePop();
			}

			ImGui::End();
		}

		void draw_scene_display()
		{
			Renderer* renderer = GameState::renderer;

			ImGui::Begin("Scene", &show_scene_display, global_window_flags);

			ui::window_position = ImGui::GetWindowPos();
			ui::window_size = ImGui::GetContentRegionAvail();

			ImGuiWindow* window = ImGui::GetCurrentWindow();
			const ImRect title_bar = window->TitleBarRect();
			const float title_bar_height = title_bar.Max.y - title_bar.Min.y;

			ImVec2 mouse_pos = ImGui::GetMousePos();
			ImVec2 padding = ImGui::GetStyle().WindowPadding;
			ImVec2 relative_mouse_pos = ImVec2(
				mouse_pos.x - window_position.x - padding.x,
				mouse_pos.y - window_position.y - title_bar_height - padding.y);
			ui::window_mouse_position = GetScreenToWorld2D(
				(Vector2){relative_mouse_pos.x, relative_mouse_pos.y},
				globals::editor_ref->camera);

			// Resize frame to correct window size
			if (renderer->get_width() != window_size.x ||
				renderer->get_height() != window_size.y)
			{
				renderer->create_frame(window_size.x, window_size.y);
			}

			rlImGuiImageRenderTexture(&renderer->frame);
			ImGui::End();
		}

		void draw_engine_console(Editor* editor)
		{
			static bool console_show_errors = true;
			static bool console_show_warnings = true;
			static bool console_show_logs = true;

			const ImVec4 warning_color = {255, 165, 0, 1.f};
			const ImVec4 error_color = {255, 0, 0, 1.f};

			const std::vector<ConsoleMessage>& messages =
				editor->get_console_messages();

			ImGui::Begin("Console", &ui::show_console, global_window_flags);

			ImGui::Checkbox("Logs", &console_show_logs);
			ImGui::SameLine();
			ImGui::Checkbox("Warnings", &console_show_warnings);
			ImGui::SameLine();
			ImGui::Checkbox("Errors", &console_show_errors);
			ImGui::SameLine();
			if (ImGui::Button("Clear"))
			{
				editor->console_clear();
			}

			ImGui::Separator();

			ImGui::BeginChild("scrollRegion");
			for (const ConsoleMessage& message : messages)
			{
				if (message.type == MessageType::LOG && console_show_logs)
				{
					ImGui::TextUnformatted(message.message.c_str());
				}
				else if (message.type == MessageType::WARNING &&
						 console_show_warnings)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, warning_color);
					ImGui::TextUnformatted(message.message.c_str());
					ImGui::PopStyleColor();
				}
				else if (message.type == MessageType::ERROR &&
						 console_show_errors)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, error_color);
					ImGui::TextUnformatted(message.message.c_str());
					ImGui::PopStyleColor();
				}
			}

			// Auto scroll to bottom
			if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			{
				ImGui::SetScrollHereY(1.f);
			}
			ImGui::EndChild();

			ImGui::End();
		}

		void draw_settings()
		{
			// WARNING!!
			using namespace event;

			if (!ui::show_settings)
				return;

			ImGuiWindowFlags settings_flags = global_window_flags;
			if (!move_windows)
			{
				settings_flags ^= ImGuiWindowFlags_NoMove;
			}

			ImGui::Begin("Settings", &ui::show_settings, settings_flags);

			if (ImGui::BeginTabBar("##tab_bar"))
			{
				if (ImGui::BeginTabItem("Editor"))
				{
					ImGui::ItemLabel("Project Title:", ItemLabelFlag::Left);
					if (ImGui::InputText("##project_title",
										 &settings::project_title,
										 ImGuiInputTextFlags_EnterReturnsTrue))
					{
						globals::has_unsaved_changes = true;
						EditorEvent* event = new EditorEvent();
						event->before = new EditorSnapshot();

						globals::project_title = settings::project_title;

						event->after = new EditorSnapshot();
						push_event(event);
					}

					ImGui::ItemLabel("Move windows:", ItemLabelFlag::Left);
					if (ImGui::Checkbox("##docking", &ui::move_windows))
					{
						ui::global_window_flags ^= ImGuiWindowFlags_NoMove;
					}

					ImGuiIO& io = ImGui::GetIO();
					ImGui::ItemLabel("Shift Dock:", ItemLabelFlag::Left);
					ImGui::Checkbox("##shift_dock", &io.ConfigDockingWithShift);

					ImGui::ItemLabel("Preview Framerate Max", ItemLabelFlag::Left);
					ImGui::InputScalar("##framerate", ImGuiDataType_U32, &settings::framerate_limit);
					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						// This probably isn't necessary for Editor changes.
						// We should probably just be saving settings to config file
						// after edit instead.
						// globals::has_unsaved_changes = true;

						EditorEvent* event = new EditorEvent();
						event->before = new EditorSnapshot();

						globals::editor_ref->set_framerate_limit(settings::framerate_limit);

						event->after = new EditorSnapshot();
						push_event(event);
					}

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Project"))
				{
					ImGui::ItemLabel("Gravity:", ItemLabelFlag::Left);
					ImGui::InputFloat("##gravity", &settings::gravity);
					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						globals::has_unsaved_changes = true;
						EditorEvent* event = new EditorEvent();
						event->before = new EditorSnapshot();

						GameState::scene.set_gravity(settings::gravity);

						event->after = new EditorSnapshot();
						push_event(event);
					}

					ImGui::ItemLabel("Physics Steps:", ItemLabelFlag::Left);
					ImGui::InputInt("##phys_step", &settings::physics_steps);
					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						globals::has_unsaved_changes = true;
						EditorEvent* event = new EditorEvent();
						event->before = new EditorSnapshot();

						GameState::scene.physics_steps = settings::physics_steps;

						event->after = new EditorSnapshot();
						push_event(event);
					}

					ImGui::ItemLabel("Pixels per Meter:", ItemLabelFlag::Left);
					ImGui::InputFloat("##units_per_meter", &settings::pixels_per_meter);
					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						globals::has_unsaved_changes = true;
						EditorEvent* event = new EditorEvent();
						event->before = new EditorSnapshot();

						GameState::scene.set_unit_length(settings::pixels_per_meter);

						event->after = new EditorSnapshot();
						push_event(event);
					}

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

			ImGui::End();
		}

		void draw_general_info_display(Editor* editor)
		{
			ImGui::Begin("Info", &show_general_info, global_window_flags);
			int fps = GetFPS();
			std::string title = "Bacon Engine " + globals::engine_version;
			ImGui::Text("%s", title.c_str());
			ImGui::Text("FPS: %i", fps);

			ImGui::Separator();

			if (editor->is_playing)
			{
				if (ImGui::Button("Stop Game"))
				{
					editor->end_game();
				}
			}
			else
			{
				if (ImGui::Button("Play Game"))
				{
					editor->start_game();
				}
			}

			ImGui::Separator();

			ImGui::Text("Cursor: (%.0f, %.0f)",
						ui::window_mouse_position.x,
						ui::window_mouse_position.y);

			ImGui::ItemLabel("Camera Move Speed", ItemLabelFlag::Left);
			ImGui::InputFloat("##camera_speed", &editor->camera_move_speed);

			ImGui::ItemLabel("Camera Zoom Speed", ItemLabelFlag::Left);
			ImGui::InputFloat("##camera_zoom_speed", &editor->camera_zoom_speed);

			ImGui::ItemLabel("Camera Position", ItemLabelFlag::Left);
			float camera_pos[] = {editor->camera.target.x, editor->camera.target.y};
			if (ImGui::InputFloat2("##camera_pos", camera_pos))
			{
				editor->camera.target = (Vector2){
					camera_pos[0],
					camera_pos[1],
				};
			}

			ImGui::ItemLabel("Camera Zoom", ItemLabelFlag::Left);
			ImGui::InputFloat("##camera_zoom", &editor->camera.zoom);

			ImGui::End();
		}

		void draw_entity_create()
		{
			static char name_buffer[256];
			static char texture_path_buffer[512];
			static float position_buffer[] = {0.f, 0.f};
			static float size_buffer[] = {32.f, 32.f};

			if (!ui::show_entity_create)
				return;

			ImGui::Begin("Create Entity", &ui::show_entity_create);

			ImGui::ItemLabel("Name", ItemLabelFlag::Left);
			ImGui::InputText("##name", name_buffer, 256);

			ImGui::ItemLabel("Position", ItemLabelFlag::Left);
			ImGui::InputFloat2("##position", position_buffer);

			ImGui::ItemLabel("Size", ItemLabelFlag::Left);
			ImGui::InputFloat2("##size", size_buffer);

			if (ImGui::Button("Create"))
			{
				Entity* entity = new Entity();
				GameState::scene.add_entity(entity);

				entity->position =
					(Vector2){.x = position_buffer[0], .y = position_buffer[1]};
				entity->set_size(size_buffer[0], size_buffer[1]);

				ui::show_entity_create = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ui::show_entity_create = false;
			}

			ImGui::End();
		}

		void draw_text_create()
		{
			static char name_buffer[256];
			static float position_buffer[] = {0.f, 0.f};

			if (!ui::show_text_create)
				return;

			ImGui::Begin("Create Text", &ui::show_text_create);

			ImGui::ItemLabel("Name", ItemLabelFlag::Left);
			ImGui::InputText("##name", name_buffer, 256);

			ImGui::ItemLabel("Position", ItemLabelFlag::Left);
			ImGui::InputFloat2("##position", position_buffer);

			if (ImGui::Button("Create"))
			{
				TextObject* text = new TextObject();
				GameState::scene.add_text_object(text);

				text->name = name_buffer;
				text->position =
					(Vector2){position_buffer[0], position_buffer[1]};

				ui::show_text_create = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ui::show_text_create = false;
			}
			ImGui::End();
		}

		void draw_camera_create()
		{
			static std::string name_buffer;

			if (!ui::show_camera_create)
				return;

			ImGui::Begin("Create Camera", &ui::show_camera_create);

			ImGui::ItemLabel("Name", ItemLabelFlag::Left);
			ImGui::InputText("##name", &name_buffer);

			if (ImGui::Button("Create"))
			{
				CameraObject* camera = new CameraObject();
				GameState::scene.add_camera(camera);

				camera->name = name_buffer;

				ui::show_camera_create = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ui::show_camera_create = false;
			}

			ImGui::End();
		}

		void draw_save_confirm_popup(Editor* editor)
		{
			if (ui::show_save_confirm_popup)
			{
				bool button_pressed = false;
				ImGui::OpenPopup("Save Project?");

				if (ImGui::BeginPopupModal("Save Project?",
										   &ui::show_save_confirm_popup,
										   ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("You have unsaved edits. Do you want to save?");

					if (ImGui::Button("Save"))
					{
						if (globals::is_project_loaded)
						{
							file::save_project();
							ui::show_save_confirm_popup = false;
							button_pressed = true;
						}
						else
						{
							ui::show_save_as_popup = true;
							ui::show_save_confirm_popup = false;
							button_pressed = false;
						}
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::Button("Discard"))
					{
						ui::show_save_confirm_popup = false;
						button_pressed = true;
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::Button("Cancel"))
					{
						ui::show_save_confirm_popup = false;
						button_pressed = false;
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				if (button_pressed)
				{
					switch (ui::last_action)
					{
						case LastEditorAction::PROJECT_NEW:
						{
							ui::show_create_new_project = true;
							break;
						}

							// This shouldn't be a case?
							// case LastEditorAction::PROJECT_SAVE_AS:
							// {
							// 	ui::show_save_as_popup = true;
							// }

						case LastEditorAction::PROJECT_OPEN:
						{
							file::load_project(true);
							break;
						}

						case LastEditorAction::PROGRAM_EXIT:
						{
							globals::program_running = false;
							break;
						}

						default:
						{
							break;
						}
					}
				}
			}
		}

		void draw_save_as_popup(Editor* editor)
		{
			static std::string project_title = globals::project_title;
			static std::string project_path = "";

			if (ui::show_save_as_popup)
			{
				ImGui::OpenPopup("Save as");

				if (ImGui::BeginPopupModal("Save as", &ui::show_save_as_popup,
										   ImGuiWindowFlags_AlwaysAutoResize))
				{

					ImGui::Text("Save project");

					ImGui::Text("Project Name");
					ImGui::InputText("##project_name", &project_title);

					ImGui::Text("Path");
					ImGui::InputText("##project_path", &project_path);
					if (ImGui::Button("Select Folder"))
					{
						nfdu8char_t* outpath = NULL;
						nfdpickfolderu8args_t args = {0};
						nfdresult_t result =
							NFD_PickFolderU8_With(&outpath, &args);

						if (result == NFD_ERROR)
						{
							debug_error("Failed to create new project.");
						}
						else if (result == NFD_CANCEL)
						{
							// Do nothing
						}
						else
						{
							project_path = outpath;
						}

						free(outpath);
					}

					if (ImGui::Button("Create"))
					{
						globals::project_directory = project_path;
						globals::project_file = project_path + "//game.json";
						globals::is_project_loaded = true;

						file::create_new_project();
						file::save_project();

						ui::show_save_as_popup = false;
					}

					ImGui::EndPopup();
				}
			}
		}

		void draw_create_project_popup(Editor* editor)
		{
			static std::string project_title = "Project";
			static std::string project_path = "";

			if (ui::show_create_new_project)
			{
				ImGui::OpenPopup("Create New Project");

				if (ImGui::BeginPopupModal("Create New Project",
										   &ui::show_create_new_project,
										   ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("Create New Project");

					ImGui::Text("Project Name");
					ImGui::InputText("##project_name", &project_title);

					ImGui::Text("Path");
					ImGui::InputText("##project_path", &project_path);
					if (ImGui::Button("Select Folder"))
					{
						nfdu8char_t* outpath = NULL;
						nfdpickfolderu8args_t args = {0};
						nfdresult_t result =
							NFD_PickFolderU8_With(&outpath, &args);

						if (result == NFD_ERROR)
						{
							debug_error("Failed to create new project.");
						}
						else if (result == NFD_CANCEL)
						{
							// Do nothing
						}
						else
						{
							project_path = outpath;
						}

						free(outpath);
					}

					if (ImGui::Button("Create"))
					{
						globals::project_directory = project_path;
						globals::project_file = project_path + "//game.json";
						globals::is_project_loaded = true;

						file::create_new_project();

						ui::show_create_new_project = false;
					}

					ImGui::EndPopup();
				}
			}
		}

		void game_object_tree_recurse(GameObject* object)
		{
			if (object == nullptr)
				return;

			// TODO This might be bad.
			ImGui::PushID(object);

			auto normalFlags =
				ImGuiTreeNodeFlags_Leaf; // | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			auto parentFlags = ImGuiTreeNodeFlags_OpenOnArrow |
							   ImGuiTreeNodeFlags_OpenOnDoubleClick |
							   ImGuiTreeNodeFlags_DefaultOpen;

			bool is_open = true;

			// Create tree node for parent
			if (object->get_children().size() == 0)
			{
				ImGui::TreeNodeEx(object->name.c_str(), normalFlags);
			}
			else
			{
				is_open = ImGui::TreeNodeEx(object->name.c_str(), parentFlags);
			}

			// View object properties if right clicked
			if (ImGui::IsItemClicked(1))
			{
				view_properties_object = object;
			}

			// Handle mouse drag
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("OBJECT_UUID", &object->uuid, sizeof(object->uuid));
				ImGui::EndDragDropSource();
			}

			// Handle mouse drop
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OBJECT_UUID"))
				{
					UUID source_uuid = *(UUID*)payload->Data;
					GameObject* source_obj = GameState::scene.find_object_by_uuid(source_uuid);

					if (source_obj != nullptr)
					{
						GameObject* parent = source_obj->get_parent();

						// Remove from current parent if object has one
						if (parent != nullptr)
						{
							parent->remove_child(source_obj);
						}

						// Add to new parent
						object->add_child(source_obj);
						source_obj->set_parent(object);

						event::TreeEvent* event = new event::TreeEvent();
						event->object = source_obj;
						event->old_parent = parent;
						event->new_parent = object;

						event::push_event(event);
						globals::has_unsaved_changes = true;
					}
				}

				ImGui::EndDragDropTarget();
			}

			// Display children if open
			auto children = object->get_children();
			if (children.size() > 0 && is_open)
			{
				ImGui::Indent(4);

				for (GameObject* child : children)
				{
					game_object_tree_recurse(child);
				}

				ImGui::Unindent(4);
			}

			if (is_open)
				ImGui::TreePop();
			ImGui::PopID();
		}

		void editor_new_project()
		{
			ui::last_action = LastEditorAction::PROJECT_NEW;

			if (globals::has_unsaved_changes)
			{
				if (globals::is_project_loaded)
				{
					ui::show_save_confirm_popup = true;
				}
				else
				{
					ui::show_save_as_popup = true;
				}
			}
			else
			{
				ui::show_create_new_project = true;
			}
		}

		void editor_save_project(Editor* editor)
		{
			ui::last_action = LastEditorAction::PROJECT_SAVE;

			if (globals::is_project_loaded)
			{
				file::save_project();
			}
			else
			{
				ui::show_save_as_popup = true;
			}
		}

		void editor_save_project_as()
		{
			ui::last_action = LastEditorAction::PROJECT_SAVE_AS;
			ui::show_save_as_popup = true;
		}

		void editor_open_project(Editor* editor)
		{
			ui::last_action = LastEditorAction::PROJECT_OPEN;

			if (globals::has_unsaved_changes)
			{
				if (globals::is_project_loaded)
				{
					ui::show_save_confirm_popup = true;
				}
				else
				{
					ui::show_save_as_popup = true;
				}
			}
			else
			{
				file::load_project(true);
			}
		}

		void SetImGuiStyle()
		{
			auto& colors = ImGui::GetStyle().Colors;
			colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};

			// Headers
			colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
			colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
			colors[ImGuiCol_HeaderActive] =
				ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

			// Buttons
			colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
			colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
			colors[ImGuiCol_ButtonActive] =
				ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

			// Frame BG
			colors[ImGuiCol_FrameBg] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
			colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
			colors[ImGuiCol_FrameBgActive] =
				ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

			// Tabs
			colors[ImGuiCol_Tab] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
			colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
			colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
			colors[ImGuiCol_TabUnfocused] =
				ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
			colors[ImGuiCol_TabUnfocusedActive] =
				ImVec4{0.2f, 0.205f, 0.21f, 1.0f};

			// Title
			colors[ImGuiCol_TitleBg] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
			colors[ImGuiCol_TitleBgActive] =
				ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
			colors[ImGuiCol_TitleBgCollapsed] =
				ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

			// Font
			ImGuiIO& io = ImGui::GetIO();
			io.Fonts->Clear();

			float fontSize = 18.f;
			std::string font_path = globals::editor_font_path;
			io.Fonts->AddFontFromFileTTF(font_path.c_str(), fontSize);
			io.FontDefault =
				io.Fonts->AddFontFromFileTTF(font_path.c_str(), fontSize);
		}
	} // namespace ui
} // namespace bacon
