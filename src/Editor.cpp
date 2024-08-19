#include <iostream>

#include "raylib.h"
#include "box2d/box2d.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "BaconEngine V2");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(DARKGRAY);
            DrawText("Bacon", 350, 280, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
