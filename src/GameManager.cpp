#include "GameManager.hpp"
#include <iostream>

std::map<std::string, sf::Texture*> GameManager::Textures;
std::vector<Entity*> GameManager::Entities;
sf::Font GameManager::font;
unsigned int GameManager::screenWidth;
unsigned int GameManager::screenHeight;
Entity* GameManager::player = nullptr;
bool GameManager::isPlayingGame;
EditorSaveState GameManager::saveState;
ImGuiTextBuffer GameManager::ConsoleLog;

sf::Texture* GameManager::LoadTexture(std::string path) {
    if (Textures.find(path) == Textures.end()) {
        //std::cout << "[DEBUG] Loading New Texture" << std::endl;
        GameManager::ConsoleWrite("[DEBUG] Loading new texture...");
        sf::Texture* texture = new sf::Texture();
        if (!texture->loadFromFile(path)) {
            //std::cout << "[ERROR] Couldn't load texture: " << path << std::endl;
            GameManager::ConsoleWrite("[ERROR] Couldn't load texture:" + path);
        }
        Textures[path] = texture;
    } 
    return Textures[path];
}

void GameManager::LoadFont(std::string path) {
    sf::Font font;
    if (!font.loadFromFile(path)) {
        //std::cout << "[ERROR] Failed to load font: " << path << std::endl;
        GameManager::ConsoleWrite("[ERROR] Failed to load font: " + path);
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

bool GameManager::MouseOnEntity(sf::Vector2f mousePos) {
    for (Entity* e : GameManager::Entities) {
        if (e->rect.contains(mousePos.x, mousePos.y)) {
            return true;
        }
    }
    return false;
}

void GameManager::SaveEditorState(sf::RenderWindow& window) {
    // Clear saveState
    for (Entity* e : saveState.Entities) {
        delete e;
    }
    saveState.Entities.clear();

    // Copy entities 
    for (Entity* e : GameManager::Entities) {
        Entity* copy = new Entity(*e);
        saveState.Entities.push_back(copy);
    }
    saveState.CameraPos = window.getView().getCenter();
}

void GameManager::RestoreEditorState(sf::RenderWindow& window) {
    for (Entity* e : GameManager::Entities) {
        delete e;
    } 
    GameManager::Entities.clear();

    for (Entity* e : saveState.Entities) {
        Entity* copy = new Entity(*e);
        if (copy->isPlayer) {
            GameManager::player = copy;
        }
        GameManager::Entities.push_back(copy);
    }
    sf::View camera(saveState.CameraPos, sf::Vector2f(screenWidth, screenHeight));
    window.setView(camera);
}

void GameManager::ConsoleWrite(std::string text) {
    std::string newText = text + "\n";
    GameManager::ConsoleLog.append(newText.c_str());
}