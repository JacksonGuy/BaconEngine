#include "Entity.h"
#include "GameManager.h"

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

    GameManager::Entities.push_back(this);

    std::cout << "[DEBUG] Entities count: " << GameManager::Entities.size() << std::endl;
}

void Entity::SetSprite(std::string path) {
    this->texturePath = path;
    sf::Texture* texture = GameManager::LoadTexture(path);
    this->scale.x = ((float)this->width) / texture->getSize().x;
    this->scale.y = ((float)this->height) / texture->getSize().y;
    this->sprite.setTexture(*texture);
    this->sprite.setScale({this->scale.x, this->scale.y});
    this->sprite.setPosition(this->position);
}

void Entity::SetPosition(sf::Vector2f position) {
    this->position = position;
    this->UpdateRect();
    this->sprite.setPosition(this->position);
}

void Entity::SetSpriteScale(sf::Vector2f scale) {
    this->scale = scale;
    this->sprite.setScale(scale.x, scale.y);
}

void Entity::UpdateRect() {
    this->rect = sf::Rect<int>(this->position.x, this->position.y, this->width, this->height);
}