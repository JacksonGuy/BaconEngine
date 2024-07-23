#include "Entity.hpp"
#include "GameManager.hpp"

#include <iostream>
#include <fstream>

Entity::Entity() : GameObject() {
    this->name = "Entity " + std::to_string(this->ID);
    this->type = ClassType::ENTITY;
    
    this->entity_type = "Default";
    this->texturePath = "";
    this->width = 64;
    this->height = 64;

    this->showHitbox = false;
    this->isPlayer = false;
    this->isSolid = false;
    this->physicsObject = false;
    this->hitboxSize = 5.0f;

    this->lua_scripts = std::vector<ScriptItem>();

    this->mass = 1.f;
    this->grounded = false;
    this->velocity = sf::Vector2f(0, 0);
    this->acceleration = sf::Vector2f(0, 0);

    GameManager::Entities.push_back(this);
}

Entity::Entity(sf::Vector2f position) : Entity() {
    this->position = position;
}   

Entity::Entity(Entity& e) : GameObject(e) {
    this->name = e.name;
    this->ID = e.ID;
    this->type = e.type;
    
    this->entity_type = e.entity_type;

    this->isSolid = e.isSolid;
    this->physicsObject = e.physicsObject;
    this->showHitbox = e.showHitbox;
    this->isPlayer = e.isPlayer;
    this->hitboxSize = e.hitboxSize;
    
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
    this->UpdateRect();

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

void Entity::Override(Entity& e) {
    this->name = e.name;
    this->entity_type = e.entity_type;
    
    this->scale = e.scale;
    this->width = e.width;
    this->height = e.height;
    this->rotation = e.rotation;
    this->isVisible = e.isVisible;

    this->showDetails = true;
    this->isSolid = e.isSolid;
    this->physicsObject = e.physicsObject;
    this->showHitbox = e.showHitbox;
    this->isPlayer = false;
    this->hitboxSize = e.hitboxSize;
    
    this->mass = e.mass;
    this->grounded = false;
    this->velocity = sf::Vector2f(0, 0);
    this->acceleration = sf::Vector2f(0, 0);

    this->parent = e.parent;
    this->children = e.children;

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
    sf::Vector2f delta = position - this->position;

    this->position = position;
    this->sprite.setPosition(this->position);
    this->UpdateRect();

    for (GameObject* child : this->children) {
        sf::Vector2f newPos = child->position + delta;

        if (child->type == ENTITY) {
            Entity* e = (Entity*)child;
            e->SetPosition(newPos);
        }
        else if (child->type == TEXT) {
            TextObj* text = (TextObj*)child;
            text->SetPosition(newPos);
        }
    }
}

void Entity::SetSpriteScale(sf::Vector2f scale) {
    this->scale = scale;
    this->sprite.setScale(this->scale);
}

void Entity::UpdateRect() {
    sf::Rect spriteRect = this->sprite.getGlobalBounds();
    sf::Vector2f pos = spriteRect.getPosition();
    sf::Vector2f size = spriteRect.getSize();
    sf::Rect newRect = sf::Rect(pos.x - this->hitboxSize, pos.y - this->hitboxSize, 
        size.x + (this->hitboxSize * 2), size.y + (this->hitboxSize * 2));
    this->rect = newRect;
    this->UpdateCollisionRects();
}

void Entity::UpdateCollisionRects() {
    sf::Vector2f rectPos = this->rect.getPosition();
    // This fixes some issues we have with physics collisions occurring 
    // with our left and right rects when jumping/falling.
    // If it works, don't change it...
    const float gap = 5;

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