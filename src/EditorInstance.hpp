#ifndef EDITOR_INSTANCE_H
#define EDITOR_INSTANCE_H

#include <iostream>
#include <queue>

#include "imgui.h"
#include "imgui-SFML.h"

#include "GameManager.hpp"
#include "Entity.hpp"
#include "File.hpp"
#include "Text.hpp"

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
        char loadProjectName[256];
        bool showSaveAsPopup;
        char saveAsProjectName[256];

        // Create Entity Variables
        char createNameBuffer[256];
        float createPosition[2];
        char createImagePath[256];

        // Create Text Variables
        char createTextName[256];
        char createTextDetails[1024 * 16];
        float createTextPosition[2];
        int createTextMode;
        int createTextEntityId;

        // Functions
        EditorInstance();
        void DrawUI(sf::Time deltaTime);              // Draw ImGui Frames
        void Update(sf::Time deltaTime);              // Input
        void FixedUpdate(sf::Time deltaTime);         // Physics
        void Run();
};

#endif