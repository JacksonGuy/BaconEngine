#include "Text.hpp"
#include "GameManager.hpp"

TextObj::TextObj() : GameObject() {
    this->name = "Text";
    this->type = TEXT;
    this->text = sf::Text("Words", GameManager::font);
    this->text.setCharacterSize(24);
    this->text.setFillColor(sf::Color::Black);

    this->showDetails = false;
    this->isVisible = true;

    GameManager::TextObjects.push_back(this);
}

TextObj::TextObj(TextObj& obj) : GameObject(obj) {
    this->type = TEXT;

    this->text = obj.text;

    this->showDetails = obj.showDetails;
    this->isVisible = obj.isVisible;

    GameManager::TextObjects.push_back(this);
}

TextObj::~TextObj() {}

/**
 * @brief Copies the variables of the given TextObj
 * 
 * @param text the TextObj to copy
 */
void TextObj::Overwrite(TextObj& text) {
    GameObject::Overwrite(text);
    
    this->name = text.name;
    this->text = text.text;
}

/**
 * @brief Sets the position of the TextObj and children objects
 * 
 * @param position the new position
 */
void TextObj::SetPosition(sf::Vector2f position) { 
    sf::Vector2f delta = position - this->position;
    
    this->position = position;
    this->text.setPosition(this->position);

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