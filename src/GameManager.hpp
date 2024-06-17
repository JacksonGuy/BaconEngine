#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <map>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>
#include "imgui.h"

#include "Entity.hpp"
#include "File.hpp"

struct EditorSaveState {
    std::vector<Entity*> Entities;
    sf::Vector2f CameraPos;
    sf::Vector2f CameraSize;
};

class GameManager {
    public:
        static ConfigState config;
        static unsigned int screenWidth;
        static unsigned int screenHeight;
        static unsigned int framerateLimit;

        static std::map<std::string, sf::Texture*> Textures;
        static std::vector<Entity*> Entities;
        static sf::Font font;
        static Entity* player;
        static bool isPlayingGame;
        static ImGuiTextBuffer ConsoleLog;
        
        static float gravity;
        static unsigned int PlayerInputMode;
        static char* InputsModes[2];

        static sf::Texture* LoadTexture(std::string path);
        static void LoadFont(std::string path);
        static void DrawEntities(sf::RenderWindow& window);
        static void DrawText(sf::RenderWindow& window, std::string text, sf::Vector2f position);

        static bool MouseOnEntity(sf::Vector2f mousePos);
        static bool checkCollision(const Entity& e1, const Entity& e2);
        static bool checkCollision(const Entity& e1);
        static std::vector<Entity*> getCollidingWith(const Entity& e1);
        static bool checkCollisionSide(const sf::Rect<float> rect);

        static void SaveEditorState(sf::RenderWindow& window);
        static void RestoreEditorState(sf::RenderWindow& window);

        static void ConsoleWrite(std::string text);

    private:
        static EditorSaveState saveState;
};

#endif