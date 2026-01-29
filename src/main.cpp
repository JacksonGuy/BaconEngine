#include <iostream>

#include "core/entity.h"
#include "imgui.h"
#include "rlImGui.h"
#include "raylib.h"

#include "core/util.h"
#include "engine/engine.h"

int main(int argc, char** argv) {
    using namespace bacon;
    debug_log("Starting BaconEngine...");

    // Setup
    Engine engine;
    engine.framerate_limit = 165;

    // Create window
    InitWindow(800, 600, "Test");
    SetTargetFPS(engine.framerate_limit);
    rlImGuiSetup(true);

    // TESTING
    Entity* platform_1 = engine.manager.instantiate_entity(body_t::STATIC);
    platform_1->position = {200.f, 400.f};
    platform_1->size = {300.f, 32.f};
    platform_1->rotation = 30.f;

    Entity* platform_2 = engine.manager.instantiate_entity(body_t::STATIC);
    platform_2->position = {450.f, 500.f};
    platform_2->size = {300.f, 32.f};

    Entity* box = engine.manager.instantiate_entity(body_t::DYNAMIC);
    box->position = {250.f, 30.f};
    box->size = {32.f, 32.f};

    debug_log("Engine startup successful.");

    engine.manager.create_physics_bodies();

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE)) {
            engine.manager.deinstantiate_entity(box);
            box = nullptr;

            box = engine.manager.instantiate_entity(body_t::DYNAMIC);
            box->position = {200.f, 30.f};
            box->size = {32.f, 32.f};
            engine.manager.entity_create_body(box);
        }

        engine.manager.simulation_step();

        BeginDrawing();
            ClearBackground(LIGHTGRAY);
            DrawText("TESTING", 10, 10, 14, BLACK);

            engine.manager.draw_entities();

            // rlImGuiBegin();
            //     ImGui::Begin("Test Menu", &test_menu, ImGuiWindowFlags_MenuBar);
            //         if (ImGui::BeginMenuBar())
            //         {
            //             if (ImGui::BeginMenu("File"))
            //             {
            //                 ImGui::EndMenu();
            //             }
            //             ImGui::EndMenuBar();
            //         }
            //     ImGui::End();
            // rlImGuiEnd();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
