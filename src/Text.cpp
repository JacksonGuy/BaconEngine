#include "Text.hpp"
#include "GameManager.hpp"

unsigned int TextObj::IDNum = 0;

TextObj::TextObj() {
    this->ID = TextObj::IDNum++;
    this->text.setFont(GameManager::font);
    this->text.setCharacterSize(24);
    this->text.setFillColor(sf::Color(255,0,255,255));
}