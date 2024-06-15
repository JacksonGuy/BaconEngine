#include "Entity.hpp"
#include "GameManager.hpp"

#include <iostream>

unsigned int Entity::IDNum = 0;

Entity::Entity(sf::Vector2f position) {
    this->ID = Entity::IDNum;
    Entity::IDNum++;
    this->name = "Entity " + std::to_string(this->ID);
    this->texturePath = "";

    this->position = position;
    this->scale = {1.0f, 1.0f};
    width = 256;
    height = 256;
    this->rotation = 0.0f;

    this->showDetailMenu = false;
    this->showHitbox = false;
    this->isPlayer = false;
    this->isSolid = false;
    this->physicsObject = false;
    this->hitboxSize = 2.0f;

    this->mass = 1.f;
    this->speed = 0.f;

    GameManager::Entities.push_back(this);
}

Entity::Entity(Entity& e) {
    this->ID = e.ID;
    this->name = e.name;
    this->position = e.position;
    this->scale = e.scale;
    this->width = e.width;
    this->height = e.height;
    this->rotation = e.rotation;

    this->showDetailMenu = e.showDetailMenu;
    this->showHitbox = e.showHitbox;
    this->isPlayer = e.isPlayer;
    this->hitboxSize = e.hitboxSize;
    
    this->mass = e.mass;
    this->speed = e.speed;

    this->SetSprite(e.texturePath, false);
    this->SetPosition(this->position);
}

void Entity::SetSprite(std::string path, bool autoScale) {
    this->texturePath = path;
    sf::Texture* texture = GameManager::LoadTexture(path);
    if (autoScale) {
        this->scale.x = ((float)this->width) / texture->getSize().x;
        this->scale.y = ((float)this->height) / texture->getSize().y;
    }
    
    this->sprite.setTexture(*texture, true);
    this->sprite.setPosition(this->position);
    this->sprite.setOrigin((sf::Vector2f)texture->getSize()/2.0f);
    this->sprite.setScale(this->scale);
    this->UpdateRect();
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
    this->rect = this->sprite.getGlobalBounds();
    this->UpdateCollisionRects();
}

void Entity::UpdateCollisionRects() {
    sf::Vector2f rectPos = this->rect.getPosition();

    this->topRect = sf::Rect<float>(
        sf::Vector2f(rectPos.x, rectPos.y - this->hitboxSize),
        sf::Vector2f(this->rect.width, this->hitboxSize)
    );

    this->bottomRect = sf::Rect<float>(
        sf::Vector2f(rectPos.x, rectPos.y + this->rect.height),
        sf::Vector2f(this->rect.width, this->hitboxSize)
    );

    this->leftRect = sf::Rect<float>(
        sf::Vector2f(rectPos.x - this->hitboxSize, rectPos.y),
        sf::Vector2f(this->hitboxSize, this->rect.height)
    );

    this->rightRect = sf::Rect<float>(
        sf::Vector2f(rectPos.x + this->rect.width, rectPos.y),
        sf::Vector2f(this->hitboxSize, this->rect.height)
    );
}