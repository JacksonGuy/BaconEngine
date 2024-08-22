#include <iostream>

#include "raylib.h"
#include "box2d/box2d.h"
#include "imgui.h"
#include "rlImGui.h"

#include "GameManager.h"
#include "Rendering.h"

// #define BOX2DPHYS
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
    Vector2 pos = GameManager::player->position;
    
    #ifdef BOX2DPHYS
        b2Vec2 bpos = b2Body_GetPosition(GameManager::player->body);
        f32 speed = 900;

        if (IsKeyDown(KEY_A)) {
            b2Body_ApplyLinearImpulse(GameManager::player->body, {-speed, 0}, bpos, true);
        }
        if (IsKeyDown(KEY_D)) {
            b2Body_ApplyLinearImpulse(GameManager::player->body, {speed, 0}, bpos, true);
        }
        if (IsKeyDown(KEY_SPACE)) {
            // b2Body_ApplyLinearImpulse(GameManager::player->body, {0, speed * 100}, bpos, true);
            b2Body_ApplyForce(GameManager::player->body, {0, -speed * 500}, bpos, true);
        }
    #else
        f32 speed = 10;
        f32 jump = 7;

        if (IsKeyDown(KEY_A)) {
            GameManager::player->position.x -= speed;
        }
        if (IsKeyDown(KEY_D)) {
            GameManager::player->position.x += speed;
        }
        if (IsKeyPressed(KEY_SPACE)) {
            GameManager::player->velocity.y -= jump;
        }

        GameManager::player->UpdateRect();
    #endif

    Rendering::DrawGameObjects();
}

/**
 * @brief Physics and Lua
 * 
 */
void FixedUpdate(f32 deltaTime) {
    #ifdef BOX2DPHYS
        f32 timestep = 1.0f / 60.f;
        b2World_Step(GameManager::world, timestep, 4);

        for (Entity* e : GameManager::Entities) {
            e->UpdateEntityFromPhysics();
        }
    #else
        for (Entity* e : GameManager::Entities) {
            if (e->bodytype == STATIC) continue;
            
            // Set Gravity
            if (!e->grounded) {
                e->velocity.y += 9.8f * deltaTime;
            }

            // Check for Collisions
            bool bottomCollision = false;
            
            Vector2 bottomPoint = {e->rect.x + e->rect.width/2, e->rect.y + e->rect.height};
            
            for (Entity* other : GameManager::Entities) {
                if (other->ID == e->ID) continue;

                // Bottom collision
                if (CheckCollisionPointRec(bottomPoint, other->rect)) {
                    f32 YDiff = (e->rect.y + e->rect.height) - other->rect.y;
                    e->position.y -= YDiff;
                    e->UpdateRect();
                    if (!e->grounded) {
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
    #endif
}

int main() {
    GameManager::screenWidth = 800;
    GameManager::screenHeight = 600;
    GameManager::framerateLimit = 60;

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

    // Create Window and set framerate limit
    InitWindow(GameManager::screenWidth, GameManager::screenHeight, "BaconEngine");
    SetTargetFPS(GameManager::framerateLimit);
    rlImGuiSetup(true);
    
    // Initialize Frame
    Rendering::frame = LoadRenderTexture(GameManager::screenWidth, GameManager::screenHeight);
    BeginTextureMode(Rendering::frame);
        ClearBackground(DARKGRAY);
    EndTextureMode();

    // Physics Setup
    GameManager::CreateBox2DWorld();

    // Debug
    Entity* player = new Entity();
    player->position = {200, 200};
    player->size = {64, 64};
    player->SetTexture("test.png");
    player->bodytype = DYNAMIC;
    player->UpdateRect();
    // player->CreateBody();
    GameManager::player = player;

    Entity* platform = new Entity();
    platform->position = {100, 500};
    platform->size = {600, 32};
    platform->SetTexture("platform.png");
    platform->UpdateRect();
    // platform->CreateBody();

    while (!WindowShouldClose()) {
        f32 deltaTime = GetFrameTime();
        
        Rendering::DrawGameObjects(); // Update frame
        BeginDrawing();
            // TODO replace dimensions here with scene window size
            DrawTextureRec(
                Rendering::frame.texture, 
                {0, 0, (float)Rendering::frame.texture.width, (float)-Rendering::frame.texture.height},
                {0,0},
                WHITE
            );
            DrawFPS(0,0);
            DrawUI(deltaTime); // This comes last 
        EndDrawing();
        
        Update(deltaTime);        
        FixedUpdate(deltaTime);
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
