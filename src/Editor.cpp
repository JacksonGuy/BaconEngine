#include <iostream>
#include <ctime>

#include "raylib.h"
#include "box2d/box2d.h"
#include "imgui.h"
#include "rlImGui.h"
#include <nfd.h>

#include "Editor.h"
#include "GameManager.h"
#include "Rendering.h"
#include "File.h"

#define BUFFSIZE 512

namespace Editor {
    // Project Details
    std::string projectTitle = "Bacon - Untitled Project";
    bool loadedProject = false;

    // Physics
    f32 lastFixedUpdate = 0;
    f32 fixedUpdateRate = 1.0f / 60.0f;

    // Scene Window
    bool sceneWindowFocused = false;
    Vector2 sceneWindowPosition = {0,0};
    Vector2 sceneWindowSize = {0,0};
    bool sceneWindowMouseCapture = false;

    // Editor Camera
    GameCamera* camera = new GameCamera();

    // ImGui Toggles
    bool showEditorMessages = true;

    // ImGui Variables
    GameObject* viewPropertiesObject = nullptr;

    // External UI Variables
    char addVariableName[BUFFSIZE] = {0};
    i32 addVariableType = 0;
    f64 addVariableNumberVal = 0;
    char addVariableStringVal[BUFFSIZE] = {0};
};

/**
 * @brief Draw ImGui windows
 * 
 */
void DrawUI(f32 deltaTime) {
    rlImGuiBegin();
    ImGui::DockSpaceOverViewport();

    i16 fps = GetFPS();

    // Scene Window
    {
        ImGui::Begin("Scene");
            // Get scene window details
            ImVec2 windowPos = ImGui::GetWindowPos();
            ImVec2 windowSize = ImGui::GetWindowSize();

            // Set Editor variables 
            Editor::sceneWindowFocused = ImGui::IsWindowFocused();                                    
            Editor::sceneWindowPosition = {windowPos.x, windowPos.y};
            Editor::sceneWindowSize = {windowSize.x - 20, windowSize.y - 40};
            Editor::sceneWindowMouseCapture = ImGui::IsWindowHovered();

            // Check if scene window was resized
            Vector2 frameSize = {Rendering::frame.texture.width, Rendering::frame.texture.height};
            if (frameSize.x != Editor::sceneWindowSize.x || frameSize.y != Editor::sceneWindowSize.y) {

                // Maintain aspect ratio upon resize
                Vector2 ratio = {Editor::sceneWindowSize.x / frameSize.x, Editor::sceneWindowSize.y / frameSize.y};

                // Recreate Frame
                UnloadRenderTexture(Rendering::frame);
                Rendering::frame = LoadRenderTexture(frameSize.x * ratio.x, frameSize.y * ratio.y);
            }  

            // Display RenderTexture
            rlImGuiImageRenderTexture(&Rendering::frame);

        ImGui::End();
    }

    // Top Menu Bar
    {
        ImGui::BeginMainMenuBar(); 
            if (ImGui::BeginMenu("File")) {
                // We don't want the user to save and load project while testing it  
                if (GameManager::isPlayingGame) {
                    ImGui::BeginDisabled();
                }

                if (ImGui::MenuItem("New Project")) {
                    std::string path;
                    
                    if (!Editor::loadedProject) {
                        if (File::CreateNewProject(path, true)) {
                            Editor::projectTitle = path;
                            Editor::loadedProject = true;
                            std::string windowTitle = "Bacon - " + path;
                            SetWindowTitle(windowTitle.c_str());
                        }
                    }
                    else {
                        File::SaveProject(Editor::projectTitle); // Save current project before creating new one
                        if (File::CreateNewProject(path, true)) {
                            Editor::projectTitle = path;
                            Editor::loadedProject = true;
                            std::string windowTitle = "Bacon - " + path;
                            SetWindowTitle(windowTitle.c_str());
                        }
                    }
                }

                if (ImGui::MenuItem("Save")) {
                    if (!Editor::loadedProject) {
                        std::string path;
                        bool result = File::CreateNewProject(path, false);
                        if (!result) {
                            GameManager::ConsoleError("Failed to create new project");
                        }
                        else {
                            Editor::loadedProject = true;
                            Editor::projectTitle = path;
                            File::SaveProject(Editor::projectTitle);
                        }
                    }
                    else {
                        File::SaveProject(Editor::projectTitle);
                    }
                }

                if (ImGui::MenuItem("Save as")) {
                    std::string path;
                    bool result = File::CreateNewProject(path, false);
                    if (!result) {
                        GameManager::ConsoleError("Failed to create new project");
                    }
                    else {
                        Editor::loadedProject = true;
                        Editor::projectTitle = path;
                        File::SaveProject(Editor::projectTitle);
                    }
                }

                if (ImGui::MenuItem("Load")) {
                    File::LoadProject(Editor::projectTitle);
                }

                if (GameManager::isPlayingGame) {
                    ImGui::EndDisabled();
                }

                ImGui::EndMenu();
            }
        ImGui::EndMainMenuBar();
    }

    // General Menu
    {
        ImGui::Begin("Game Details");
            ImGui::Text(GameManager::engineVersion.c_str());
            ImGui::Text(("FPS: " + std::to_string(fps)).c_str());

            // Playtest game button
            if (!GameManager::isPlayingGame) {
                if (ImGui::Button("Play Game")) {
                    File::SaveProject(Editor::projectTitle);

                    // Change Camera
                    for (GameCamera* camera : GameManager::GameCameras) {
                        if (camera->isActive) {
                            GameManager::current_camera = camera;
                        }
                    }

                    GameManager::ConsoleMessage("Editor data saved. Starting game...");
                    GameManager::isPlayingGame = true;
                }
            }
            // End playtest button
            else {
                if (ImGui::Button("End Game")) {
                    GameManager::Reset();

                    File::LoadProject(Editor::projectTitle);

                    // Set camera back
                    GameManager::current_camera = Editor::camera;

                    GameManager::ConsoleMessage("Game ended. Editor data restored.");
                    GameManager::isPlayingGame = false;
                }
            }

            // General Menus
            ImGui::Separator();
            ImGui::BeginTabBar("EngineItems");
                if (ImGui::BeginTabItem("Camera")) {
                    
                    // Camera Position
                    float cameraPos[2] = {Editor::camera->camera.target.x, Editor::camera->camera.target.y};
                    if (ImGui::InputFloat2("Position", cameraPos)) {
                        Editor::camera->camera.target = {cameraPos[0], cameraPos[1]};
                    }

                    // Camera zoom
                    ImGui::InputFloat("Zoom", &Editor::camera->camera.zoom);

                    ImGui::EndTabItem();
                }
            ImGui::EndTabBar();

        ImGui::End();
    }

    // GameObject view
    {
        ImGui::Begin("Properties");
            if (Editor::viewPropertiesObject != nullptr) {
                if (Editor::viewPropertiesObject->type == ENTITY) {
                    Entity* e = (Entity*)Editor::viewPropertiesObject;
                    e->DrawPropertiesUI();
                }
                else if (Editor::viewPropertiesObject->type == TEXT) {
                    TextObject* text = (TextObject*)Editor::viewPropertiesObject;
                    text->DrawPropertiesUI();
                }
                else if (Editor::viewPropertiesObject->type == CAMERA) {
                    GameCamera* camera = (GameCamera*)Editor::viewPropertiesObject;
                    camera->DrawPropertiesUI();
                }
            }
        ImGui::End();
    }

    // Debug Console
    {
        ImGui::Begin("Console");
            if (ImGui::Button("Clear")) {
                GameManager::ConsoleMessages.clear();
                // GameManager::ConsoleBuffer.clear();
            }
            ImGui::SameLine();
            ImGui::Checkbox("Engine Messages", &Editor::showEditorMessages);
            
            ImGui::Separator();
        
            // Filter console messages
            GameManager::ConsoleBuffer.clear();
            for (std::string str : GameManager::ConsoleMessages) {
                GameManager::ConsoleBuffer.append(str.c_str());
            }

            // Display messages
            ImGui::TextUnformatted(GameManager::ConsoleBuffer.begin(), GameManager::ConsoleBuffer.end());
            
            // Auto scroll
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.f);
            }
        ImGui::End();
    }

    rlImGuiEnd();
}

/**
 * @brief Input and general updates
 * 
 */
void Update(f32 deltaTime) {
    bool windowFocused = IsWindowFocused();

    // Input for Editing
    if (!GameManager::isPlayingGame) {
        // Editor Camera Movement
        if (Editor::sceneWindowFocused && windowFocused) {
            f32 camSpeed = 8.f;
            
            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                camSpeed *= 2.0f;
            }

            if (IsKeyDown(KEY_W)) {
                Editor::camera->MoveCamera({0, -camSpeed});
            }
            if (IsKeyDown(KEY_S)) {
                Editor::camera->MoveCamera({0, camSpeed});
            }
            if (IsKeyDown(KEY_A)) {
                Editor::camera->MoveCamera({-camSpeed, 0});
            }
            if (IsKeyDown(KEY_D)) {
                Editor::camera->MoveCamera({camSpeed, 0});
            }

            // Zoom
            if (GetMouseWheelMove() > 0) {
                Editor::camera->camera.zoom += 0.1;
            }
            else if (GetMouseWheelMove() < 0) {
                Editor::camera->camera.zoom -= 0.1;
            }
        }
    }
}

/**
 * @brief Physics and Lua
 * 
 */
void FixedUpdate(f32 deltaTime) {
    if (!GameManager::isPlayingGame) return;
    
    Editor::lastFixedUpdate += deltaTime;

    if (Editor::lastFixedUpdate >= Editor::fixedUpdateRate) {
        Editor::lastFixedUpdate -= Editor::fixedUpdateRate;
        for (Entity* e : GameManager::Entities) {
            if (e->bodytype == STATIC) continue;
            
            // Set Gravity
            if (!e->grounded) {
                e->velocity.y += 9.8f * Editor::fixedUpdateRate;
            }

            // Check for Collisions
            bool bottomCollision = false;
            
            Vector2 bottomPoint = {e->rect.x + e->rect.width/2, e->rect.y + e->rect.height};
            
            for (Entity* other : GameManager::Entities) {
                if (other->ID == e->ID) continue;

                // Bottom collision
                if (CheckCollisionPointRec(bottomPoint, other->rect)) {
                    if (!e->grounded) {
                        f32 YDiff = (e->rect.y + e->rect.height) - other->rect.y;
                        e->position.y -= YDiff;
                        e->UpdateRect();
                        e->velocity.y = 0.f;
                        e->grounded = true;
                        bottomCollision = true;
                    }
                }
            }

            if (!bottomCollision) {
                e->grounded = false;
            }

            // Apply Velocity
            e->position.x += e->velocity.x;
            e->position.y += e->velocity.y;
            e->UpdateRect();
        }
    }
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

    rlImGuiReloadFonts();
}

int main() {
    // Initialize variables
    // Window
    GameManager::screenWidth = 1280;
    GameManager::screenHeight = 720;
    GameManager::framerateLimit = 244;

    // Project Details
    std::string projectTitle;
    bool loadedProject = false;

    // Scene Window
    bool sceneWindowFocused = false;
    Vector2 sceneWindowPos = {0,0};
    Vector2 sceneWindowSize = {0,0};
    Vector2 sceneMousePos = {0,0};
    bool sceneWindowMouseCapture = false;

    // Create Window and set framerate limit
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GameManager::screenWidth, GameManager::screenHeight, "BaconEngine");
    SetTargetFPS(GameManager::framerateLimit);
    rlImGuiSetup(true);
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    SetImGuiStyle();
    
    // Initialize Frame
    Rendering::frame = LoadRenderTexture(GameManager::screenWidth, GameManager::screenHeight);
    BeginTextureMode(Rendering::frame);
        ClearBackground(DARKGRAY);
    EndTextureMode();

    // Set Editor Camera
    GameManager::current_camera = Editor::camera;

    // Disable Raylib Logging
    SetTraceLogLevel(LOG_WARNING);

    // NativeFileDialog
    NFD_Init();

    // Debug
    Entity* player = new Entity();
    player->position = {200, 200};
    player->size = {64, 64};
    player->SetTexture("test.png");
    player->bodytype = DYNAMIC;
    player->UpdateRect();
    GameManager::player = player;
    Editor::viewPropertiesObject = player;

    Entity* platform = new Entity();
    platform->position = {100, 500};
    platform->size = {600, 32};
    platform->SetTexture("platform.png");
    platform->UpdateRect();

    while (!WindowShouldClose()) {
        f32 deltaTime = GetFrameTime();
        
        Rendering::DrawGameObjects();
        BeginDrawing();
            DrawUI(deltaTime);
        EndDrawing();
        
        Update(deltaTime);        
        FixedUpdate(deltaTime);
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
