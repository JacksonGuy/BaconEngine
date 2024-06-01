#include <iostream>

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "src/GameManager.h"
#include "src/Entity.h" 

int main() {
    GameManager::screenWidth = 1280;
    GameManager::screenHeight = 720;

    sf::RenderWindow window(sf::VideoMode(GameManager::screenWidth, GameManager::screenHeight), "Demo");
    if (!ImGui::SFML::Init(window)) {
        std::cout << "[ERROR] Failed to initialize ImGui\n";
        return -1;
    }

    // Camera View
    sf::View camera(sf::Vector2f(0,0), sf::Vector2f(GameManager::screenWidth, GameManager::screenHeight));
    window.setView(camera);

    // Create Entities
    Entity player(sf::Vector2f{0.0f, 0.0f});
    player.SetSprite("./assets/player.jpg");

    // Engine UI variables
    sf::Clock deltaClock;
    sf::Clock fpsClock;
    std::vector<Entity*> showDetails;
    float color[] = {0.5f, 0.5f, 0.5f};
    sf::Vector2f mousePos;
    bool cameraMove = false;
    bool showEntityCreate = false;

    // Create Entity Variables
    char createNameBuffer[32] = "";
    float createPosition[] = {0.0f, 0.0f};
    char createImagePath[32] = "";

    // Editor Tools
    bool panMode = true;
    bool selectMode = false;

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

            if (event.type == sf::Event::Closed)
                window.close();

            // Input
            if (event.type == sf::Event::MouseButtonPressed) {
                // Left Mouse
                if (event.mouseButton.button == 0) {
                    if (panMode) {
                        // Camera drag move toggle
                        // Get starting position as reference for movement
                        mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                        cameraMove = true;
                    }
                }
                // Right Mouse
                else if (event.mouseButton.button == 1) {
                    // Check each entity to see if we right clicked it
                    sf::Vector2f converted = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    for (Entity* e : GameManager::Entities) {
                        bool selected = false;
                        if (e->rect.contains(converted.x, converted.y)) {
                            // Check if there's already a window for the entity
                            for (Entity* other : showDetails) {
                                if (e->ID == other->ID) {
                                    selected = true;
                                    break;
                                }
                            }
                            // Don't open another window if there's already one 
                            if (!selected) showDetails.push_back(e);
                        }                        
                    }
                }
            }
            
            if (event.type == sf::Event::MouseButtonReleased) {
                if (panMode) {
                    // Untoggle camera move
                    cameraMove = false;
                }
            }

            if (event.type == sf::Event::MouseMoved) {
                if (panMode) {
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
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
            panMode = true;
            selectMode = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            selectMode = true;
            panMode = false;
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // Top menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                    ImGui::MenuItem("Save");
                    ImGui::MenuItem("Save as");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Create")) {
                    ImGui::MenuItem("Entity", NULL, &showEntityCreate);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // General Test Menu
        ImGui::Begin("Game Details");
            ImGui::Text("Version 0.1a");
            ImGui::Text( ("FPS: " + std::to_string((int)fps)).c_str() );
            if (ImGui::Button("This is a button")) {
                for (Entity* e : GameManager::Entities) {
                    std::cout << "[DEBUG] Scale: " << e->scale.x << ", " << e->scale.y << std::endl; 
                }
            }
        ImGui::End();

        // Entity detail menus
        for (Entity* e : showDetails) {
            std::string name = "Details (ID: " + std::to_string(e->ID) + ")";
            ImGui::Begin(name.c_str());
                ImGui::Text("ID: %d", e->ID);

                char nameBuff[32];
                strcpy(nameBuff, e->name.data());
                if (ImGui::InputText("Name", nameBuff, sizeof(nameBuff))) {
                    e->name = nameBuff;
                }

                char e_textureBuff[64];
                strcpy(e_textureBuff, e->texturePath.data());
                if (ImGui::InputText("Texture", e_textureBuff, 64)) {
                    e->SetSprite(e_textureBuff);
                }
                
                float e_pos[] = {e->position.x, e->position.y};
                if (ImGui::InputFloat2("Position", e_pos)) {
                    e->SetPosition({e_pos[0], e_pos[1]});
                }

                int e_width = e->width;
                int e_height = e->height;
                if (ImGui::InputInt("Width", &e_width)) {
                    e->width = e_width;
                    e->UpdateRect();
                    e->SetSprite(e->texturePath);
                }
                if (ImGui::InputInt("Height", &e_height)) {
                    e->height = e_height;
                    e->UpdateRect();
                    e->SetSprite(e->texturePath);
                }
            ImGui::End();
        }

        if (showEntityCreate) {
            ImGui::Begin("Create new entity");
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

        window.clear(sf::Color(40, 40, 40));

        if (panMode) {
            GameManager::DrawScreenText(window, camera, "Pan", {10.0f, 10.0f});
        }
        else if (selectMode) {
            GameManager::DrawScreenText(window, camera, "Select", {10.0f, 10.0f});
        }

        GameManager::DrawEntities(window);

        ImGui::SFML::Render(window);
        window.display();
    }

    window.close();
    ImGui::SFML::Shutdown();
    return 0;
}
