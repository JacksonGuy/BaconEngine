#include "Entity.h"
#include "GameManager.h"

#include <iostream>

Entity::Entity() {
    this->position = {0.0f, 0.0f};
    this->scale = {1.0f, 1.0f};
    width = 256;
    height = 256;
    this->rect = sf::Rect<int>(this->position.x, this->position.y, this->width, this->height);
    this->rotation = 0.0f;

    GameManager::Entities.push_back(this);
}

Entity::Entity(sf::Vector2<float> position) {
    this->position = position;
    this->scale = {1.0f, 1.0f};
    width = 256;
    height = 256;
    this->rect = sf::Rect<int>(this->position.x, this->position.y, this->width, this->height);
    this->rotation = 0.0f;

    GameManager::Entities.push_back(this);
}

void Entity::SetSprite(std::string path) {
    sf::Texture* texture = GameManager::LoadTexture(path);
    this->scale.x = ((float)this->width) / texture->getSize().x;
    this->scale.y = ((float)this->height) / texture->getSize().y;
    this->sprite.setTexture(*texture);
    this->UpdateSpriteScale();
}

void Entity::UpdateSpriteScale() {
    this->sprite.setScale(this->scale.x, this->scale.y);
}

void Entity::UpdateRect() {
    this->rect = sf::Rect<int>(this->position.x, this->position.y, this->width, this->height);
}