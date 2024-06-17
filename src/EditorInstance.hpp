#ifndef EDITOR_INSTANCE_H
#define EDITOR_INSTANCE_H

#include <iostream>
#include <queue>

#include "imgui.h"
#include "imgui-SFML.h"

#include "GameManager.hpp"
#include "Entity.hpp"
#include "File.hpp" 

namespace Settings {};

class EditorInstance {
    public:
        std::string projectTitle;
        bool loadedProject = false;

        sf::RenderWindow* window;
        sf::View* camera;
        float cameraZoom;

        // Engine Variables
        sf::Clock clock;
        sf::Vector2f mousePos;
        sf::Time lastFixedUpdate;
        unsigned int frameLimit;
        sf::Time TimePerFrame;

        // UI Variables
        bool cameraMove;
        bool showEntityCreate;
        bool showMainMenu;
        bool showEntityList;
        bool showConsole;
        bool showSettingsMenu;
        bool showFailedPopup;
        bool showTextCreate;
        std::string failedMessage;

        // Save and Load project variables
        bool showLoadPopup;
        char loadProjectName[64];
        bool showSaveAsPopup;
        char saveAsProjectName[64];

        // Create Entity Variables
        char createNameBuffer[64];
        float createPosition[2];
        char createImagePath[64];

        // Create Text Variables
        

        // Functions
        EditorInstance();
        void DrawUI(sf::Time deltaTime);              // Draw ImGui Frames
        void Update(sf::Time deltaTime);              // Input
        void FixedUpdate(sf::Time deltaTime);         // Physics
        void Run();
};

#endif