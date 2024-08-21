#include "GameObject.h"
#include "GameManager.h"
#include "Rendering.h"

#include "raylib.h"

u32 GameObject::IDCount = 0;
std::vector<u32> GameObject::unusedIDs;

GameObject::GameObject() {
    // Meta Information
    type = OBJECT;
    if (!GameObject::unusedIDs.empty()) {
        ID = GameObject::unusedIDs.back();
        GameObject::unusedIDs.pop_back();
    }
    else {
        ID = GameObject::IDCount++;
    }
    name = "Object";
    tag = "Default";

    // Physical Information
    position = {0, 0};
    size = {0, 0};
    rotation = 0.f;
    isVisible = true;
    layer = 0;

    // Tree data
    parent = nullptr;

    // UI Info
    showDetails = false;

    // Add to GameManager
    GameManager::GameObjects.push_back(this);
    Rendering::AddToLayer(this);
}

GameObject::~GameObject() {
    // Remove from GameManager
    for (size_t i = 0; i < GameManager::GameObjects.size(); i++) {
        if (GameManager::GameObjects[i]->ID == ID) {
            GameManager::GameObjects.erase(GameManager::GameObjects.begin() + i);
            break;
        }
    }

    // Free up ID
    GameObject::unusedIDs.push_back(this->ID);
}
