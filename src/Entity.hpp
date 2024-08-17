#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"

#include "GameObject.hpp"
#include "ScriptItem.hpp"

enum EntityVar_t {
    NUMBER,
    STRING
};

enum EntityBody_t {
    STATIC,
    DYNAMIC
};

typedef struct {
    std::string name;
    EntityVar_t type;
    std::string stringval;
    double numval;
} EntityVar;

class Entity : public GameObject {
    public:
        std::string name;
        std::string texturePath;

        sf::Sprite sprite;
        b2Body* body;
        EntityBody_t bodytype = STATIC;
        float density = 0.0f;
        float friction = 0.3f;
        
        bool showHitbox = false;

        std::vector<ScriptItem> lua_scripts;
        std::vector<EntityVar> variables;

        bool isPlayer;

        Entity();
        Entity(sf::Vector2f position);
        Entity(Entity& e);  // For creating objects
        ~Entity();

        void Overwrite(Entity& e);
        void SetSprite(std::string path, bool autoScale = true);
        void SetPosition(sf::Vector2f position) override;
        void SetSize(sf::Vector2f size);
        void SetSpriteScale(sf::Vector2f scale);
        void CreateBody();
};

#endif