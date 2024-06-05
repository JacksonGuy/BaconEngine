#include <iostream>

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "src/GameManager.hpp"
#include "src/Entity.hpp"
#include "src/File.hpp" 

int main() {
    GameManager::screenWidth = 1280;
    GameManager::screenHeight = 720;

    std::string projectTitle = "Untitled Project";
    bool loadedProject = false;

    sf::RenderWindow window(sf::VideoMode(GameManager::screenWidth, GameManager::screenHeight), "Bacon - " + projectTitle);
    if (!ImGui::SFML::Init(window)) {
        std::cout << "[ERROR] Failed to initialize ImGui\n";
        return -1;
    }

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
                if (event.mouseWheel.delta == 1) {
                    if (cameraZoom > 1) cameraZoom = 1;
                    cameraZoom -= 0.1f;
                }
                else if (event.mouseWheel.delta == -1) {
                    if (cameraZoom < 1) cameraZoom = 1;
                    cameraZoom += 0.1f;
                }
                camera.zoom(cameraZoom);
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
                        std::cout << "[DEBUG] Successfully Loaded Project" << std::endl;
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
                    window.setTitle("Bacon - " + std::string(saveAsProjectname));
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
                ImGui::Text("Version 0.1a");
                ImGui::Text( ("FPS: " + std::to_string((int)fps)).c_str() );
                if (ImGui::Button("This is a button")) {
                    for (Entity* e : GameManager::Entities) {
                        std::cout << "[DEBUG] Entity Name: " << e->name << std::endl;
                    }
                }
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
                        std::cout << "[DEBUG] Set Player" << std::endl;
                    }
                    else {
                        GameManager::player = nullptr;
                        std::cout << "[DEBUG] Unset Player" << std::endl;
                    }
                }
                if (GameManager::player != nullptr && GameManager::player != e) {
                    ImGui::EndDisabled();
                }

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
                    std::cout << "[DEBUG] Deleted Entity" << std::endl;
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
                    std::cout << "[DEBUG] Created new entity" << std::endl;
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

        window.clear(sf::Color(40, 40, 40));
        window.draw(originDot);

        GameManager::DrawEntities(window);

        ImGui::SFML::Render(window);
        window.display();
    }

    window.close();
    ImGui::SFML::Shutdown();
}
