#include "GameManager.hpp"

#include <iostream>

#include "Lua/luaApi.hpp"
#include "Rendering.hpp"

File::ConfigState GameManager::config;
std::vector<GameObject*> GameManager::GameObjects;
std::vector<Entity*> GameManager::Entities;
std::vector<TextObj*> GameManager::TextObjects;
sf::Font GameManager::font;
unsigned int GameManager::screenWidth = 1280;
unsigned int GameManager::screenHeight = 720;
Entity* GameManager::player = nullptr;
bool GameManager::isPlayingGame;
ImGuiTextBuffer GameManager::ConsoleLog;
unsigned int GameManager::framerateLimit = 500;
float GameManager::gravity = 0.2f;
sf::Keyboard::Key GameManager::lastKeyboardInput;
sf::Mouse::Button GameManager::lastMouseInput;
sf::Vector2f GameManager::mousePos = sf::Vector2f(0,0);
bool GameManager::windowHasFocus = true;
std::string GameManager::entryPoint = "";
sf::Clock GameManager::clock;

/**
 * @brief Check for collision between two specific Entities
 * 
 * @param e1 first entity
 * @param e2 second entity
 * @return boolean: if the entities are colliding  
 */
bool GameManager::checkCollision(const Entity& e1, const Entity& e2) {
    if (!e1.isSolid || !e2.isSolid) return false;
    return e1.rect.intersects(e2.rect);
}

/**
 * @brief Checks if the given Entity is colliding with any Entity in the game
 * 
 * @param e1 The Entity to check
 * @return boolean: if the entity is colliding with anything  
 */
bool GameManager::checkCollision(const Entity& e1) {
    if (!e1.isSolid) return false;
    for (Entity* e2 : GameManager::Entities) {
        if (!e2->isSolid) continue;
        if (e1.ID == e2->ID) continue;
        if (e1.rect.intersects(e2->rect)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Returns a std::vector of Entities that a given Entity is colliding with
 * 
 * @param e1 The Entity to check
 * @return std::vector<Entity*> the vector of Entities being colliding with
 */
std::vector<Entity*> GameManager::getCollidingWith(const Entity& e1) {
    std::vector<Entity*> collidingWith;
    
    if (!e1.isSolid) return collidingWith;

    for (Entity* e2 : GameManager::Entities) {
        if (!e2->isSolid) continue;
        if (e1.ID == e2->ID) continue;
        if (e1.rect.intersects(e2->rect)) {
            collidingWith.push_back(e2);
        }
    }
    return collidingWith;
}

/**
 * @brief Checks if a given side of an Entity is being collided with
 * 
 * @param side The rectangle side to check
 * @return boolean: if the side is being collided with 
 */
bool GameManager::checkCollisionSide(const sf::Rect<float> side) {
    for (Entity* e : GameManager::Entities) {
        if (e->ID == GameManager::player->ID) continue;
        if (!e->isSolid) continue;
        if (side.intersects(e->rect)) {
            return true;
        }
    } 
    return false;
}

/**
 * @brief Saves the game before testing.
 * 
 * @param window The game window
 * @param filename File to save game data to
 */
void GameManager::SaveEditorState(sf::RenderWindow& window, std::string filename) {
    File::save(filename);
}

/**
 * @brief Restore the saved game data
 * 
 * @param window The game window
 * @param filename File to retrieve save data from
 */
void GameManager::RestoreEditorState(sf::RenderWindow& window, std::string filename) {
    // Delete Editor stuff
    for (Entity* e : GameManager::Entities) {
        free(e);
    }
    for (TextObj* text : GameManager::TextObjects) {
        free(text);
    }
    GameManager::GameObjects.clear();
    GameManager::Entities.clear();
    GameManager::TextObjects.clear();
    Rendering::m_layers.clear();

    // Load Back data
    File::load(filename);
}

/**
 * @brief Write a string to the Editor Console
 * 
 * @param text The string to write
 */
void GameManager::ConsoleWrite(std::string text) {
    std::string newText = text + "\n";
    GameManager::ConsoleLog.append(newText.c_str());
}

/**
 * @brief Finds a GameObject with the given ID. Uses a binary search algorithm.
 * 
 * @param id The ID to search for
 * @return GameObject* The GameObject with the ID
 */
GameObject* GameManager::FindObjectByID(int id) {
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
Entity* GameManager::FindEntityByID(int id) {
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
Entity* GameManager::FindEntityByName(std::string name) {
    for (Entity* e : GameManager::Entities) {
        if (e->name == name) {
            return e;
        }
    }
    return nullptr;
}

/**
 * @brief Returns a std::vector of GameObject with the given tag (string)
 * 
 * @param type The GameObject tag
 * @return std::vector<Entity*> The vector of GameObjects with the given tag 
 */
std::vector<GameObject*> GameManager::FindObjectsByTag(std::string tag) {
    std::vector<GameObject*> found;
    for (GameObject* obj : GameManager::GameObjects) {
        if (obj->tag == tag) {
            found.push_back(obj);
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
TextObj* GameManager::FindTextByID(int id) {
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
void GameManager::SortObjectsByID() {
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