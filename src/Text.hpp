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
        Entity* target;
        int target_id;
        TextRenderMode mode;
        bool showDetails;
        bool isVisible;

        TextObj();
        TextObj(TextObj& obj);
};

#endif