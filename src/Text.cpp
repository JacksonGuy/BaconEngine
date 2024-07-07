#include "Text.hpp"
#include "GameManager.hpp"

unsigned int TextObj::IDNum = 0;

TextObj::TextObj() {
    this->ID = TextObj::IDNum++;
    this->name = "Text";
    this->text.setFont(GameManager::font);
    this->text.setCharacterSize(24);
    this->text.setFillColor(sf::Color::Black);

    this->showDetails = false;
    this->isVisible = true;

    this->mode = TextRenderMode::Absolute;
    this->target = nullptr;
    this->target_id = 0;

    GameManager::TextObjects.push_back(this);
}

TextObj::TextObj(TextObj& obj) {
    this->ID = obj.ID;
    this->name = obj.name;

    this->target_id = obj.target_id;
    this->target = GameManager::FindEntityByID(this->target_id);

    this->text = obj.text;
    this->position = obj.position;
    this->mode = obj.mode;
    this->showDetails = obj.showDetails;
    this->isVisible = obj.isVisible;
}