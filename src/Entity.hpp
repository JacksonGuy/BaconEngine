#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include "ScriptItem.hpp"

class Entity {
    public:
        static unsigned int IDNum;
        unsigned int ID;
        std::string name;
        std::string texturePath;

        sf::Vector2f position;
        sf::Vector2f scale;
        int width, height; 
        float rotation;
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

        // We can also have users type out lua code directly
        // into the editor, then save that code as a string,
        // and use L.do_string(str) to run the code.
        // It could be faster in certain scenarios, but adds an
        // extra layer of complexity to running/saving that
        // information, so we won't do that just yet.
        // std::vector<std::string> lua_strings;

        bool showDetailMenu;
        bool showHitbox;
        bool isPlayer;
        float speed;

        Entity(sf::Vector2f position = sf::Vector2f(0,0));
        Entity(Entity& e);

        void SetSprite(std::string path, bool autoScale = true);
        void SetPosition(sf::Vector2f position);
        void SetSpriteScale(sf::Vector2f scale);
        void UpdateRect();
        void UpdateCollisionRects();
};

#endif