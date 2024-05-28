#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <map>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "Entity.h"

class GameManager {
    public:
        static std::map<std::string, sf::Texture*> Textures;
        static std::vector<Entity*> Entities;
        
        static sf::Texture* LoadTexture(std::string path);
        static void DrawEntities(sf::RenderWindow& window);
    private:
};

#endif
