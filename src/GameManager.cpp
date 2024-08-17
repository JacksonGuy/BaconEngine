#include "GameManager.hpp"

#include <iostream>

#include "Lua/LuaApi.hpp"
#include "Rendering.hpp"

namespace GameManager {
    File::ConfigState config;
    
    std::vector<GameObject*> GameObjects;
    std::vector<Entity*> Entities;
    std::vector<TextObj*> TextObjects;
    std::vector<Camera*> Cameras;
    
    sf::Font font;
    unsigned int screenWidth = 1280;
    unsigned int screenHeight = 720;
    
    Entity* player = nullptr;
    bool isPlayingGame = false;
    sf::Clock clock;
    Camera* camera = nullptr;
    
    ImGuiTextBuffer ConsoleLog;
    std::vector<std::string> ConsoleMessages;
    
    unsigned int framerateLimit = 500;
    sf::Keyboard::Key lastKeyboardInput;
    sf::Mouse::Button lastMouseInput;
    std::unordered_map<sf::Keyboard::Key, bool> keypresses;
    sf::Vector2f mousePos = sf::Vector2f(0,0);
    bool windowHasFocus = true;
    
    std::string entryPoint = "";

    b2Vec2 gravity(0.0f, 10.0f);
    b2World* world = new b2World(GameManager::gravity);

    /**
     * @brief Saves the game before testing.
     * 
     * @param window The game window
     * @param filename File to save game data to
     */
    void SaveEditorState(sf::RenderWindow& window, std::string filename) {
        File::save(filename);
    }

    /**
     * @brief Restore the saved game data
     * 
     * @param window The game window
     * @param filename File to retrieve save data from
     */
    void RestoreEditorState(sf::RenderWindow& window, std::string filename) {
        // Delete Editor stuff
        for (Entity* e : GameManager::Entities) {
            //delete(e);

            // TODO
            // For some reason, calling delete causes the program to segfault
            // We don't strictly need to here, since we just clear everything anyways,
            // but this could be a massive issue in the future
            free(e);
        }
        for (TextObj* text : GameManager::TextObjects) {
            delete(text);
        }
        for (Camera* camera : GameManager::Cameras) {
            delete(camera);
        }
        GameManager::GameObjects.clear();
        GameManager::Entities.clear();
        GameManager::TextObjects.clear();
        GameManager::Cameras.clear();
        Rendering::m_layers.clear();

        // Load Back data
        File::load(filename);
    }

    /**
     * @brief Write a string to the Editor Console
     * 
     * @param text The string to write
     */
    void ConsoleWrite(std::string text) {
        std::string newText = text + "\n";
        // GameManager::ConsoleLog.append(newText.c_str());
        GameManager::ConsoleMessages.push_back(newText);
    }

    /**
     * @brief Finds a GameObject with the given ID. Uses a binary search algorithm.
     * 
     * @param id The ID to search for
     * @return GameObject* The GameObject with the ID
     */
    GameObject* FindObjectByID(int id) {
        int low = 0;
        int high = GameManager::GameObjects.size();
        while (low <= high) {
            int mid = low + (high - low) / 2;

            if (GameManager::GameObjects[mid]->ID == id) {
                return GameManager::GameObjects[mid];
            }

            if (GameManager::GameObjects[mid]->ID < id) {
                low = mid + 1;
            }
            else {
                high = mid - 1;
            }
        }

        return nullptr;
    }

    /**
     * @brief Finds an Entity with the given ID. Uses a binary search algorithm.
     * 
     * @param id the ID to search for
     * @return Entity* The Entity with the ID
     */
    Entity* FindEntityByID(int id) {
        int low = 0;
        int high = GameManager::Entities.size();
        while (low <= high) {
            int mid = low + (high - low) / 2;

            if (GameManager::Entities[mid]->ID == id) {
                return GameManager::Entities[mid];
            }

            if (GameManager::Entities[mid]->ID < id) {
                low = mid + 1;
            }
            else {
                high = mid - 1;
            }
        }

        return nullptr;
    }

    /**
     * @brief Find an Entity with the given name (string). Linear search.
     * 
     * @param name The name to search for
     * @return Entity* The Entity with the given name
     */
    Entity* FindEntityByName(std::string name) {
        for (Entity* e : GameManager::Entities) {
            if (e->name == name) {
                return e;
            }
        }
        return nullptr;
    }

    /**
     * @brief Returns a std::vector of Entities with the given tag (string)
     * 
     * @param type The Entity tag
     * @return std::vector<Entity*> The vector of Entities with the given tag 
     */
    std::vector<Entity*> FindEntitiesByTag(std::string tag) {
        std::vector<Entity*> found;
        for (Entity* e : GameManager::Entities) {
            if (e->tag == tag) {
                found.push_back(e);
            }
        }
        return found;
    }

    /**
     * @brief Find an TextObj with the given ID. Binary search algorithm.
     * 
     * @param id The ID to search for
     * @return TextObj* The TextObj with the ID
     */
    TextObj* FindTextByID(int id) {
        int low = 0;
        int high = GameManager::TextObjects.size();
        while (low <= high) {
            int mid = low + (high - low) / 2;

            if (GameManager::TextObjects[mid]->ID == id) {
                return GameManager::TextObjects[mid];
            }

            if (GameManager::TextObjects[mid]->ID < id) {
                low = mid + 1;
            }
            else {
                high = mid - 1;
            }
        }

        return nullptr;
    }

    /**
     * @brief Sorts GameObjects by ID. Insertion sort.
     */
    void SortObjectsByID() {
        GameObject* key;
        int j;
        for (size_t i = 1; i < GameManager::GameObjects.size(); i++) {
            key = GameManager::GameObjects[i];
            j = i - 1;

            while (j >= 0 && GameManager::GameObjects[j]->ID > key->ID) {
                GameManager::GameObjects[j+1] = GameManager::GameObjects[j];
                j--; 
            }
            GameManager::GameObjects[j+1] = key;
        }
    }
}