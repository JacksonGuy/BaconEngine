#include <ctime>

#include "GameManager.h"
#include "Rendering.h"

namespace GameManager {
    // Default window settings
    u16 screenWidth = 800;
    u16 screenHeight = 600;
    u16 framerateLimit = 60;

    // Engine
    std::string engineVersion = "0.0";
    Font defaultFont = {0};
    GameCamera* current_camera = nullptr;

    // Object Lists
    std::vector<GameObject*> GameObjects;
    std::vector<Entity*> Entities;
    std::vector<TextObject*> TextObjects;
    std::vector<GameCamera*> GameCameras;

    // Game info 
    Entity* player = nullptr;
    bool isPlayingGame = false;
    std::string projectEntryPath = "";
    GameCamera* activeCameraTracker = nullptr;

    // Input
    bool windowHasFocus = false;
    KeyboardKey lastKeyboardInput = KEY_NULL;
    MouseButton lastMouseInput = MOUSE_BUTTON_LEFT;
    Vector2 lastMousePosition = {0,0};
    std::unordered_map<KeyboardKey, bool> keypresses;

    // Lua
    sol::state lua;

    // Console
    ImGuiTextBuffer ConsoleBuffer;
    std::vector<std::string> ConsoleMessages;

    // Physics
    f32 gravity = 10.f;

    /**
     * @brief Writes a standard message to the console window
     * 
     * @param message the message to write
     */
    void ConsoleMessage(std::string message) {
        time_t now = time(0);
        char* current_time = ctime(&now);
        std::string time_str = std::string(current_time);
        time_str.pop_back();
        std::string text = "[ENGINE] (" + time_str + "): " + message + "\n";
        ConsoleMessages.push_back(text); 
    }

    /**
     * @brief Writes an error message to the console window
     * 
     * @param message the error message to write
     */
    void ConsoleError(std::string message) {
        time_t now = time(0);
        char* current_time = ctime(&now);
        std::string time_str = std::string(current_time);
        time_str.pop_back();
        std::string text = "[ERROR] (" + time_str + "): " + message + "\n";
        ConsoleMessages.push_back(text);
    }

    /**
     * @brief Searches for a GameObject with the given ID. Binary search.
     * 
     * @param ID The ID of the GameObject to search for
     * @return GameObject* The GameObject, if found. Otherwise nullptr.
     */
    GameObject* FindObjectByID(u32 ID) {
        if (GameManager::GameObjects.empty()) return nullptr;

        int low = 0;
        int high = GameManager::GameObjects.size() - 1; 

        while (low <= high) {
            int mid = low + (high - low) / 2;

            // We found the game object
            if (GameManager::GameObjects[mid]->ID == ID) {
                return GameManager::GameObjects[mid];
            }

            // Shift search window
            if (GameManager::GameObjects[mid]->ID < ID) {
                low = mid + 1;
            }
            else {
                high = mid - 1; 
            }
        }

        return nullptr;
    }

    /**
     * @brief Checks if  
     * 
     * @param p1 
     * @param p2 
     * @return true 
     * @return false 
     */
    bool PointIntersects(const GameObject& obj, Vector2 point) {
        if (point.x > obj.position.x && point.x < obj.position.x + obj.size.x &&
            point.y > obj.position.y && point.y < obj.position.y + obj.size.y) {
                return true;
        }
        return false;
    }

    /**
     * @brief Resets the variables of the GameManager to their default values
     * 
     */
    void Reset() {
        // Delete all objects
        for (GameObject* obj : GameManager::GameObjects) {
            free(obj);
        }

        // Reset object vectors and ID count
        GameObjects.clear();
        GameObject::IDCount = 0;
        Entities.clear();
        TextObjects.clear();
        GameCameras.clear();

        // Clean Rendering layers
        for (RenderingLayer& layer : Rendering::layers) {
            layer.objects.clear();
        }

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