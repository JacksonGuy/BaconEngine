#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>

class Entity {
    public:
        sf::Vector2<float> position;
        sf::Vector2<float> scale;
        int width, height; 
        float rotation;
        sf::Sprite sprite;
        sf::Rect<int> rect;

        Entity();
        Entity(sf::Vector2<float> position);

        void SetSprite(std::string path);
        void UpdateSpriteScale();
        void UpdateRect();
};

#endif