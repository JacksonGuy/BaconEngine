#include <iostream>

#include "raylib.h"

#include "Editor.h"
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
    previousPosition = {0,0};

    // UI info
    showBoundingBox = false;

    // Add to GameManager
    GameManager::GameObjects.push_back(this);
    Rendering::AddToLayer(this);
}

GameObject::GameObject(const GameObject* obj) {
    type = OBJECT;

    if (!GameObject::unusedIDs.empty()) {
        ID = GameObject::unusedIDs.back();
        GameObject::unusedIDs.pop_back();
    }
    else {
        ID = GameObject::IDCount++;
    }
    
    name = obj->name;
    tag = obj->tag;

    position = obj->position;
    size = obj->size;
    rotation = obj->rotation;
    isVisible = obj->isVisible;
    layer = obj->layer;

    // We don't necessarily want new object to have the same
    // parent. This should be explicitly defined by the
    // user when creating a new object.
    parent = nullptr;
    previousPosition = {0,0};

    showBoundingBox = false;

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

    // Remove from RenderingLayer
    for (RenderingLayer& layer : Rendering::layers) {
        if (layer.layerNum == this->layer) {
            for (size_t i = 0; i < layer.objects.size(); i++) {
                if (layer.objects[i]->ID == ID) {
                    layer.objects.erase(layer.objects.begin() + i);
                    break;
                }
            }
            break;
        }
    }

    // Remove from ViewProperties
    if (Editor::viewPropertiesObject == this) {
        Editor::viewPropertiesObject = nullptr;
    }

    // Free up ID
    GameObject::unusedIDs.push_back(this->ID);
}

/**
 * @brief Swaps the objects rendering layer 
 * 
 * @param layer the new layer
 */
void GameObject::SetLayer(int layer) {
    u8 newLayer = 0;
    if (layer >= 0) {
        newLayer = layer;
    }
    Rendering::RemoveFromLayer(this);
    this->layer = newLayer;
    Rendering::AddToLayer(this);
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
    else{
        data["parent"] = -1;
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
            previousPosition = position;
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

/**
 * @brief Saves GameObject data to JSON prefab file
 * 
 * @param data the file to save to
 */
void GameObject::SaveGameObjectPrefab(nlohmann::json& data) {
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
    else{
        data["parent"] = -1;
    }
}

/**
 * @brief Loads data from JSON prefab file 
 * 
 * @param data the file to load from
 */
void GameObject::LoadGameObjectPrefab(nlohmann::json& data) {
    for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++) {
        std::string key = it.key();
        auto value = *it;

        if (key == "name") {
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
            previousPosition = position;
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
