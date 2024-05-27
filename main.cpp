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

    // Create Entities
    Entity player({20.0f, 20.0f});
    player.SetSprite("./assets/test.png");
    GameManager::Entities.push_back(player);

    sf::Clock deltaClock;
    float color[] = {0.5f, 0.5f, 0.5f};
    float scaleX;
    float scaleY;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed)
                window.close();
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Test Window");
        ImGui::Button("This is a button");
        ImGui::ColorEdit3("Color", color);
        ImGui::SliderFloat("Scale X", &scaleX, 0.0f, 10.0f);
        ImGui::SliderFloat("Scale Y", &scaleY, 0.0f, 10.0f);
        ImGui::End();

        player.sprite.setScale(scaleX, scaleY);

        window.clear(sf::Color(40, 40, 40));
        GameManager::DrawEntities(window);
        ImGui::SFML::Render(window);
        window.display();
    }

    window.close();
    ImGui::SFML::Shutdown();
    return 0;
}