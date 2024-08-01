#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>
#include "imgui.h"

#include "Entity.hpp"
#include "Text.hpp"
#include "File.hpp"
#include "Camera.hpp"

extern "C" {
    #include <lauxlib.h>
    #include <lua.h>
    #include <lualib.h>
}

class GameManager {
    public:
        static File::ConfigState config;
        static unsigned int screenWidth;
        static unsigned int screenHeight;
        static unsigned int framerateLimit;

        static std::vector<GameObject*> GameObjects;
        static std::vector<Entity*> Entities;
        static std::vector<TextObj*> TextObjects;
        static std::vector<Camera*> Cameras;
        
        static sf::Font font;
        static Entity* player;
        static Camera* camera;
        static bool isPlayingGame;
        static bool windowHasFocus;
        static std::string entryPoint;
        static sf::Keyboard::Key lastKeyboardInput;
        static sf::Mouse::Button lastMouseInput;
        static sf::Vector2f mousePos;
        static ImGuiTextBuffer ConsoleLog;
        
        static float gravity;
        static sf::Clock clock;

        static bool checkCollision(const Entity& e1, const Entity& e2);
        static bool checkCollision(const Entity& e1);
        static std::vector<Entity*> getCollidingWith(const Entity& e1);
        static bool checkCollisionSide(const sf::Rect<float> rect);

        static void SaveEditorState(sf::RenderWindow& window, std::string filename);
        static void RestoreEditorState(sf::RenderWindow& window, std::string filename);

        static GameObject* FindObjectByID(int id);
        static Entity* FindEntityByID(int id);
        static Entity* FindEntityByName(std::string name);
        static std::vector<GameObject*> FindObjectsByTag(std::string tag);
        static TextObj* FindTextByID(int id);

        static void SortObjectsByID();

        static void ConsoleWrite(std::string text);
};

#endif