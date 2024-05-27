#include "Entity.h"
#include "GameManager.h"

Entity::Entity() {
    this->position = {0.0f, 0.0f};
    this->scale = {1.0f, 1.0f};
    this->rotation = 0.0f;

    //GameManager::Entities.push_back(*this);
}

Entity::Entity(sf::Vector2<float> position) {
    this->position = position;
    this->scale = {1.0f, 1.0f};
    this->rotation = 0.0f;

    //GameManager::Entities.push_back(*this);
}

void Entity::SetSprite(std::string path) {
    sf::Texture texture = GameManager::LoadTexture(path);
    this->sprite.setTexture(texture);
}