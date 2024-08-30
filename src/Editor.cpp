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
            Editor::sceneWindowSize = {windowSize.x, windowSize.y};
            Editor::sceneWindowMouseCapture = ImGui::IsWindowHovered();

            // Check if scene window was resized
            Vector2 frameSize = {Rendering::frame.texture.width, Rendering::frame.texture.height};
            // std::cout << "FrameSize: " << frameSize.x << "," << frameSize.y << std::endl;
            if (frameSize.x != Editor::sceneWindowSize.x || frameSize.y != Editor::sceneWindowSize.y) {
                std::cout << "Scene window resize\n";

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
    if (ImGui::BeginMainMenuBar()) {
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
                    float cameraPos[2] = {Editor::camera->camera.target.x, Editor::camera->camera.target.y};
                    if (ImGui::InputFloat2("Position", cameraPos)) {
                        Editor::camera->camera.target = {cameraPos[0], cameraPos[1]};
                    }

                    ImGui::EndTabItem();
                }
            ImGui::EndTabBar();

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
            f32 camSpeed = 10.f;
            
            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                camSpeed = 50.f;
            }

            if (IsKeyDown(KEY_W)) {
                Editor::camera->MoveCamera({0, camSpeed});
            }
            if (IsKeyDown(KEY_S)) {
                Editor::camera->MoveCamera({0, -camSpeed});
            }
            if (IsKeyDown(KEY_A)) {
                Editor::camera->MoveCamera({camSpeed, 0});
            }
            if (IsKeyDown(KEY_D)) {
                Editor::camera->MoveCamera({-camSpeed, 0});
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
    InitWindow(GameManager::screenWidth, GameManager::screenHeight, "BaconEngine");
    SetTargetFPS(GameManager::framerateLimit);
    rlImGuiSetup(true);
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
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

    Entity* platform = new Entity();
    platform->position = {100, 500};
    platform->size = {600, 32};
    platform->SetTexture("platform.png");
    platform->UpdateRect();

    while (!WindowShouldClose()) {
        f32 deltaTime = GetFrameTime();
        
        Rendering::DrawGameObjects();
        BeginDrawing();
            // BeginMode2D(GameManager::current_camera->camera);
            //     ClearBackground(DARKGRAY);
            //     Rendering::DrawGameObjects();
            //     DrawFPS(400, 400);
            // EndMode2D();
            DrawUI(deltaTime);
        EndDrawing();
        
        Update(deltaTime);        
        FixedUpdate(deltaTime);
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
