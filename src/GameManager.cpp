#include "GameManager.h"
#include <iostream>

std::map<std::string, sf::Texture*> GameManager::Textures;
std::vector<Entity*> GameManager::Entities;

sf::Texture* GameManager::LoadTexture(std::string path) {
    sf::Texture* texture = new sf::Texture();
    if (!texture->loadFromFile(path)) {
        std::cout << "[ERROR] Couldn't load texture: " << path << std::endl;
    }
    Textures[path] = texture;
    return texture;
}

void GameManager::DrawEntities(sf::RenderWindow& window) {
    for (Entity* e : Entities) {
        window.draw(e->sprite);
    }
}

sf::Vector2f GameManager::ConvertMouseCoords(sf::Vector2f mousePos, sf::View& view) {
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f adjusted = viewCenter - mousePos;
}