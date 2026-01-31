#include "editor_ui.h"

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

        void draw_object_tree(GameObject *object) {
            // debug_error("This function has not been implemented yet.");
        }

        void draw_scene_display(Renderer* renderer) {
            ImGui::Begin("Scene");
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
            // debug_error("This function has not been implemented yet.");
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
