#include "GameManager.hpp"
#include "Lua/luaApi.hpp"
#include <iostream>

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

bool GameManager::MouseOnEntity(sf::Vector2f mousePos) {
    for (Entity* e : GameManager::Entities) {
        if (e->rect.contains(mousePos.x, mousePos.y)) {
            return true;
        }
    }
    return false;
}

// Check for collision between two specific Entities
bool GameManager::checkCollision(const Entity& e1, const Entity& e2) {
    if (!e1.isSolid || !e2.isSolid) return false;
    return e1.rect.intersects(e2.rect);
}

// Check if an object is colliding with any entity
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

// Get a list of entities that the object is colliding with
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

void GameManager::SaveEditorState(sf::RenderWindow& window, std::string filename) {
    File::save(filename);
}

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

    // Load Back data
    File::load(filename);
}

void GameManager::ConsoleWrite(std::string text) {
    std::string newText = text + "\n";
    GameManager::ConsoleLog.append(newText.c_str());
}

// Binary Search Tree
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

Entity* GameManager::FindEntityByName(std::string name) {
    for (Entity* e : GameManager::Entities) {
        if (e->name == name) {
            return e;
        }
    }
    return nullptr;
}

std::vector<Entity*> GameManager::FindEntitiesByType(std::string type) {
    std::vector<Entity*> found;
    for (Entity* e : GameManager::Entities) {
        if (e->entity_type == type) {
            found.push_back(e);
        }
    }
    return found;
}

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

// Insertion Sort
void GameManager::SortObjectsByID() {
    GameObject* key;
    size_t j;
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