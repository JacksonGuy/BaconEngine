#include <iostream>

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "src/GameManager.h"
#include "src/Entity.h" 

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Demo");
    if (!ImGui::SFML::Init(window)) {
        std::cout << "[ERROR] Failed to initialize ImGui\n";
        return -1;
    }

    // Camera View
    sf::View camera(sf::Vector2f(0,0), sf::Vector2f(800, 600));
    window.setView(camera);

    // Create Entities
    Entity player({20.0f, 20.0f});
    player.SetSprite("./assets/player.jpg");

    // Engine UI variables
    sf::Clock deltaClock;
    std::vector<Entity*> showDetails;
    float color[] = {0.5f, 0.5f, 0.5f};
    sf::Vector2f mousePos;
    bool cameraMove = false; 
    
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
                //std::cout << "[DEBUG] Mouse button " << event.mouseButton.button << " pressed" << std::endl;

                if (event.mouseButton.button == 0) {
                    // Camera drag move toggle
                    // Get starting position as reference for movement
                    mousePos = window.mapPixelToCoords(sf::Mouse::getPosition());
                    cameraMove = true;
                }
                else if (event.mouseButton.button == 1) {
                    sf::Vector2f converted = window.mapPixelToCoords(sf::Mouse::getPosition(), camera);
                    std::cout << "[DEBUG] Mouse Position: " << converted.x << "," << converted.y << std::endl;
                    for (Entity* e : GameManager::Entities) {
                        std::cout << "[DEBUG] Entity Rect: (" << e->rect.top << ","
                            << e->rect.left << ","
                            << e->rect.width << ","
                            << e->rect.height << ")" << std::endl;
                        if (e->rect.contains(converted.x, converted.y)) {
                            std::cout << "[DEBUG] Showing details for Entity" << std::endl;
                            showDetails.push_back(e);
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
                sf::Vector2f currentPos = window.mapPixelToCoords(sf::Mouse::getPosition());
                sf::Vector2f change = (sf::Vector2f)(mousePos - currentPos);

                // Update position
                camera.setCenter(camera.getCenter() + change);
                window.setView(camera);
            }
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
            ImGui::Button("This is a button");
        ImGui::End();

        for (Entity* e : showDetails) {
            ImGui::Begin("Entity Details");
                ImGui::InputFloat("X", &e->position.x);
                ImGui::InputFloat("Y", &e->position.y);
                ImGui::InputFloat("Scale X", &e->scale.x);
                ImGui::InputFloat("Scale Y", &e->scale.y);
            ImGui::End();
        }

        player.UpdateSpriteScale();

        window.clear(sf::Color(40, 40, 40));
        
        GameManager::DrawEntities(window);

        ImGui::SFML::Render(window);
        window.display();
    }

    window.close();
    ImGui::SFML::Shutdown();
    return 0;
}
