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

namespace GameManager {
    extern File::ConfigState config;
    extern unsigned int screenWidth;
    extern unsigned int screenHeight;
    extern unsigned int framerateLimit;

    extern std::vector<GameObject*> GameObjects;
    extern std::vector<Entity*> Entities;
    extern std::vector<TextObj*> TextObjects;
    extern std::vector<Camera*> Cameras;
    
    extern sf::Font font;
    extern Entity* player;
    extern Camera* camera;
    extern bool isPlayingGame;
    extern bool windowHasFocus;
    extern std::string entryPoint;
    extern sf::Keyboard::Key lastKeyboardInput;
    extern std::unordered_map<sf::Keyboard::Key, bool> keypresses;
    extern sf::Mouse::Button lastMouseInput;
    extern sf::Vector2f mousePos;

    extern ImGuiTextBuffer ConsoleLog;
    extern std::vector<std::string> ConsoleMessages;
    
    extern float gravity;
    extern sf::Clock clock; 

    bool CheckCollisionLines(sf::Vector2f s1, sf::Vector2f e1,
        sf::Vector2f s2, sf::Vector2f e2);
    bool CheckCollisionPointLine(sf::Vector2f point, sf::Vector2f s, sf::Vector2f e);
    bool CheckCollisionPointRect(sf::Vector2f point, sf::FloatRect rect);

    bool checkCollision(const Entity& e1, const Entity& e2);
    bool checkCollision(const Entity& e1);
    std::vector<Entity*> getCollidingWith(const Entity& e1);
    bool checkCollisionSide(const Entity& e, const std::string side);

    void SaveEditorState(sf::RenderWindow& window, std::string filename);
    void RestoreEditorState(sf::RenderWindow& window, std::string filename);

    GameObject* FindObjectByID(int id);
    Entity* FindEntityByID(int id);
    Entity* FindEntityByName(std::string name);
    std::vector<Entity*> FindEntitiesByTag(std::string tag);
    TextObj* FindTextByID(int id);

    void SortObjectsByID();

    void ConsoleWrite(std::string text);
};

#endif