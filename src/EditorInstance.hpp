#ifndef EDITOR_INSTANCE_H
#define EDITOR_INSTANCE_H

#include <vector>
#include <unordered_map>

#include "imgui.h"
#include "imgui-SFML.h"

#include "Entity.hpp"

#define BUFFSIZE 512

namespace Settings {};

struct RenderLayer {
    std::vector<GameObject*> objects;
    unsigned int layerNum;
};

class EditorInstance {
    public:
        std::string m_projectTitle;
        bool m_loadedProject = false;

        sf::RenderWindow* m_window; // Program window
        sf::View* m_camera;         // Editor Camera
        sf::Vector2f m_cameraPos;   // Used to modify camera position
        sf::Vector2f m_cameraSize;  // Used to modify camera size

        // Scene window info
        bool m_swindowFocused;        // Are we clicked on the window
        sf::Vector2f m_swindowpos;    // Position relative to entire window
        sf::Vector2f m_swindowsize;   // Scene window size
        sf::Vector2f m_sceneMousePos; // Mouse position within scene window
        bool m_sceneMouseCapture;     // Is mouse hovering over scene window
        GameObject* m_viewObject;     // Object to display in properties window

        // Engine Variables
        sf::Clock m_clock;          // Time
        sf::Vector2i m_mousePos;    // Used for camera movements
        sf::Time m_lastFixedUpdate; // Time since last fixed update
        unsigned int m_frameLimit;  // How often to run fixed update (frames) 
        sf::Time m_TimePerFrame;    // How often to run fixed update (seconds)
        std::unordered_map<sf::Keyboard::Key, bool> m_keypresses; // Keyboard Inputs

        // UI Variables
        bool m_cameraMove;                      // Are we moving the camera
        bool m_showEntityCreate;                // Show Entity create menu 
        bool m_showMainMenu;                    // Show main side panel
        bool m_showEntityList;                  // Show GameObject tree
        bool m_showConsole;                     // Show Console
        bool m_showSettingsMenu;                // Show Editor settings
        bool m_showTextCreate;                  // Show TextObj create menu
        bool m_showCameraCreate;                // Show Camera create menu
        std::string m_failedMessage;            // Used to store error messages
        GameObject* m_currentSelectedObject;    // GameObject to move with mouse
        GameObject* m_copyObject;               // GameObject to copy/paste
        bool m_ConsoleEngineMessages = true;    // Show engine messages in console

        Entity* m_AddAttributeEntity;           // Used when adding variable to entity
        char m_AddScriptName[BUFFSIZE];         // Script name
        char m_AddVariableName[BUFFSIZE];       // Variable name
        int m_AddVariableType;                  // Variable type
        double m_AddVariableNumber;             // Variable number value
        char m_AddVariableString[BUFFSIZE];     // Variable string value

        char m_PrefabPath[BUFFSIZE];            // Where to save prefab file

        // Create Entity Variables
        char m_createNameBuffer[BUFFSIZE];
        float m_createPosition[2];
        char m_createImagePath[BUFFSIZE];
        int m_createDimension[2];

        // Create Text Variables
        char m_createTextName[BUFFSIZE];
        char m_createTextDetails[BUFFSIZE * 64];
        float m_createTextPosition[2];

        // Create Camera Variables
        char m_createCameraName[BUFFSIZE];
        float m_createCameraPosition[2];
        int m_createCameraSize[2];

        // Engine Functions
        EditorInstance();
        ~EditorInstance();
        void DrawUI(sf::Time deltaTime);              // Draw ImGui Frames
        void Update(sf::Time deltaTime);              // Input
        void FixedUpdate(sf::Time deltaTime);         // Physics
        void Run();

        // UI Functions
        void DisplayEntityTree(GameObject* e);
        void* CopyTree(GameObject* obj);

    private:
        void PrintVector(sf::Vector2f vec);
        void SetImGuiStyle();
};

#endif
