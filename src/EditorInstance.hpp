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
        std::string failedMessage;

        // Save and Load project variables
        bool showLoadPopup;
        std::string loadProjectName;
        bool showSaveAsPopup;
        std::string saveAsProjectName;

        // Create Entity Variables
        std::string createNameBuffer;
        float createPosition[2];
        std::string createImagePath;

        // Functions
        EditorInstance();
        void DrawUI(sf::Time deltaTime);              // Draw ImGui Frames
        void Update(sf::Time deltaTime);              // Input
        void FixedUpdate(sf::Time deltaTime);         // Physics
        void Run();
};

#endif