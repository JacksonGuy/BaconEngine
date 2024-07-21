#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <string>
#include <SFML/Graphics.hpp>

enum ClassType {
    OBJECT,
    ENTITY,
    TEXT
};

class GameObject {
    public:
        ClassType type;

        static unsigned int IDCount;
        unsigned int ID;
        std::string name;
        
        sf::Vector2f position;
        sf::Vector2f scale;
        int width, height;
        float rotation;
        bool isVisible;

        GameObject* parent;
        std::vector<GameObject*> children;

        bool showDetails;

        GameObject();
        GameObject(GameObject& obj);
        ~GameObject();

        virtual void SetPosition(sf::Vector2f position);
};

#endif