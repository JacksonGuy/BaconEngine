#include "Entity.hpp"
#include "GameManager.hpp"
#include "Rendering.hpp"
#include "BaconMath.h"

#include <iostream>
#include <fstream>

Entity::Entity() : GameObject() {
    this->name = "Entity";
    this->type = ENTITY;
    
    this->texturePath = "";

    this->width = 64;
    this->height = 64;
    // CreateBody();

    this->isPlayer = false;

    this->lua_scripts = std::vector<ScriptItem>();

    GameManager::Entities.push_back(this);
}

Entity::Entity(sf::Vector2f position) : Entity() {
    this->position = position;
    // CreateBody();
}   

Entity::Entity(Entity& e) : GameObject(e) {
    this->name = e.name;
    this->ID = e.ID;
    
    this->isPlayer = e.isPlayer;
    if (this->isPlayer) {
        GameManager::player = this;
    }

    this->lua_scripts = e.lua_scripts;
    this->variables = e.variables;

    this->SetSprite(e.texturePath, false);
    this->sprite.setPosition(this->position);

    this->bodytype = e.bodytype;
    this->density = e.density;
    this->friction = e.friction;
    CreateBody();

    GameManager::Entities.push_back(this);
}

Entity::~Entity() {
    // Remove from Entities
    for (size_t i = 0; i < GameManager::Entities.size(); i++) {
        free(this->body);
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

    this->isPlayer = false;

    this->lua_scripts = e.lua_scripts;
    this->variables = e.variables;

    this->SetSprite(e.texturePath, false);
    this->SetPosition(this->position);
    
    this->bodytype = e.bodytype;
    this->density = e.density;
    this->friction = e.friction;
    CreateBody();
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

    // body->SetTransform({position.x, position.y}, body->GetAngle());
    this->UpdateChildrenPositions(delta);
}

/**
 * @brief Sets the size of the entity and its hitbox
 * 
 * @param size the new size of the entity
 */
void Entity::SetSize(sf::Vector2f size) {

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

void Entity::CreateBody() {
    // Body Definition
    b2BodyDef boxBodyDef;
    boxBodyDef.position.Set(this->position.x / bmath::PPM, this->position.y / bmath::PPM);
    if (bodytype == STATIC) {
        boxBodyDef.type = b2_staticBody;
    }
    else if (bodytype == DYNAMIC) {
        boxBodyDef.type = b2_dynamicBody;
    }

    // Shape definition
    b2PolygonShape boxShape;
    boxShape.SetAsBox((width / 2 / bmath::PPM), (height / 2 / bmath::PPM));

    // Fixture Definition
    b2FixtureDef fixtureDef;
    fixtureDef.density = this->density;
    fixtureDef.friction = this->friction;
    fixtureDef.shape = &boxShape;

    // Create Body
    this->body = GameManager::world->CreateBody(&boxBodyDef);
    this->body->CreateFixture(&fixtureDef);
}