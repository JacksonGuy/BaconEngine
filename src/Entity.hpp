#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>

#include "GameObject.hpp"
#include "ScriptItem.hpp"

enum EntityVar_t {
    NUMBER,
    STRING
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
        sf::Vector2f hitbox[5] = {};
        
        bool isSolid;           // Can collide with things
        bool physicsObject;     // Is affected by gravity
        float mass;
        bool grounded;
        sf::Vector2f velocity;
        sf::Vector2f acceleration;

        std::vector<ScriptItem> lua_scripts;
        std::vector<EntityVar> variables;

        bool showHitbox;
        bool editHitbox;
        bool isPlayer;

        Entity();
        Entity(sf::Vector2f position);
        Entity(Entity& e);  // For creating objects
        ~Entity();

        void Overwrite(Entity& e);
        void SetSprite(std::string path, bool autoScale = true);
        void SetPosition(sf::Vector2f position) override;
        void SetSpriteScale(sf::Vector2f scale);

        void CreateHitbox();
        void UpdateHitbox(sf::Vector2f delta);
};

#endif