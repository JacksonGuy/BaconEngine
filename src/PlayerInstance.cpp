#include "PlayerInstance.hpp"

#include <iostream>
#include "GameManager.hpp"
#include "File.hpp"

PlayerInstance::PlayerInstance() {
    this->screenWidth = 1280;
    this->screenHeight = 720;

    this->window = new sf::RenderWindow(sf::VideoMode(screenWidth, screenHeight), "Bacon Player");  
    this->camera = new sf::View(sf::Vector2f(0,0), sf::Vector2f(screenWidth, screenHeight));
    this->window->setView(*this->camera);
    
    GameManager::screenWidth = this->screenWidth;
    GameManager::screenHeight = this->screenHeight;
    GameManager::LoadFont("arial.ttf");
}

bool PlayerInstance::LoadGame(std::string name) {
    bool result = load(name);

    this->playerExists = false;
    if (GameManager::player != nullptr) {
        playerExists = true;
        this->camera->setCenter(GameManager::player->position);
    }
    else {
        std::cout << "[ERROR] No player entity was found" << std::endl;
    }

    return result;
}

void PlayerInstance::Update() {
    sf::Event event;
    while (this->window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            this->window->close();
        }
    }

    // Input
    float speed = 0.1f;
    if (this->playerExists) {
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

        GameManager::player->UpdateRect();
        GameManager::player->sprite.setPosition(GameManager::player->position);
        this->camera->setCenter(GameManager::player->position);
        this->window->setView(*this->camera);
    }

    // Draw
    this->window->clear(sf::Color(40, 40, 40));
    GameManager::DrawEntities(*this->window);
    this->window->display();
}