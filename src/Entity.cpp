#include "Entity.hpp"
#include "GameManager.hpp"

#include <iostream>

unsigned int Entity::IDNum = 0;

Entity::Entity(sf::Vector2f position = {0.0f, 0.0f}) {
    this->ID = Entity::IDNum;
    Entity::IDNum++;
    this->name = "Entity " + std::to_string(this->ID);
    this->texturePath = "";

    this->position = position;
    this->scale = {1.0f, 1.0f};
    width = 256;
    height = 256;
    this->rect = sf::Rect<int>(this->position.x, this->position.y, this->width, this->height);
    this->rotation = 0.0f;

    this->showDetailMenu = false;

    GameManager::Entities.push_back(this);

    std::cout << "[DEBUG] Entities count: " << GameManager::Entities.size() << std::endl;
}

void Entity::SetSprite(std::string path, bool autoScale) {
    this->texturePath = path;
    sf::Texture* texture = GameManager::LoadTexture(path);
    if (autoScale) {
        this->scale.x = ((float)this->width) / texture->getSize().x;
        this->scale.y = ((float)this->height) / texture->getSize().y;
    }
    
    this->sprite.setScale(this->scale);
    this->sprite.setTexture(*texture, true);
    this->sprite.setPosition(this->position);
}

void Entity::SetPosition(sf::Vector2f position) {
    this->position = position;
    this->UpdateRect();
    this->sprite.setPosition(this->position);
}

void Entity::SetSpriteScale(sf::Vector2f scale) {
    this->scale = scale;
    this->sprite.setScale(this->scale);
}

void Entity::UpdateRect() {
    this->rect = sf::Rect<int>(this->position.x, this->position.y, this->width, this->height);
}