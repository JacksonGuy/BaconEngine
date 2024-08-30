#pragma once

#include <string> 

#include "util.h"
#include "Camera.h"

namespace Editor {
    // Project Details
    extern std::string projectTitle;
    extern bool loadedProject;

    // Physics
    extern f32 lastFixedUpdate;
    extern f32 fixedUpdateRate;

    // Scene Window
    extern bool sceneWindowFocused;
    extern Vector2 sceneWindowPosition;
    extern Vector2 sceneWindowSize;
    extern bool sceneWindowMouseCapture; 

    // Editor Camera
    extern GameCamera* camera;
}