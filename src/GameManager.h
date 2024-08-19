#pragma once

#include <vector>

#include "box2d/box2d.h"

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

    // Object Lists
    extern std::vector<GameObject*> GameObjects;
    extern std::vector<Entity*> Entities;
    extern std::vector<TextObject*> TextObjects;
    extern std::vector<GameCamera*> GameCameras;

    // Game info
    extern Entity* player;
    extern GameCamera* current_camera;
    extern bool isPlayingGame;
    
    // Input

    // Physics
    extern b2WorldId world;
    extern f32 gravity;

    // Function
    void CreateBox2DWorld();
};
