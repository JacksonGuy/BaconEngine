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
    debug_log("NativeFileDialog initialized.");

    // Setup
    Editor editor;
    editor.framerate_limit = 165;
    editor.is_playing = false;

    globals::project_directory = "/home/jackson/BaconEngine/projects/test2";
    globals::project_file = "/home/jackson/BaconEngine/projects/test2/game.json";
    file::load_project(editor.manager, false);

    debug_log("Engine startup successful.");

    for (int i = 0; i < 25; i++)
    {
        editor.console_log("This is a test log.");
        editor.console_warn("This is a test warning.");
        editor.console_error("This is a test error.");
    }

    while (!WindowShouldClose())
    {
        if (editor.is_playing)
        {
            // Do physics step
            editor.manager.simulation_step();
        }
        else
        {
            editor.editor_input();
        }

        BeginDrawing();
            ClearBackground(LIGHTGRAY);
            DrawText("TESTING", 10, 10, 14, BLACK);

            Camera2D* drawing_camera = nullptr;
            if (editor.is_playing) {
                drawing_camera = &editor.manager.get_active_camera()->camera;
            }
            else {
                drawing_camera = &editor.camera;
            }
            editor.manager.draw_entities(drawing_camera);

            editor.draw_ui();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
