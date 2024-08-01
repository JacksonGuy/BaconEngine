#include "GameObject.hpp"
#include "GameManager.hpp"
#include "Rendering.hpp"

unsigned int GameObject::IDCount = 0;

GameObject::GameObject() {
    this->type = OBJECT;
    this->ID = GameObject::IDCount++;
    this->name = "Object " + std::to_string(this->ID);
    this->tag = "Default";

    this->position = sf::Vector2f(0.f, 0.f); 
    this->scale = sf::Vector2f(1.f, 1.f);
    this->width = 0;
    this->height = 0;
    this->rotation = 0.f;
    this->isVisible = true;
    this->layer = 0;

    this->parent = nullptr;
    this->children = std::vector<GameObject*>();

    this->showDetails = false;

    GameManager::GameObjects.push_back(this);
}

GameObject::GameObject(GameObject& obj) {
    this->ID = obj.ID;
    this->type = obj.type;
    this->name = obj.name;
    this->tag = obj.tag;

    this->position = obj.position;
    this->scale = obj.scale;
    this->width = obj.width;
    this->height = obj.height;
    this->rotation = obj.rotation;
    this->isVisible = obj.isVisible;

    this->parent = obj.parent;
    this->children = obj.children;

    this->showDetails = obj.showDetails;

    GameManager::GameObjects.push_back(this);
}

GameObject::~GameObject() {
    // Remove from GameObjects
    for (size_t i = 0; i < GameManager::GameObjects.size(); i++) {
        if (GameManager::GameObjects[i]->ID == this->ID) {
            GameManager::GameObjects.erase(GameManager::GameObjects.begin() + i);
            break;
        }
    }

    // Remove GameObject from parent's children vector
    if (parent != nullptr) {
        for (size_t i = 0; i < parent->children.size(); i++) {
            if (parent->children[i]->ID == this->ID) {
                parent->children.erase(parent->children.begin() + i);
                break;
            }
        }
    }

    // Remove as parent for children
    for (GameObject* obj : this->children) {
        obj->parent = nullptr;
    }

    // Remove from RenderingLayer
    Rendering::RemoveFromLayer(this);
}

/**
 * @brief Copies the variables of another GameObject
 * 
 * @param obj The GameObject to copy from
 */
void GameObject::Overwrite(GameObject& obj) {
    this->name = obj.name;
    this->type = obj.type;
    this->tag = obj.tag;
    
    this->scale = obj.scale;
    this->width = obj.width;
    this->height = obj.height;
    this->rotation = obj.rotation;
    this->isVisible = obj.isVisible; 
} 

/**
 * @brief Sets the position of the GameObject and its children
 * 
 * @param position the new position
 */
void GameObject::SetPosition(sf::Vector2f position) {
    sf::Vector2f delta = position - this->position;
    
    this->position = position;

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

/**
 * @brief Updates the positions of all children GameObjects relative to some change
 * 
 * @param change The amount to change each child's position by
 */
void GameObject::UpdateChildrenPositions(sf::Vector2f change) {
    for (GameObject* child : this->children) {
        sf::Vector2f newPos = child->position + change;

        if (child->type == ENTITY) {
            Entity* e = (Entity*)child;
            e->SetPosition(newPos);
        }
        else if (child->type == TEXT) {
            TextObj* text = (TextObj*)child;
            text->SetPosition(newPos);
        }
        else if (child->type == CAMERA) {
            Camera* camera = (Camera*)child;
            camera->SetPosition(newPos);
        }
    }
}