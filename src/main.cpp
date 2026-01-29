#include <iostream>

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

    // Create window
    InitWindow(800, 600, "Test");
    SetTargetFPS(engine.framerate_limit);
    rlImGuiSetup(true);

    // TESTING
    Entity* platform = engine.manager.instantiate_entity(body_t::STATIC);
    platform->position = {400.f, 500.f};
    platform->size = {400.f, 32.f};

    Entity* box = engine.manager.instantiate_entity(body_t::DYNAMIC);
    box->position = {400.f, 30.f};
    box->size = {32.f, 32.f};

    debug_log("Engine startup successful.");

    engine.manager.create_physics_bodies();

    while (!WindowShouldClose())
    {
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
