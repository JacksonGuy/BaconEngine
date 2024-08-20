#include <iostream>

#include "raylib.h"
#include "box2d/box2d.h"
#include "imgui.h"
#include "rlImGui.h"

#include "GameManager.h"
#include "Rendering.h"

#define BUFFSIZE 512

/**
 * @brief Draw ImGui windows
 * 
 */
void DrawUI(f32 deltaTime) {
    rlImGuiBegin();

    ImGui::Begin("Test");
        ImGui::Text("Something");
    ImGui::End();

    rlImGuiEnd();
}

/**
 * @brief Input
 * 
 */
void Update(f32 deltaTime) {
    Rendering::DrawGameObjects();
}

/**
 * @brief Physics and Lua
 * 
 */
void FixedUpdate(f32 deltaTime) {

}

int main() {
    GameManager::screenWidth = 800;
    GameManager::screenHeight = 600;
    GameManager::framerateLimit = 244;

    // Initialize variables
    // Project Details
    std::string projectTitle;
    bool loadedProject = false;

    // Scene Window
    bool sceneWindowFocused = false;
    Vector2 sceneWindowPos = {0,0};
    Vector2 sceneWindowSize = {0,0};
    Vector2 sceneMousePos = {0,0};
    bool sceneWindowMouseCapture = false;

    Entity* test = new Entity();
    test->size = {128, 128};
    test->SetTexture("player.jpg");

    // Create Window and set framerate limit
    InitWindow(GameManager::screenWidth, GameManager::screenHeight, "BaconEngine");
    SetTargetFPS(GameManager::framerateLimit);
    rlImGuiSetup(true);

    while (!WindowShouldClose()) {
        f32 deltaTime = GetFrameTime();
        
        Rendering::DrawGameObjects(); // Update frame
        BeginDrawing();
            // TODO replace dimensions here with scene window size
            DrawTextureRec(
                Rendering::frame.texture, 
                {0, 0, (float)GameManager::screenWidth, (float)GameManager::screenHeight},
                {0,0},
                WHITE
            );
            DrawUI(deltaTime); // This comes last 
        EndDrawing();
        
        Update(deltaTime);        
        FixedUpdate(deltaTime);
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
