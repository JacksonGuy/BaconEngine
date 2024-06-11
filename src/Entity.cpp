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
    this->isPlayer = false;
    this->isSolid = false;
    this->physicsObject = false;
    this->collisionBorder = 2.0f;

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

    this->showDetailMenu = false;
    this->isPlayer = e.isPlayer;
    this->collisionBorder = 2.0f; // DEBUG

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
    // We want our collision rect (hitbox) to be slightly larger than the boundary of our sprite
    // This way we are colliding before our sprite clips through the intersecting Entity  
    // sf::Rect<float> basic = this->sprite.getGlobalBounds();
    // sf::Vector2f newPos = sf::Vector2f(basic.getPosition().x - this->collisionBorder, basic.getPosition().y - this->collisionBorder);
    // sf::Vector2f newSize = sf::Vector2f(basic.getSize().x + this->collisionBorder, basic.getSize().y + this->collisionBorder);
    // 
    // this->rect = sf::Rect<float>(newPos, newSize);
    
    this->rect = this->sprite.getGlobalBounds();
    this->UpdateCollisionRects();
}

void Entity::UpdateCollisionRects() {
    sf::Vector2f rectPos = this->rect.getPosition();

    this->topRect = sf::Rect<float>(
        sf::Vector2f(rectPos.x, rectPos.y - this->collisionBorder),
        sf::Vector2f(this->rect.width, this->collisionBorder)
    );

    this->bottomRect = sf::Rect<float>(
        sf::Vector2f(rectPos.x, rectPos.y + this->rect.height),
        sf::Vector2f(this->rect.width, this->collisionBorder)
    );

    this->leftRect = sf::Rect<float>(
        sf::Vector2f(rectPos.x - this->collisionBorder, rectPos.y),
        sf::Vector2f(this->collisionBorder, this->rect.height)
    );

    this->rightRect = sf::Rect<float>(
        sf::Vector2f(rectPos.x + this->rect.width, rectPos.y),
        sf::Vector2f(this->collisionBorder, this->rect.height)
    );
}