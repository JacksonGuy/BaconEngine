#include <iostream>

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "src/GameManager.hpp"
#include "src/Entity.hpp"
#include "src/File.hpp" 

namespace Settings {
    int selectedResolution = 2;
    const int resolutionsCount = 6;
    std::string EngineVersion = "";
    const char* resolutions[] = {
        "640x480",
        "800x600",
        "1280x720",
        "1440x900",
        "1600x900",
        "1920x1080"
    };
    std::map<std::string, sf::Vector2u> resolutionConvert = {
        {"640x480", sf::Vector2u(640, 480)},
        {"800x600", sf::Vector2u(800, 600)},
        {"1280x720", sf::Vector2u(1280, 720)},
        {"1440x900", sf::Vector2u(1440, 900)},
        {"1600x900", sf::Vector2u(1600, 900)},
        {"1920x1080", sf::Vector2u(1920,1080)}
    };
    
    sf::Vector2u GetCurrentResolution() {
        std::string setting = Settings::resolutions[Settings::selectedResolution];
        return Settings::resolutionConvert[setting];
    }

    std::string GetResolutionString() {
        return Settings::resolutions[Settings::selectedResolution];
    }

    void ApplySettings(ConfigState state) {
        for (int i = 0; i < Settings::resolutionsCount; i++) {
            if (Settings::resolutions[i] == state.resolution) {
                selectedResolution = i;
                break;
            }
        }
        Settings::EngineVersion = state.version;
    }

    void ChangeResolution(sf::RenderWindow& window) {
        sf::Vector2u size = Settings::GetCurrentResolution();
        window.setSize(size);
        GameManager::screenWidth = size.x;
        GameManager::screenHeight = size.y;
    }
}

int main() {
    std::string projectTitle = "Untitled Project";
    bool loadedProject = false;

    sf::RenderWindow window(sf::VideoMode(GameManager::screenWidth, GameManager::screenHeight), "Bacon - " + projectTitle);
    if (!ImGui::SFML::Init(window)) {
        std::cout << "[ERROR] Failed to initialize ImGui\n";
        return -1;
    }

    // Load Editor Settings
    ConfigState settings = loadConfig();
    Settings::ApplySettings(settings);
    Settings::ChangeResolution(window);

    // Camera View
    sf::View camera(sf::Vector2f(0,0), sf::Vector2f(GameManager::screenWidth, GameManager::screenHeight));
    window.setView(camera);
    float cameraZoom = 1.0f;

    // Create Entities
    sf::CircleShape originDot(5);
    originDot.setFillColor(sf::Color::White);
    originDot.setPosition(0.0f, 0.0f);

    // Engine UI variables
    sf::Clock deltaClock;
    sf::Clock fpsClock;
    sf::Vector2f mousePos;
    bool cameraMove = false;
    bool showEntityCreate = false;
    bool showTestMenu = true;
    bool entitySelect = false;
    bool showEntityList = false;
    bool showConsole = false;
    bool showSettingsMenu = false;

    bool showFailedPopup = false;
    std::string failedMessage;

    // Save/Load Project Variables
    bool showLoadPopup = false;
    char loadProjectName[64] = {0};
    bool showSaveAsPopup = false;
    char saveAsProjectname[64];

    // Create Entity Variables
    char createNameBuffer[32] = "Entity";
    float createPosition[] = {0.0f, 0.0f};
    char createImagePath[32] = "";

    // GameManager Things
    GameManager::LoadFont("./assets/fonts/arial.ttf");

    // Player Things
    bool playerExists = false; 
    
    // DEBUG
    load("Game.json");

    while (window.isOpen())
    {
        float currentTime = fpsClock.restart().asSeconds();
        float fps = 1.0f / currentTime;

        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(window, event);
            auto& io = ImGui::GetIO(); 

            if (event.type == sf::Event::Closed)
                window.close();

            // Input
            if (event.type == sf::Event::MouseButtonPressed) {
                // Left Mouse
                if (event.mouseButton.button == 0 && !io.WantCaptureMouse) {
                    // Camera drag move toggle
                    // Get starting position as reference for movement
                    mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    cameraMove = true;
                }

                // Right Mouse
                else if (event.mouseButton.button == 1) {
                    // Check if we right clicked an entity
                    sf::Vector2f converted = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    for (Entity* e : GameManager::Entities) {
                        if (e->rect.contains(converted.x, converted.y)) {
                            // Toggle visability
                            e->showDetailMenu = !e->showDetailMenu;
                        }
                    }
                }
            }
            
            if (event.type == sf::Event::MouseButtonReleased) {
                // Untoggle camera move
                cameraMove = false;
            }

            if (event.type == sf::Event::MouseMoved) {
                // Only move while holding down left mouse
                if (!cameraMove) {
                    break;
                }
                // Calculate change
                sf::Vector2f currentPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::Vector2f change = (sf::Vector2f)(mousePos - currentPos);

                // Update position
                camera.setCenter(camera.getCenter() + change);
                window.setView(camera);
            }

            // Mouse Wheel
            if (event.type == sf::Event::MouseWheelMoved) {
                // Mouse wheel moved forwards
                if (event.mouseWheel.delta == 1) {
                    camera.zoom(0.8);
                    cameraZoom *= 1.2;
                }
                // Mouse wheel moved back
                else if (event.mouseWheel.delta == -1) {
                    camera.zoom(1.2);
                    cameraZoom *= 0.8;
                }
                window.setView(camera);
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // Top menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Save")) {
                        if (!loadedProject) {
                            showSaveAsPopup = true;
                        }
                        else {
                            save(projectTitle);
                        }
                    }
                    
                    if (ImGui::MenuItem("Save as")) {
                        showSaveAsPopup = true;
                    }
                    
                    if (ImGui::MenuItem("Load")) {
                        showLoadPopup = true;
                    }

                    // Popups
                    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Create")) {
                    ImGui::MenuItem("Entity", NULL, &showEntityCreate);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Entity List", NULL, &showEntityList));
                if (ImGui::MenuItem("Test Menu", NULL, &showTestMenu));
                if (ImGui::MenuItem("Console", NULL, &showConsole));
                if (ImGui::MenuItem("Editor Settings", NULL, &showSettingsMenu));
                ImGui::EndMenu();
            }

            if (showLoadPopup) {
                ImGui::OpenPopup("Load Project");
            }

            if (showSaveAsPopup) {
                ImGui::OpenPopup("Save As Project");
            }

            if (showFailedPopup) {
                ImGui::OpenPopup("Failed");
            }

            if (ImGui::BeginPopupModal("Load Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Load Project");
                ImGui::Separator();
                ImGui::InputText("##", loadProjectName, 64);
                if (ImGui::Button("Load")) {
                    ImGui::CloseCurrentPopup();
                    showLoadPopup = false;
                    if (load(loadProjectName)) {
                        //std::cout << "[DEBUG] Successfully Loaded Project" << std::endl;
                        GameManager::ConsoleWrite("[DEBUG] Successfully Loaded Project");
                        loadedProject = true;
                        projectTitle = loadProjectName;
                        window.setTitle("Bacon - " + projectTitle);
                    }
                    else {
                        showFailedPopup = true;
                        failedMessage = "Error: Failed to load project (Project doesn't exist).";
                        showLoadPopup = false;
                        ImGui::CloseCurrentPopup();
                    }
                }
                if (ImGui::Button("Cancel")) {
                    showLoadPopup = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal("Save As Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Save Project");
                ImGui::Separator();
                ImGui::InputText("##", saveAsProjectname, 64);
                if (ImGui::Button("Save")) {
                    ImGui::CloseCurrentPopup();
                    showSaveAsPopup = false;

                    save(saveAsProjectname);
                    projectTitle = saveAsProjectname;
                    loadedProject = true;
                    window.setTitle("Bacon - " + projectTitle);
                }
                if (ImGui::Button("Cancel")) {
                    showSaveAsPopup = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal("Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text(failedMessage.c_str());
                if (ImGui::Button("OK")) {
                    showFailedPopup = false;
                    failedMessage = "";
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::EndMainMenuBar();
        }

        // General Test Menu
        if (showTestMenu) {
            ImGui::Begin("Game Details", &showTestMenu);
                ImGui::Text(Settings::EngineVersion.c_str());
                ImGui::Text( ("FPS: " + std::to_string((int)fps)).c_str() );
                if (ImGui::Button("This is a button")) {
                    GameManager::ConsoleWrite("This is some text");
                }
                if (!GameManager::isPlayingGame) {
                    if (ImGui::Button("Play Game")) {
                        GameManager::SaveEditorState(window);
                        //std::cout << "[PLAYER] Editor data saved. Starting game..." << std::endl;
                        GameManager::ConsoleWrite("[PLAYER] Editor data saved. Starting game...");

                        GameManager::isPlayingGame = true;
                    }
                }
                else {
                    if (ImGui::Button("End Game")) {
                        GameManager::RestoreEditorState(window);
                        //std::cout << "[PLAYER] Game Ended. Editor data restored." << std::endl;
                        GameManager::ConsoleWrite("[PLAYER] Game Ended. Editor data restored.");
                        GameManager::isPlayingGame = false;
                    }
                }
                ImGui::Separator();
                ImGui::BeginTabBar("EngineItems");
                    ImGui::BeginTabItem("Camera");
                        float cameraPos[] = {camera.getCenter().x, camera.getCenter().y};
                        ImGui::Text("Position");
                        if (ImGui::InputFloat2("##", cameraPos)) {
                            camera.setCenter(sf::Vector2f(cameraPos[0], cameraPos[1]));
                            window.setView(camera);
                        }

                        float oldZoom = cameraZoom;
                        ImGui::Text("Zoom");
                        if (ImGui::InputFloat("##", &cameraZoom)) {
                            float zoomFactor = oldZoom / cameraZoom;
                            camera.zoom(zoomFactor);
                            window.setView(camera);
                        }
                    ImGui::EndTabItem();
                ImGui::EndTabBar();

            ImGui::End();
        }

        // Entity detail menus
        for (size_t i = 0; i < GameManager::Entities.size(); i++) {
            Entity* e = GameManager::Entities[i];

            if (!e->showDetailMenu) continue;
            std::string name = "Details (ID: " + std::to_string(e->ID) + ")";
            ImGui::Begin(name.c_str(), &(e->showDetailMenu));
                ImGui::Text("ID: %d", e->ID);

                ImGui::InputText("Name", e->name.data(), sizeof(e->name.data()));

                if (GameManager::player != nullptr && GameManager::player != e) {
                    ImGui::BeginDisabled();
                }
                if (ImGui::Checkbox("Player", &e->isPlayer)) {
                    if (GameManager::player == nullptr) {
                        GameManager::player = e;
                    }
                    else {
                        GameManager::player = nullptr;
                    }
                }
                if (GameManager::player != nullptr && GameManager::player != e) {
                    ImGui::EndDisabled();
                }

                ImGui::Checkbox("Solid", &e->isSolid);
                ImGui::SameLine();
                ImGui::Checkbox("Physics Object", &e->physicsObject);

                ImGui::Separator();
                ImGui::InputText("Texture", e->texturePath.data(), 64);
                if (ImGui::Button("Change Texture")) {
                    e->SetSprite(e->texturePath);
                }
                ImGui::Separator();
                
                // Temporary Conversion to float array for input
                float e_pos[] = {e->position.x, e->position.y};
                if (ImGui::InputFloat2("Position", e_pos)) {
                    e->SetPosition({e_pos[0], e_pos[1]});
                }

                if (ImGui::InputInt("Width", &e->width)) {
                    e->UpdateRect();
                    e->SetSprite(e->texturePath);
                }
                if (ImGui::InputInt("Height", &e->height)) {
                    e->UpdateRect();
                    e->SetSprite(e->texturePath);
                }

                if (ImGui::InputFloat("Rotation", &e->rotation)) {
                    // Adjust rotation angle if necessary
                    if (e->rotation > 360) {
                        e->rotation -= 360;
                    }
                    if (e->rotation < -360) {
                        e->rotation += 360;
                    }

                    // Rotate
                    e->sprite.setRotation(e->rotation);
                    e->UpdateRect();
                }

                if (ImGui::Button("Delete")) {
                    free(GameManager::Entities[i]);
                    GameManager::Entities.erase(GameManager::Entities.begin() + i);
                }
            ImGui::End();
        }

        if (showEntityCreate) {
            ImGui::Begin("Create new entity", &showEntityCreate);
                // Displays
                ImGui::InputText("Name", createNameBuffer, 32);
                ImGui::InputFloat2("Position", createPosition);
                ImGui::InputText("Image Path", createImagePath, 32);

                // Create Button
                if (ImGui::Button("Create")) {
                    Entity* entity = new Entity({createPosition[0], createPosition[1]});
                    entity->name = createNameBuffer;
                    entity->SetSprite(createImagePath);
                }
            ImGui::End();
        }

        if (showEntityList) {
            ImGui::Begin("Entities", &showEntityList);
                auto flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                for (Entity* e : GameManager::Entities) {
                    ImGui::TreeNodeEx(e->name.data(), flags);
                    if (ImGui::IsItemClicked()) {
                        e->showDetailMenu = !e->showDetailMenu;
                    }
                }
            ImGui::End();
        }

        if (showConsole) {
            ImGui::Begin("Console", &showConsole);
                if (ImGui::Button("Clear")) {
                    GameManager::ConsoleLog.clear();
                }
                ImGui::Separator();
                ImGui::TextUnformatted(GameManager::ConsoleLog.begin(), GameManager::ConsoleLog.end());
            ImGui::End();
        }

        if (showSettingsMenu) {
            ImGui::Begin("Settings", &showSettingsMenu);
                ImGui::BeginTabBar("SettingsMenus", ImGuiTabBarFlags_None);
                    ImGui::BeginTabItem("Display");
                        ImGui::Text("Resolution");
                        const char* previewValue = Settings::resolutions[Settings::selectedResolution];
                        if (ImGui::BeginCombo("##", previewValue)) {
                            for (int i = 0; i < Settings::resolutionsCount; i++) {
                                const bool isSelected = (Settings::selectedResolution == i);
                                if (ImGui::Selectable(Settings::resolutions[i], isSelected)) {
                                    Settings::selectedResolution = i;
                                    Settings::ChangeResolution(window);
                                }

                                if (isSelected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }
                    ImGui::EndTabItem();
                ImGui::EndTabBar();
            ImGui::End();
        }

        // Player Input
        float speed = 0.1f;
        if (GameManager::isPlayingGame && GameManager::player != nullptr) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                if (!GameManager::checkCollision(*GameManager::player)) { 
                    GameManager::player->position.y -= speed;
                }
                else {
                    GameManager::player->position.y += 0.1;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                if (!GameManager::checkCollision(*GameManager::player)) { 
                    GameManager::player->position.y += speed;
                }
                else {
                    GameManager::player->position.y -= 0.1;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                if (!GameManager::checkCollision(*GameManager::player)) { 
                    GameManager::player->position.x -= speed;
                }
                else {
                    GameManager::player->position.x += 0.1;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                if (!GameManager::checkCollision(*GameManager::player)) { 
                    GameManager::player->position.x += speed;
                }
                else {
                    GameManager::player->position.x -= 0.1;
                }
            }

            GameManager::player->UpdateRect();
            GameManager::player->sprite.setPosition(GameManager::player->position);
            camera.setCenter(GameManager::player->position);
            window.setView(camera);
        }

        // Editor
        window.clear(sf::Color(40, 40, 40));

        if (!GameManager::isPlayingGame) {
            window.draw(originDot);
        }
        GameManager::DrawEntities(window);

        ImGui::SFML::Render(window);
        window.display();
    }

    ConfigState finalSettings = {
        Settings::EngineVersion,
        Settings::GetResolutionString()
    };
    saveConfig(finalSettings);
    std::cout << "Saved Editor Settings" << std::endl;
    
    window.close();
    ImGui::SFML::Shutdown();
}