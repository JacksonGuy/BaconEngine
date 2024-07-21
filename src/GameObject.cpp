#include "GameObject.hpp"
#include "GameManager.hpp"

unsigned int GameObject::IDCount = 0;

GameObject::GameObject() {
    this->type = OBJECT;
    this->ID = GameObject::IDCount++;
    this->name = "Object " + std::to_string(this->ID);

    this->position = sf::Vector2f(0.f, 0.f); 
    this->scale = sf::Vector2f(1.f, 1.f);
    this->width = 0;
    this->height = 0;
    this->rotation = 0.f;
    this->isVisible = true;

    this->parent = nullptr;
    this->children = std::vector<GameObject*>();

    this->showDetails = false;

    GameManager::GameObjects.push_back(this);
}

GameObject::GameObject(GameObject& obj) {
    this->ID = obj.ID;
    this->type = obj.type;
    this->name = obj.name;

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
}

void GameObject::SetPosition(sf::Vector2f position) {
    sf::Vector2f delta = position - this->position;
    
    this->position = position;

    for (GameObject* child : this->children) {
        sf::Vector2f newPos = child->position + delta;
        child->SetPosition(newPos);
    }
}