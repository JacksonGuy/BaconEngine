#include "GameManager.hpp"
#include <iostream>

std::map<std::string, sf::Texture*> GameManager::Textures;
std::vector<Entity*> GameManager::Entities;
sf::Font GameManager::font;
unsigned int GameManager::screenWidth;
unsigned int GameManager::screenHeight;
Entity* GameManager::player = nullptr;


sf::Texture* GameManager::LoadTexture(std::string path) {
    if (Textures.find(path) == Textures.end()) {
        std::cout << "[DEBUG] Loading New Texture" << std::endl;
        sf::Texture* texture = new sf::Texture();
        if (!texture->loadFromFile(path)) {
            std::cout << "[ERROR] Couldn't load texture: " << path << std::endl;
        }
        Textures[path] = texture;
    } 
    return Textures[path];
}

void GameManager::LoadFont(std::string path) {
    sf::Font font;
    if (!font.loadFromFile(path)) {
        std::cout << "[ERROR] Failed to load font: " << path << std::endl;
    }
    GameManager::font = font; 
}

void GameManager::DrawEntities(sf::RenderWindow& window) {
    for (Entity* e : Entities) {
        window.draw(e->sprite);
    }
}

void GameManager::DrawText(sf::RenderWindow& window, std::string text, sf::Vector2f position) {
    sf::Text renderText;
    renderText.setFont(GameManager::font);
    renderText.setString(text);
    renderText.setPosition(position);

    // TODO other rendering options (size, color)
    renderText.setCharacterSize(24);
    renderText.setColor(sf::Color::Black);

    window.draw(renderText);
}

void GameManager::DrawScreenText(sf::RenderWindow& window, sf::View& view, std::string text, sf::Vector2f position) {
    // Adjust for View offset
    sf::Vector2f center = view.getCenter();
    sf::Vector2f corner = {center.x - (GameManager::screenWidth/2), center.y - (GameManager::screenHeight/2)};
    sf::Vector2f adjusted = corner + position;

    GameManager::DrawText(window, text, adjusted);
}  

sf::Vector2f GameManager::ConvertMouseCoords(sf::Vector2f mousePos, sf::View& view) {
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f adjusted = viewCenter - mousePos;
}

bool GameManager::MouseOnEntity(sf::Vector2f mousePos) {
    for (Entity* e : GameManager::Entities) {
        if (e->rect.contains(mousePos.x, mousePos.y)) {
            return true;
        }
    }
    return false;
}