#include "Entity.h"
#include "Rendering.h"
#include "GameManager.h"

#include <iostream>

Entity::Entity() : GameObject() {
    type = ENTITY;
    
    texture = {0};
    texturePath = "";

    bodytype = STATIC;
    grounded = false;
    velocity = {0,0};

    GameManager::Entities.push_back(this);
}

Entity::~Entity() {
    // Remove from Entities list
    for (size_t i = 0; i < GameManager::Entities.size(); i++) {
        if (GameManager::Entities[i]->ID == ID) {
            GameManager::Entities.erase(GameManager::Entities.begin() + i);
            break;
        }
    }

    UnloadTexture(texture);
}

void Entity::SetTexture(std::string path) {
    UnloadTexture(texture);

    Image image = LoadImage(path.c_str());
    
    // For debug purposes
    // Don't use for production, it's very unsafe
    // Image image = B_LoadImage(path); 

    ImageResize(&image, size.x, size.y);
    texture = LoadTextureFromImage(image);
    UnloadImage(image);
    texturePath = path;
}

void Entity::UpdateRect() {
    this->rect = {this->position.x, this->position.y, this->size.x, this->size.y};
}

void Entity::SaveEntityJson(nlohmann::json& data) {
    data = {
        {"id", ID},
        {"name", name}
    };
}