#ifndef TEXT_H
#define TEXT_H

#include <iostream>
#include <SFML/Graphics.hpp>

#include "GameObject.hpp"

class TextObj : public GameObject {
    public:
        sf::Text text;

        TextObj();
        TextObj(TextObj& obj);
        ~TextObj();

        void Overwrite(TextObj& text);
        void SetPosition(sf::Vector2f position) override;
};

#endif