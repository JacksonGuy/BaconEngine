#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "src/GameManager.hpp"
#include "src/File.hpp"

int main() {
    GameManager::screenWidth = 1280;
    GameManager::screenHeight = 720;

    sf::RenderWindow window(sf::VideoMode(GameManager::screenWidth, GameManager::screenHeight), "Bacon Player");

    sf::View camera(sf::Vector2f(0,0), sf::Vector2f(GameManager::screenWidth, GameManager::screenHeight));
    window.setView(camera);

    GameManager::LoadFont("./assets/fonts/arial.ttf");

    load("Game.json");

    bool playerExists = false;
    if (GameManager::player != nullptr) {
        playerExists = true;
        camera.setCenter(GameManager::player->position);
    }
    else {
        std::cout << "[ERROR] No Player Found" << std::endl;
    }
    float speed = 0.1f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Input
        if (playerExists) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                GameManager::player->position.y -= speed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                GameManager::player->position.y += speed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                GameManager::player->position.x -= speed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                GameManager::player->position.x += speed;
            }

            GameManager::player->sprite.setPosition(GameManager::player->position);
            camera.setCenter(GameManager::player->position);
            window.setView(camera);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab)) {
            std::cout << "[DEBUG] Player Position: " << GameManager::player->position.x << ", " << GameManager::player->position.y << std::endl;
        }

        // Draw
        window.clear(sf::Color(40,40,40));
        GameManager::DrawEntities(window);
        window.display();
    }

    window.close();
}
