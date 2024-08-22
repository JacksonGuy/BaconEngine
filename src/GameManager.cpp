#include "GameManager.h"

namespace GameManager {
    // Default window settings
    u16 screenWidth = 800;
    u16 screenHeight = 600;
    u16 framerateLimit = 60;

    // Object Lists
    std::vector<GameObject*> GameObjects;
    std::vector<Entity*> Entities;
    std::vector<TextObject*> TextObjects;
    std::vector<GameCamera*> GameCameras;

    // Game info 
    Entity* player = nullptr;
    GameCamera* current_camera = nullptr;
    bool isPlayingGame = false;

    // Input
    bool windowHasFocus = false;
    KeyboardKey lastKeyboardInput = KEY_NULL;
    MouseButton lastMouseInput = MOUSE_BUTTON_LEFT;
    Vector2 lastMousePosition = {0,0};
    std::unordered_map<KeyboardKey, bool> keypresses;

    // Physics
    b2WorldId world;
    f32 gravity = 10.f;
    
    /**
     * @brief Create Box2D world object
     */
    void CreateBox2DWorld() {
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = {0.0f, gravity};
        GameManager::world = b2CreateWorld(&worldDef);
    }
};
