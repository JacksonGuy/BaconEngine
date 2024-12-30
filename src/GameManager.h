#pragma once

#include <vector>
#include <unordered_map>

#include "imgui.h"
#include <sol/sol.hpp>

#include "util.h"
#include "GameObject.h"
#include "Entity.h"
#include "TextObject.h"
#include "Camera.h"

namespace GameManager {
    // Window Information
    extern u16 screenWidth;
    extern u16 screenHeight;
    extern u16 framerateLimit;

    // Engine
    extern std::string engineVersion;
    extern Font defaultFont;
    extern GameCamera* current_camera;

    // Object Lists
    extern std::vector<GameObject*> GameObjects;
    extern std::vector<Entity*> Entities;
    extern std::vector<TextObject*> TextObjects;
    extern std::vector<GameCamera*> GameCameras;

    // Game info
    extern Entity* player;
    extern GameCamera* activeCameraTracker;
    extern bool isPlayingGame;
    extern std::string projectEntryPath;
    
    // Input
    extern bool windowHasFocus;
    extern KeyboardKey lastKeyboardInput;
    extern MouseButton lastMouseInput;
    extern Vector2 lastMousePosition;
    extern std::unordered_map<KeyboardKey, bool> keypresses;

    // Lua
    extern sol::state lua;

    // Console
    extern ImGuiTextBuffer ConsoleBuffer;
    extern std::vector<std::string> ConsoleMessages;

    // Physics
    extern f32 gravity;

    // Functions
    void ConsoleMessage(std::string message);
    void ConsoleError(std::string message);

    GameObject* FindObjectByID(u32 id);
    bool PointIntersects(const GameObject& p1, Vector2 p2);

    void Reset();
};
