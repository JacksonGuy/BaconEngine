#include "Entity.hpp"
#include "GameManager.hpp"
#include "Rendering.hpp"

#include <iostream>
#include <fstream>

Entity::Entity() : GameObject() {
    this->name = "Entity";
    this->type = ENTITY;
    
    this->texturePath = "";

    this->showHitbox = false;
    this->isPlayer = false;
    this->isSolid = false;
    this->physicsObject = false;

    this->lua_scripts = std::vector<ScriptItem>();

    this->mass = 1.f;
    this->grounded = false;
    this->velocity = sf::Vector2f(0, 0);
    this->acceleration = sf::Vector2f(0, 0);

    this->InitRect();

    GameManager::Entities.push_back(this);
}

Entity::Entity(sf::Vector2f position) : Entity() {
    this->position = position;
}   

Entity::Entity(Entity& e) : GameObject(e) {
    this->name = e.name;
    this->ID = e.ID;

    this->isSolid = e.isSolid;
    this->physicsObject = e.physicsObject;
    this->showHitbox = e.showHitbox;
    this->isPlayer = e.isPlayer;
    
    this->mass = e.mass;
    this->grounded = false;
    this->velocity = sf::Vector2f(0, 0);
    this->acceleration = sf::Vector2f(0, 0);

    this->lua_scripts = e.lua_scripts;
    this->entity_variables = e.entity_variables;
    this->entity_numbers = e.entity_numbers;
    this->entity_strings = e.entity_strings;

    this->SetSprite(e.texturePath, false);
    this->sprite.setPosition(this->position);
    
    this->rect = e.rect;
    this->topRect = e.topRect;
    this->bottomRect = e.bottomRect;
    this->leftRect = e.leftRect;
    this->rightRect = e.rightRect;

    GameManager::Entities.push_back(this);
}

Entity::~Entity() {
    // Remove from Entities
    for (size_t i = 0; i < GameManager::Entities.size(); i++) {
        if (GameManager::Entities[i]->ID == ID) {
            GameManager::Entities.erase(GameManager::Entities.begin() + i);
            break;
        }
    }
}

/**
 * @brief Copies the variables of another Entity
 * 
 * @param e The Entity to copy from
 */
void Entity::Overwrite(Entity& e) {
    GameObject::Overwrite(e);
    this->name = e.name;

    this->isSolid = e.isSolid;
    this->physicsObject = e.physicsObject;
    this->showHitbox = e.showHitbox;
    this->isPlayer = false;
    
    this->mass = e.mass;
    this->grounded = false;
    this->velocity = sf::Vector2f(0, 0);
    this->acceleration = sf::Vector2f(0, 0);

    this->lua_scripts = e.lua_scripts;
    this->entity_variables = e.entity_variables;
    this->entity_numbers = e.entity_numbers;
    this->entity_strings = e.entity_strings;

    this->SetSprite(e.texturePath, false);
    this->SetPosition(this->position);
}

/**
 * @brief Sets the sprite of the Entity
 * 
 * @param path path to the image
 * @param autoScale automatically scale the image to fit the Entity's width and height
 */
void Entity::SetSprite(std::string path, bool autoScale) {
    this->texturePath = path;
    sf::Texture* texture = Rendering::LoadTexture(path);
    if (autoScale) {
        this->scale.x = ((float)this->width) / texture->getSize().x;
        this->scale.y = ((float)this->height) / texture->getSize().y;
    }
    
    this->sprite.setTexture(*texture, true);
    this->sprite.setPosition(this->position);
    this->sprite.setOrigin((sf::Vector2f)texture->getSize()/2.0f);
    this->sprite.setScale(this->scale);
}

/**
 * @brief Sets the position of the Entity and its children
 * 
 * @param position the new position
 */
void Entity::SetPosition(sf::Vector2f position) {
    sf::Vector2f delta = position - this->position;

    this->position = position;
    this->sprite.setPosition(this->position);
    this->UpdateRect(delta);

    this->UpdateChildrenPositions(delta);
}

/**
 * @brief Scales the Entity sprite
 * 
 * @param scale the scale to apply
 */
void Entity::SetSpriteScale(sf::Vector2f scale) {
    this->scale = scale;
    this->sprite.setScale(this->scale);
}

void Entity::InitRect() {
    this->rect = sf::Rect<float>(
        this->position.x - this->width/2,
        this->position.y - this->height/2, 
        this->width, this->height);

    this->UpdateCollisionRects();
}

/**
 * @brief Updates the border rect and side rects
 */
void Entity::UpdateRect(sf::Vector2f change) {
    this->rect.left += change.x;
    this->rect.top += change.y;

    this->UpdateCollisionRects();
}

/**
 * @brief Updates the side rects
 */
void Entity::UpdateCollisionRects() {
    sf::Vector2f rectPos = this->rect.getPosition();
    sf::Vector2f size = sf::Vector2f(5, 5);

    const float gap = 1;

    this->topRect = sf::Rect<float>(
        sf::Vector2f(rectPos.x, rectPos.y - size.y),
        sf::Vector2f(this->rect.width, size.x - gap)
    );

    this->bottomRect = sf::Rect<float>(
        sf::Vector2f(rectPos.x, rectPos.y + this->rect.height + gap),
        sf::Vector2f(this->rect.width, size.y)
    );

    this->leftRect = sf::Rect<float>(
        sf::Vector2f(rectPos.x - gap, rectPos.y),
        sf::Vector2f(size.x, this->rect.height)
    );

    this->rightRect = sf::Rect<float>(
        sf::Vector2f(rectPos.x + this->rect.width + gap, rectPos.y),
        sf::Vector2f(size.x, this->rect.height)
    );
}