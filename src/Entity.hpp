#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>

#include "GameObject.hpp"
#include "ScriptItem.hpp"

class Entity : public GameObject {
    public:
        std::string entity_type;
        std::string name;
        std::string texturePath;

        sf::Sprite sprite;
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

        bool showHitbox;
        bool isPlayer;

        Entity();
        Entity(sf::Vector2f position);
        Entity(Entity& e);  // For creating objects
        ~Entity();

        // For overriding current object data
        // Yes this does something different than our copy constructor
        void Override(Entity& e);
        void SetSprite(std::string path, bool autoScale = true);
        void SetPosition(sf::Vector2f position) override;
        void SetSpriteScale(sf::Vector2f scale);
        void UpdateRect();
        void UpdateCollisionRects();
};

#endif