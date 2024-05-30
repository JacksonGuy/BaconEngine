#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>

class Entity {
    static unsigned int IDNum;

    public:
        unsigned int ID;
        std::string name;

        sf::Vector2f position;
        sf::Vector2f scale;
        int width, height; 
        float rotation;
        sf::Sprite sprite;
        sf::Rect<int> rect;

        Entity(sf::Vector2f position);

        void SetSprite(std::string path);
        void SetPosition(sf::Vector2f position);
        void SetSpriteScale(sf::Vector2f scale);
        void UpdateRect();
};

#endif