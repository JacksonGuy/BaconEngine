#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include "ScriptItem.hpp"

class Entity {
    public:
        static unsigned int IDNum;
        unsigned int ID;
        std::string entity_type;
        std::string name;
        std::string texturePath;

        sf::Vector2f position;
        sf::Vector2f scale;
        int width, height; 
        float rotation;
        sf::Sprite sprite;
        bool isVisible;

        sf::Rect<float> rect;       // Whole sprite
        sf::Rect<float> topRect;    // Sides (for collision)
        sf::Rect<float> bottomRect;
        sf::Rect<float> leftRect;
        sf::Rect<float> rightRect;
        float hitboxSize;
        
        bool isSolid;           // Can collide with things
        bool physicsObject;     // Is affected by gravity
        float mass;
        bool grounded;
        sf::Vector2f velocity;
        sf::Vector2f acceleration;

        std::vector<ScriptItem> lua_scripts;
        std::map<int, std::string> entity_variables;
        std::map<std::string, double> entity_numbers;
        std::map<std::string, std::string> entity_strings;

        bool showDetailMenu;
        bool showHitbox;
        bool isPlayer;

        Entity(sf::Vector2f position = sf::Vector2f(0,0));
        Entity(Entity& e);

        void Copy(Entity& e);
        void SetSprite(std::string path, bool autoScale = true);
        void SetPosition(sf::Vector2f position);
        void SetSpriteScale(sf::Vector2f scale);
        void UpdateRect();
        void UpdateCollisionRects();
};

#endif