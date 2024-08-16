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

        sf::RenderWindow* m_window;
        sf::View* m_camera;
        sf::Vector2f m_cameraPos;
        sf::Vector2f m_cameraSize;
        float m_cameraZoom;

        bool m_swindowFocused;
        sf::Vector2f m_swindowpos;
        sf::Vector2f m_swindowsize;
        sf::Vector2f m_sceneMousePos;
        bool m_sceneMouseCapture;
        GameObject* m_viewObject;

        // Engine Variables
        sf::Clock m_clock;
        sf::Vector2f m_mousePos;
        sf::Time m_lastFixedUpdate;
        unsigned int m_frameLimit;
        sf::Time m_TimePerFrame;
        std::unordered_map<sf::Keyboard::Key, bool> m_keypresses;

        // UI Variables
        bool m_showDockSpace;
        bool m_cameraMove;
        bool m_showEntityCreate;
        bool m_showMainMenu;
        bool m_showEntityList;
        bool m_showConsole;
        bool m_showSettingsMenu;
        bool m_showTextCreate;
        bool m_showCameraCreate;
        std::string m_failedMessage;
        GameObject* m_currentSelectedObject;
        GameObject* m_copyObject;
        bool m_ConsoleEngineMessages = true;
        int m_DebugIntInput = 0;

        Entity* m_AddAttributeEntity;
        char m_AddScriptName[BUFFSIZE];
        char m_AddVariableName[BUFFSIZE];
        int m_AddVariableType;
        double m_AddVariableNumber;
        char m_AddVariableString[BUFFSIZE];
        float m_HitboxAdjust;

        char m_PrefabPath[BUFFSIZE];

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
