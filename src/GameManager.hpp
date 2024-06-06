#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <map>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "Entity.hpp"

struct EditorSaveState {
    std::vector<Entity*> Entities;
    sf::Vector2f CameraPos;
};

class GameManager {
    public:
        static unsigned int screenWidth;
        static unsigned int screenHeight;
        static std::map<std::string, sf::Texture*> Textures;
        static std::vector<Entity*> Entities;
        static sf::Font font;
        static Entity* player;
        static bool isPlayingGame;

        static sf::Texture* LoadTexture(std::string path);
        static void LoadFont(std::string path);
        static void DrawEntities(sf::RenderWindow& window);
        static void DrawText(sf::RenderWindow& window, std::string text, sf::Vector2f position);

        static bool MouseOnEntity(sf::Vector2f mousePos);

        static void SaveEditorState(sf::RenderWindow& window);
        static void RestoreEditorState(sf::RenderWindow& window);

    private:
        static EditorSaveState saveState;
};

#endif