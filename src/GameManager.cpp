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

    // Physics
    b2WorldId world;
    f32 gravity = -10.f;
    
    /**
     * @brief Create Box2D world object
     */
    void CreateBox2DWorld() {
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = (b2Vec2){0.0f, gravity};
        GameManager::world = b2CreateWorld(&worldDef);
    }
};
