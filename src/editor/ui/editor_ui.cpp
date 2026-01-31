#include "editor_ui.h"

#include "core/globals.h"
#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"

#include "core/util.h"

namespace bacon {
    namespace ui {
        void init(uint32_t width, uint32_t height) {
            rlImGuiSetup(true);
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

            SetImGuiStyle();
        }

        void draw_top_bar() {
            ImGui::BeginMainMenuBar();
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New Project", "Ctrl-Shift-N"))
                    {
                        debug_error("This function has not been implemented yet.");
                    }

                    if (ImGui::MenuItem("Save Project", "Ctrl-S"))
                    {
                        debug_error("This function has not been implemented yet.");
                    }

                    if (ImGui::MenuItem("Save Project As...", "Ctrl-Shift-S"))
                    {
                        debug_error("This function has not been implemented yet.");
                    }

                    if (ImGui::MenuItem("Open Project", "Ctrl-O"))
                    {
                        debug_error("This function has not been implemented yet.");
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
            ImGui::EndMainMenuBar();
        }

        void draw_object_properties() {
            ImGui::Begin("Properties", &show_object_properties);
                if (view_properties_object != nullptr)
                {
                    view_properties_object->draw_properties_editor();
                }
            ImGui::End();
        }

        void draw_object_tree(GameManager* manager) {
            auto parentFlags =  ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
                                | ImGuiTreeNodeFlags_DefaultOpen;

            ImGui::Begin("Objects", &show_object_tree);
                ImGui::SetNextItemOpen(true);

                if (ImGui::TreeNodeEx("Scene", parentFlags))
                {
                    for (GameObject* obj : manager->get_objects()) {
                        if (obj->get_parent() == nullptr) {
                            game_object_tree_recurse(obj);
                        }
                    }

                    ImGui::TreePop();
                }

            ImGui::End();
        }

        void draw_scene_display(Renderer* renderer) {
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

        void draw_engine_console() {
            // debug_error("This function has not been implemented yet.");
        }

        void draw_settings() {
            // debug_error("This function has not been implemented yet.");
        }

        void draw_general_info_display() {
            ImGui::Begin("Info", &show_general_info);
                int fps = GetFPS();
                std::string title = "Bacon Engine " + engine_version;
                ImGui::Text("%s", title.c_str());
                ImGui::Text("FPS: %i", fps);
            ImGui::End();
        }

        void game_object_tree_recurse(GameObject* object) {
            ImGui::PushID(object->get_uid());

            auto normalFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            auto parentFlags =  ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
                                | ImGuiTreeNodeFlags_DefaultOpen;

            bool is_open = true;
            const std::vector<GameObject*>& children = object->get_children();

            // Create tree node for parent
            if (children.size() == 0) {
                ImGui::TreeNodeEx(object->name.c_str(), normalFlags);
            }
            else {
                is_open = ImGui::TreeNodeEx(object->name.c_str(), parentFlags);
            }

            // View object properties if right clicked
            if (ImGui::IsItemClicked(1)) {
                view_properties_object = object;
            }

            // Display children if open
            if (children.size() > 0 && is_open) {
                ImGui::Indent(4);

                for (GameObject* child : children) {
                    game_object_tree_recurse(child);
                }

                ImGui::TreePop();
                ImGui::Unindent(4);
            }

            ImGui::PopID();
        }

        void SetImGuiStyle() {
            auto& colors = ImGui::GetStyle().Colors;
            colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

            // Headers
            colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
            colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
            colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

            // Buttons
            colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
            colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
            colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

            // Frame BG
            colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
            colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
            colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

            // Tabs
            colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
            colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
            colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
            colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
            colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

            // Title
            colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
            colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
            colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

            // Font
            ImGuiIO& io = ImGui::GetIO();
            io.Fonts->Clear();

            float fontSize = 18.f;
            io.Fonts->AddFontFromFileTTF("Roboto-Regular.ttf", fontSize);
            io.FontDefault = io.Fonts->AddFontFromFileTTF("Roboto-Regular.ttf", fontSize);
        }
    }
}
