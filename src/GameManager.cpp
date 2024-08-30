#include "GameManager.h"

#include <ctime>

namespace GameManager {
    // Default window settings
    u16 screenWidth = 800;
    u16 screenHeight = 600;
    u16 framerateLimit = 60;

    // Engine Info
    std::string engineVersion = "0.0";

    // Object Lists
    std::vector<GameObject*> GameObjects;
    std::vector<Entity*> Entities;
    std::vector<TextObject*> TextObjects;
    std::vector<GameCamera*> GameCameras;

    // Game info 
    Entity* player = nullptr;
    GameCamera* current_camera = nullptr;
    bool isPlayingGame = false;
    std::string projectEntryPath = "";

    // Input
    bool windowHasFocus = false;
    KeyboardKey lastKeyboardInput = KEY_NULL;
    MouseButton lastMouseInput = MOUSE_BUTTON_LEFT;
    Vector2 lastMousePosition = {0,0};
    std::unordered_map<KeyboardKey, bool> keypresses;

    // Console
    ImGuiTextBuffer ConsoleBuffer;
    std::vector<std::string> ConsoleMessages;

    // Physics
    f32 gravity = 10.f;

    // Functions
    void ConsoleMessage(std::string message) {
        time_t now = time(0);
        char* time = ctime(&now);
        std::string text = "[ENGINE] (" + std::string(time) + "): " + message + "\n";
        ConsoleMessages.push_back(text); 
        free(time);
    }

    void ConsoleError(std::string message) {
        time_t now = time(0);
        char* time = ctime(&now);
        std::string text = "[ERROR] (" + std::string(time) + "): " + message + "\n";
        ConsoleMessages.push_back(text); 
        free(time);
    }

    /**
     * @brief Resets the variables of the GameManager to their default values
     * 
     */
    void Reset() {
        // Clean out vectors
        for (Entity* e : GameManager::Entities) {
            delete e;
        }
        for (TextObject* text : GameManager::TextObjects) {
            delete text;
        }
        for (GameCamera* camera : GameManager::GameCameras) {
            delete camera;
        }
        GameObjects.clear();
        GameObject::IDCount = 0;
        Entities.clear();
        TextObjects.clear();
        GameCameras.clear();
        ConsoleMessages.clear();

        // Reset game info
        player = nullptr;
        current_camera = nullptr;
        isPlayingGame = false;
        projectEntryPath = "";

        // Reset input
        bool windowHasFocus = false;
        KeyboardKey lastKeyboardInput = KEY_NULL;
        MouseButton lastMouseInput = MOUSE_BUTTON_LEFT;
        Vector2 lastMousePosition = {0,0};
        std::unordered_map<KeyboardKey, bool> keypresses;
    }
};
