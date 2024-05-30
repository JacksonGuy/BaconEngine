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
    std::vector<Entity*> showDetails;
    float color[] = {0.5f, 0.5f, 0.5f};
    sf::Vector2f mousePos;
    bool cameraMove = false;

    // Editor Tools
    bool panMode = true;
    bool selectMode = false;

    // GameManager Things
    GameManager::LoadFont("./assets/fonts/arial.ttf"); 
    
    while (window.isOpen())
    {
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

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                    ImGui::MenuItem("Save");
                    ImGui::MenuItem("Save as");
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::Begin("Game Details");
            if (ImGui::Button("This is a button")) {
                std::cout << "[DEBUG] Player Position: " << player.position.x << ", " << player.position.y << std::endl;
                std::cout << "[DEBUG] Entity Name: " << player.name << std::endl;
            }
        ImGui::End();

        for (Entity* e : showDetails) {
            std::string name = "Details (ID: " + std::to_string(e->ID) + ")";
            ImGui::Begin(name.c_str());
                ImGui::Text("ID: %d", e->ID);

                char nameBuff[32];
                strcat(nameBuff, e->name.c_str());
                if (ImGui::InputText("Name", nameBuff, 32)) {
                    e->name = nameBuff;
                }
                
                float e_pos[] = {e->position.x, e->position.y};
                if (ImGui::InputFloat2("Position", e_pos)) {
                    e->SetPosition({e_pos[0], e_pos[1]});
                }

                float e_scale[] = {e->scale.y, e->scale.y};
                if (ImGui::InputFloat2("Scale", e_scale)) {
                    e->SetSpriteScale({e_scale[0], e_scale[1]});
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
