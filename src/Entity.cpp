#include "Entity.hpp"
#include "GameManager.hpp"
#include "Rendering.hpp"

#include <iostream>
#include <fstream>

Entity::Entity() : GameObject() {
    this->name = "Entity";
    this->type = ENTITY;
    
    this->texturePath = "";

    this->width = 64;
    this->height = 64;

    this->showHitbox = false;
    this->editHitbox = false;
    this->isPlayer = false;
    this->isSolid = false;
    this->physicsObject = false;

    this->lua_scripts = std::vector<ScriptItem>();

    this->mass = 1.f;
    this->grounded = false;
    this->velocity = sf::Vector2f(0, 0);
    this->acceleration = sf::Vector2f(0, 0);

    this->CreateHitbox();

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
    this->editHitbox = e.editHitbox;
    
    this->isPlayer = e.isPlayer;
    if (this->isPlayer) {
        GameManager::player = this;
    }

    this->mass = e.mass;
    this->grounded = false;
    this->velocity = sf::Vector2f(0, 0);
    this->acceleration = sf::Vector2f(0, 0);

    this->lua_scripts = e.lua_scripts;
    this->variables = e.variables;

    this->SetSprite(e.texturePath, false);
    this->sprite.setPosition(this->position);
    
    this->CreateHitbox();

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
    this->variables = e.variables;

    this->SetSprite(e.texturePath, false);

    sf::Vector2f delta = this->position - e.position; 
    for (int i = 0; i < 5; i++) {
        this->hitbox[i] = e.hitbox[i];
    }
    this->UpdateHitbox(delta);
    
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
    this->UpdateHitbox(delta);

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

/**
 * @brief Creates a new hitbox with points located at the corners
 * of the entity's sprite
 * 
 */
void Entity::CreateHitbox() {
    sf::Vector2f p = position;
    sf::Vector2f s = sf::Vector2f(width/2, height/2);

    hitbox[0] = sf::Vector2f(p.x - s.x, p.y - s.y); // Top Left
    hitbox[1] = sf::Vector2f(p.x + s.x, p.y - s.y); // Top Right
    hitbox[2] = sf::Vector2f(p.x + s.x, p.y + s.y); // Bottom Right
    hitbox[3] = sf::Vector2f(p.x - s.x, p.y + s.y); // Bottom Left
    hitbox[4] = sf::Vector2f(p.x - s.x, p.y - s.y); // Top Left
}

/**
 * @brief Updates the vertices of the hitbox
 * 
 */
void Entity::UpdateHitbox(sf::Vector2f delta) {
    for (int i = 0; i < 5; i++) {
        hitbox[i].x += delta.x;
        hitbox[i].y += delta.y;
    }
}