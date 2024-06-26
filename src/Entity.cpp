#include "Entity.hpp"
#include "GameManager.hpp"

#include <iostream>

unsigned int Entity::IDNum = 0;

Entity::Entity(sf::Vector2f position) {
    this->ID = Entity::IDNum++;
    this->name = "Entity " + std::to_string(this->ID);
    this->texturePath = "";

    this->position = position;
    this->scale = {1.0f, 1.0f};
    width = 64;
    height = 64;
    this->rotation = 0.0f;

    this->showDetailMenu = false;
    this->showHitbox = false;
    this->isPlayer = false;
    this->isSolid = false;
    this->physicsObject = false;
    this->hitboxSize = 5.0f;

    this->mass = 1.f;
    this->speed = 0.f;
    this->grounded = false;
    this->velocity = sf::Vector2f(0, 0);
    this->acceleration = sf::Vector2f(0, 0);

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
    this->isSolid = e.isSolid;
    this->physicsObject = e.physicsObject;
    this->showHitbox = e.showHitbox;
    this->isPlayer = e.isPlayer;
    this->hitboxSize = e.hitboxSize;
    
    this->mass = e.mass;
    this->speed = e.speed;
    this->grounded = false; // DEBUG
    this->velocity = sf::Vector2f(0, 0); // DEBUG
    this->acceleration = sf::Vector2f(0, 0); // DEBUG

    this->lua_scripts = e.lua_scripts;
    this->entity_variables = e.entity_variables;
    this->entity_numbers = e.entity_numbers;
    this->entity_strings = e.entity_strings;

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
    float gap = 5; // DEBUG

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
        sf::Vector2f(this->hitboxSize, this->rect.height - gap)
    );

    this->rightRect = sf::Rect<float>(
        sf::Vector2f(rectPos.x + this->rect.width, rectPos.y),
        sf::Vector2f(this->hitboxSize, this->rect.height - gap)
    );
}