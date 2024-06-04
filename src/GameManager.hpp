#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <map>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "Entity.hpp"

class GameManager {
    public:
        static unsigned int screenWidth;
        static unsigned int screenHeight;
        static std::map<std::string, sf::Texture*> Textures;
        static std::vector<Entity*> Entities;
        static sf::Font font;
        static Entity* player;
        
        static sf::Texture* LoadTexture(std::string path);
        static void LoadFont(std::string path);
        static void DrawEntities(sf::RenderWindow& window);
        static void DrawText(sf::RenderWindow& window, std::string text, sf::Vector2f position);
        static sf::Vector2f ConvertMouseCoords(sf::Vector2f mousePos, sf::View& view);
    
        static bool MouseOnEntity(sf::Vector2f mousePos);
    private:
};

#endif
