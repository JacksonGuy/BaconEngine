#include <iostream>

#include "raylib.h"

#include "GameObject.h"
#include "GameManager.h"
#include "Rendering.h"

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

/**
 * @brief Saves the GameObject's data to a JSON object
 * 
 * @param data the JSON object to save to
 */
void GameObject::SaveGameObjectJson(nlohmann::json& data) {
    data["ID"] = ID;
    data["name"] = name;
    data["tag"] = tag;
    data["type"] = type;

    data["position"] = {position.x, position.y};
    data["size"] = {size.x, size.y};
    data["rotation"] = rotation;
    data["isVisible"] = isVisible;
    data["layer"] = layer;

    if (parent != nullptr) {
        data["parent"] = parent->ID;
    }

    for (size_t i = 0; i < children.size(); i++) {
        data["children"][i] = children[i]->ID;
    }
}

/**
 * @brief Loads GameObject data from JSON object
 * 
 * @param data the JSON object to load from 
 */
void GameObject::LoadGameObjectJson(nlohmann::json& data) {
    for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++) {
        std::string key = it.key();
        auto value = *it;

        // This whole segment looks ridiculous, but I promise it
        // is for a good reason (backwards compatability).  

        // Meta information
        if (key == "ID") {
            ID = value;
        }
        else if (key == "name") {
            name = value;
        }
        else if (key == "tag") {
            tag = value;
        }
        else if (key == "type") {
            type = ClassType(value);
        }

        // Physical information
        else if (key == "position") {
            position = {value[0], value[1]};
        }
        else if (key == "size") {
            size = {value[0], value[1]};
        }
        else if (key == "rotation") {
            rotation = value;
        }
        else if (key == "isVisible") {
            isVisible = value;
        }
        else if (key == "layer") {
            layer = value;
        }
    }
}