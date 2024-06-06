#ifndef PLAYER_INSTANCE_H
#define PLAYER_INSTANCE_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class PlayerInstance {
    public:
        unsigned int screenWidth;
        unsigned int screenHeight;

        sf::RenderWindow* window;
        sf::View* camera;

        bool playerExists;

        PlayerInstance();
        bool LoadGame(std::string name);
        void Update();
};

#endif