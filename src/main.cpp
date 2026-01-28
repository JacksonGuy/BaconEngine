#include <iostream>

#include "imgui.h"
#include "rlImGui.h"
#include "raylib.h"

#include "core/util.h"

int main(int argc, char** argv) {
    using namespace bacon;
    debug_log("Starting BaconEngine...");

    bool test_menu = true;

    InitWindow(800, 600, "Test");
    SetTargetFPS(512);
    rlImGuiSetup(true);
    debug_log("Engine startup successful.");

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(LIGHTGRAY);
            DrawText("TESTING", 10, 10, 14, BLACK);

            rlImGuiBegin();
                ImGui::Begin("Test Menu", &test_menu, ImGuiWindowFlags_MenuBar);
                    if (ImGui::BeginMenuBar())
                    {
                        if (ImGui::BeginMenu("File"))
                        {
                            ImGui::EndMenu();
                        }
                        ImGui::EndMenuBar();
                    }
                ImGui::End();
            rlImGuiEnd();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
