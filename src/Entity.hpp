#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>

class Entity {
    static unsigned int IDNum;

    public:
        unsigned int ID;
        std::string name;
        std::string texturePath;

        sf::Vector2f position;
        sf::Vector2f scale;
        int width, height; 
        float rotation;
        sf::Sprite sprite;
        
        sf::Rect<float> rect;       // Whole sprite
        bool isSolid;           // Can collide with things
        bool physicsObject;     // Is affected by gravity

        bool showDetailMenu;
        bool isPlayer;

        Entity(sf::Vector2f position = sf::Vector2f(0,0));
        Entity(Entity& e);

        void SetSprite(std::string path, bool autoScale = true);
        void SetPosition(sf::Vector2f position);
        void SetSpriteScale(sf::Vector2f scale);
        void UpdateRect();
};

#endif