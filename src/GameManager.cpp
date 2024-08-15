#include "GameManager.hpp"

#include <iostream>

#include "Lua/LuaApi.hpp"
#include "Rendering.hpp"
#include "BaconMath.h"

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
Camera* GameManager::camera;
std::vector<Camera*> GameManager::Cameras;
std::unordered_map<sf::Keyboard::Key, bool> GameManager::keypresses;

/**
 * @brief Check if two lines intersect each other
 * 
 * @param s1 start point for line 1
 * @param e1 end point for line 1
 * @param s2 start point for line 2
 * @param e2 end point for line 2
 * @return true if they intersect 
 * @return false if they don't
 */
bool GameManager::CheckCollisionLines(sf::Vector2f s1, sf::Vector2f e1, 
    sf::Vector2f s2, sf::Vector2f e2) {
    
    int o1 = bmath::orientation(s1, e1, s2);
    int o2 = bmath::orientation(s1, e1, e2);
    int o3 = bmath::orientation(s2, e2, s1);
    int o4 = bmath::orientation(s2, e2, e1);

    if (o1 != o2 && o3 != o4) return true;

    if (o1 == 0 && bmath::onSegment(s1, s2, e1)) return true;
    if (o2 == 0 && bmath::onSegment(s1, e2, e1)) return true;
    if (o3 == 0 && bmath::onSegment(s2, s1, e2)) return true;
    if (o4 == 0 && bmath::onSegment(s2, e1, e2)) return true;

    return false;
}

/**
 * @brief Checks if a point lies on a line segment
 * 
 * @param point the point to check
 * @param s line start point
 * @param e line end point
 * @return true if point lies on the line se
 * @return false if it doesn't
 */
bool GameManager::CheckCollisionPointLine(sf::Vector2f point, 
        sf::Vector2f s, sf::Vector2f e) {
    float crossproduct = (point.y - s.y) * (e.x - s.x) - (point.x - s.x) * (e.y - s.y);

    float epsilon = 0.1f; // Error tolerance
    if (bmath::abs(crossproduct) > epsilon) return false;

    float dotproduct = (point.x - s.x) * (e.x - s.x) + (point.y - s.y) * (e.y - s.y);
    if (dotproduct < 0) return false;

    float squaredlength = (e.x - s.x) * (e.x - s.x) + (e.y - s.y) * (e.y - s.y);
    if (dotproduct > squaredlength) return false;

    return true;
}

bool GameManager::CheckCollisionPointRect(sf::Vector2f point, sf::FloatRect rect) {
    if (point.x >= rect.left && point.x <= rect.left + rect.width &&
            point.y >= rect.top && point.y <= rect.top + rect.height) { 
        return true;
    }
    return false;
}

/**
 * @brief Check for collision between two specific Entities
 * 
 * @param e1 first entity
 * @param e2 second entity
 * @return boolean: if the entities are colliding  
 */
bool GameManager::checkCollision(const Entity& e1, const Entity& e2) {
    if (!e1.isSolid || !e2.isSolid) return false;
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (CheckCollisionLines(e1.hitbox[i], e1.hitbox[i+1], 
                e2.hitbox[j], e2.hitbox[j+1])) {
                    return true;
            }
        }
    }
    return false;
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
        if (checkCollision(e1, *e2)) {
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
        if (checkCollision(e1, *e2)) {
            collidingWith.push_back(e2);
        }
    }
    return collidingWith;
}

/**
 * @brief Checks if a given side of an Entity is being collided with by any other Entity
 * 
 * @param e The Entity to check
 * @param side (string) The rectangle side to check (TOP/BOTTOM/LEFT/RIGHT)
 * @return boolean: if the side is being collided with 
 */
bool GameManager::checkCollisionSide(const Entity& e, const std::string side) {
    if (!e.isSolid) return false;

    bool result = false;
    for (Entity* e2 : Entities) {
        if (e.ID == e2->ID) continue;
        if (!e2->isSolid) continue;
    
        if (side == "TOP") {
            float width = e.hitbox[1].x - e.hitbox[0].x;

            sf::Vector2f point;
            point.x = e.hitbox[0].x + width/2;
            point.y = e.hitbox[0].y - 1;
            
            sf::FloatRect rect(
                e2->position.x - e2->width/2,
                e2->position.y - e2->height/2, 
                e2->width,
                e2->height
            );

            if (CheckCollisionPointRect(point, rect)) {
                result = true;
                break;
            }
        }
        else if (side == "BOTTOM") {
            float width = e.hitbox[2].x - e.hitbox[3].x;

            sf::Vector2f point;
            point.x = e.hitbox[3].x + width/2;
            point.y = e.hitbox[3].y + 1;

            sf::FloatRect rect(
                e2->position.x - e2->width/2,
                e2->position.y - e2->height/2, 
                e2->width,
                e2->height
            );
 
            if (CheckCollisionPointRect(point, rect)) {
                result = true;
                break;
            }
        }
        else if (side == "LEFT") {
            float height = e.hitbox[3].y - e.hitbox[0].y;
            
            sf::Vector2f point;
            point.x = e.hitbox[0].x - 1;
            point.y = e.hitbox[0].y + height/2;

            sf::FloatRect rect(
                e2->position.x - e2->width/2,
                e2->position.y - e2->height/2, 
                e2->width,
                e2->height
            );

            if (CheckCollisionPointRect(point, rect)) {
                result = true;
                break;
            }
        }
        else if (side == "RIGHT") {
            float height = e.hitbox[3].y - e.hitbox[0].y;
            float width = e.hitbox[1].x - e.hitbox[0].x;

            sf::Vector2f point;
            point.x = e.hitbox[0].x + width + 1;
            point.y = e.hitbox[0].y + height/2;

            sf::FloatRect rect(
                e2->position.x - e2->width/2,
                e2->position.y - e2->height/2, 
                e2->width,
                e2->height
            );

            if (CheckCollisionPointRect(point, rect)) {
                result = true;
                break;
            }
        }
    } 
    return result;
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
 * @brief Returns a std::vector of Entities with the given tag (string)
 * 
 * @param type The Entity tag
 * @return std::vector<Entity*> The vector of Entities with the given tag 
 */
std::vector<Entity*> GameManager::FindEntitiesByTag(std::string tag) {
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