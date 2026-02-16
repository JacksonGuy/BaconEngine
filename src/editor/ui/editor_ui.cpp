#include "editor_ui.h"

#include "core/game_state.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "raylib.h"
#include "rlImGui.h"

#include "core/2D/text_object.h"
#include "core/globals.h"
#include "core/util.h"
#include "editor/editor.h"
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

			SetImGuiStyle();
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
			ImGui::Begin("Properties", &show_object_properties);
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

			ImGui::Begin("Objects", &show_object_tree);
			ImGui::SetNextItemOpen(true);

			if (ImGui::TreeNodeEx("Scene", parentFlags))
			{
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

			ImGui::Begin("Scene", &show_scene_display);
			ImVec2 window_size = ImGui::GetContentRegionAvail();

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

			ImGui::Begin("Console");
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
			// debug_error("This function has not been implemented yet.");
		}

		void draw_general_info_display(Editor* editor)
		{
			ImGui::Begin("Info", &show_general_info);
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

			ImGui::ItemLabel("Camera Move Speed", ItemLabelFlag::Left);
			ImGui::InputFloat("##cameraspeed", &editor->camera_move_speed);

			ImGui::ItemLabel("Camera Zoom Speed", ItemLabelFlag::Left);
			ImGui::InputFloat("##camerazoom", &editor->camera_zoom_speed);
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
				Entity* entity = GameState::allocate_entity();
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
				TextObject* text = GameState::allocate_text_object();
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
				CameraObject* camera = GameState::allocate_camera();
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
			// TODO This might be bad.
			ImGui::PushID(object->uuid.get_left());

			auto normalFlags =
				ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			auto parentFlags = ImGuiTreeNodeFlags_OpenOnArrow |
							   ImGuiTreeNodeFlags_OpenOnDoubleClick |
							   ImGuiTreeNodeFlags_DefaultOpen;

			bool is_open = true;
			const std::vector<GameObject*>& children = object->get_children();

			// Create tree node for parent
			if (children.size() == 0)
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

			// Display children if open
			if (children.size() > 0 && is_open)
			{
				ImGui::Indent(4);

				for (GameObject* child : children)
				{
					game_object_tree_recurse(child);
				}

				ImGui::TreePop();
				ImGui::Unindent(4);
			}

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
			std::string font_path = globals::editor_default_font_path;
			io.Fonts->AddFontFromFileTTF(font_path.c_str(), fontSize);
			io.FontDefault =
				io.Fonts->AddFontFromFileTTF(font_path.c_str(), fontSize);
		}
	} // namespace ui
} // namespace bacon
