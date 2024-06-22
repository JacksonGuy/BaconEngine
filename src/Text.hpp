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
    public:
        static unsigned int IDNum;
        unsigned int ID;
        std::string name;
        sf::Text text;
        sf::Vector2f position;
        Entity* entity;
        TextRenderMode mode;
        bool showDetails;

        TextObj();
        TextObj(TextObj& obj);
};

#endif