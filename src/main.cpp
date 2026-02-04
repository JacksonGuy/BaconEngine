#include <iostream>

#include "file/file.h"
#include "imgui.h"
#include "nfd.h"
#include "rlImGui.h"
#include "raylib.h"

#include "core/2D/entity.h"
#include "core/globals.h"
#include "core/util.h"
#include "editor/editor.h"
#include "editor/ui/editor_ui.h"

int main(int argc, char** argv) {
    using namespace bacon;
    debug_log("Starting BaconEngine...");

    globals::engine_version = "v0.1";

    // Disable RayLib logging
    SetTraceLogLevel(LOG_WARNING);

    // NativeFileDialog
    NFD_Init();

    // Setup
    Editor editor;
    editor.framerate_limit = 165;

    // TESTING
    // Entity* platform_1 = editor.manager.instantiate_entity(BodyType::STATIC);
    // platform_1->position = {200.f, 400.f};
    // platform_1->size = {300.f, 32.f};
    // platform_1->rotation = 30.f;

    // Entity* platform_2 = editor.manager.instantiate_entity(BodyType::STATIC);
    // platform_2->position = {450.f, 500.f};
    // platform_2->size = {300.f, 32.f};

    // Entity* box = editor.manager.instantiate_entity(BodyType::DYNAMIC);
    // box->position = {250.f, 30.f};
    // box->size = {32.f, 32.f};

    // CameraObject* camera = editor.manager.instantiate_camera();
    // camera->set_position({
    //     0,
    //     0
    // });
    // editor.manager.set_active_camera(camera);
    // editor.manager.create_physics_bodies();

    // globals::project_directory = "~/BaconEngine/projects/test2";
    // globals::project_file = "~/BaconEngine/projects/test2/game.json";
    file::load_project(editor.manager);

    debug_log("Engine startup successful.");

    while (!WindowShouldClose())
    {
        // if (IsKeyPressed(KEY_SPACE)) {
        //     editor.manager.deinstantiate_entity(box);
        //     box = nullptr;

        //     box = editor.manager.instantiate_entity(BodyType::DYNAMIC);
        //     box->position = {200.f, 30.f};
        //     box->size = {32.f, 32.f};
        //     editor.manager.entity_create_body(box);
        // }

        if (editor.is_playing)
        {
            // Do physics step
            editor.manager.simulation_step();
        }

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
