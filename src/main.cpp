#include <iostream>

#include "core/2D/entity.h"
#include "imgui.h"
#include "rlImGui.h"
#include "raylib.h"

#include "core/util.h"
#include "editor/editor.h"
#include "editor/ui/editor_ui.h"

int main(int argc, char** argv) {
    using namespace bacon;
    debug_log("Starting BaconEngine...");

    // Disable RayLib logging
    SetTraceLogLevel(LOG_WARNING);

    // Setup
    Editor editor;
    editor.framerate_limit = 165;

    // TESTING
    Entity* platform_1 = editor.manager.instantiate_entity(body_t::STATIC);
    platform_1->position = {200.f, 400.f};
    platform_1->size = {300.f, 32.f};
    platform_1->rotation = 30.f;

    Entity* platform_2 = editor.manager.instantiate_entity(body_t::STATIC);
    platform_2->position = {450.f, 500.f};
    platform_2->size = {300.f, 32.f};

    Entity* box = editor.manager.instantiate_entity(body_t::DYNAMIC);
    box->position = {250.f, 30.f};
    box->size = {32.f, 32.f};

    CameraObject* camera = editor.manager.instantiate_camera();
    camera->set_position({
        0,
        0
    });
    editor.manager.set_active_camera(camera);

    debug_log("Engine startup successful.");

    editor.manager.create_physics_bodies();

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE)) {
            editor.manager.deinstantiate_entity(box);
            box = nullptr;

            box = editor.manager.instantiate_entity(body_t::DYNAMIC);
            box->position = {200.f, 30.f};
            box->size = {32.f, 32.f};
            editor.manager.entity_create_body(box);
        }

        editor.manager.simulation_step();

        BeginDrawing();
            ClearBackground(LIGHTGRAY);
            DrawText("TESTING", 10, 10, 14, BLACK);

            editor.manager.draw_entities(&editor.camera);

            editor.draw_ui();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
