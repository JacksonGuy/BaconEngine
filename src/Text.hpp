#ifndef TEXT_H
#define TEXT_H

#include <iostream>
#include <SFML/Graphics.hpp>

#include "Entity.hpp"

enum TextRenderMode {
    Absolute,
    Relative,
    Screen
};

class TextObj {
    static unsigned int IDNum;

    public:
        unsigned int ID;
        std::string name;
        sf::Text text;
        sf::Vector2f position;
        Entity entity;
        TextRenderMode mode;
        bool showDetails;

        TextObj();
};

#endif