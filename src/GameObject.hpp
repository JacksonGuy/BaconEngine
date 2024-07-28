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
        std::string tag;
        
        sf::Vector2f position;
        sf::Vector2f scale;
        int width, height;
        float rotation;
        bool isVisible;
        unsigned int layer;

        GameObject* parent;
        std::vector<GameObject*> children;
        std::vector<unsigned int> childrenIDs;

        bool showDetails;

        GameObject();
        GameObject(GameObject& obj);
        virtual ~GameObject();

        void Overwrite(GameObject& obj);
        virtual void SetPosition(sf::Vector2f position);
};

#endif