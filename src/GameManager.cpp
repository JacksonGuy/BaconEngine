#include "GameManager.hpp"
#include <iostream>

ConfigState GameManager::config;
std::map<std::string, sf::Texture*> GameManager::Textures;
std::vector<Entity*> GameManager::Entities;
std::vector<TextObj*> GameManager::TextObjects;
sf::Font GameManager::font;
unsigned int GameManager::screenWidth = 1280;
unsigned int GameManager::screenHeight = 720;
Entity* GameManager::player = nullptr;
bool GameManager::isPlayingGame;
EditorSaveState GameManager::saveState;
ImGuiTextBuffer GameManager::ConsoleLog;
unsigned int GameManager::framerateLimit = 500;
float GameManager::gravity = 0.2f;
unsigned int GameManager::PlayerInputMode = 0;
char* GameManager::InputsModes[2] = {"Top Down", "Platformer"};
lua_State* GameManager::LuaState = luaL_newstate();

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

        if (e->showHitbox) {
            sf::Vector2f pos = e->rect.getPosition();
            sf::Vector2f size = e->rect.getSize();
            float x = pos.x - e->hitboxSize;
            float y = pos.y - e->hitboxSize;
            float width = size.x + (e->hitboxSize * 2);
            float height = size.y + (e->hitboxSize * 2);

            sf::Vertex lines[] = {
                sf::Vertex(sf::Vector2f(x, y), sf::Color::Red),
                sf::Vertex(sf::Vector2f(x + width, y), sf::Color::Red),
                sf::Vertex(sf::Vector2f(x + width, y + height), sf::Color::Red),
                sf::Vertex(sf::Vector2f(x, y + height), sf::Color::Red),
                sf::Vertex(sf::Vector2f(x, y), sf::Color::Red)
            };

            window.draw(lines, 5, sf::LinesStrip);
        }
    }
}

void GameManager::DrawText(sf::RenderWindow& window) {
    for (TextObj* text : GameManager::TextObjects) {
        // Draw text relative to some entity
        // (0,0) is the entity position
        if (text->mode == Relative) {
            if (text->target != nullptr) {
                sf::Vector2f pos = text->position;
                pos += text->target->position;
                text->text.setPosition(pos);
            }
            else {
                std::cout << "Text Name: " << text->name << std::endl;
                std::cout << "Text ID: " << text->ID << std::endl;
                std::cout << "Text Target: " << text->target_id << std::endl;
            }
        }

        // Draw text relative to the screen
        // (0,0) is the top left corner of the camera
        else if (text->mode == Screen) {
            sf::View cam = window.getView();
            sf::FloatRect rect = cam.getViewport();
            sf::Vector2f point = sf::Vector2f(text->position.x + rect.left, text->position.y + rect.top);
            point = window.mapPixelToCoords((sf::Vector2i)point);
            text->text.setPosition(point);
        }

        window.draw(text->text);
    }
}

bool GameManager::MouseOnEntity(sf::Vector2f mousePos) {
    for (Entity* e : GameManager::Entities) {
        if (e->rect.contains(mousePos.x, mousePos.y)) {
            return true;
        }
    }
    return false;
}

// Check for collision between two specific Entities
bool GameManager::checkCollision(const Entity& e1, const Entity& e2) {
    if (!e1.isSolid || !e2.isSolid) return false;
    return e1.rect.intersects(e2.rect);
}

// Check if an object is colliding with any entity
bool GameManager::checkCollision(const Entity& e1) {
    if (!e1.isSolid) return false;
    for (Entity* e2 : GameManager::Entities) {
        if (!e2->isSolid) continue;
        if (e1.ID == e2->ID) continue;
        if (e1.rect.intersects(e2->rect)) {
            return true;
        }
    }
    return false;
}

// Get a list of entities that the object is colliding with
std::vector<Entity*> GameManager::getCollidingWith(const Entity& e1) {
    std::vector<Entity*> collidingWith;
    
    if (!e1.isSolid) return collidingWith;

    for (Entity* e2 : GameManager::Entities) {
        if (!e2->isSolid) continue;
        if (e1.ID == e2->ID) continue;
        if (e1.rect.intersects(e2->rect)) {
            collidingWith.push_back(e2);
        }
    }
    return collidingWith;
}

bool GameManager::checkCollisionSide(const sf::Rect<float> side) {
    for (Entity* e : GameManager::Entities) {
        if (e->ID == GameManager::player->ID) continue;
        if (!e->isSolid) continue;
        if (side.intersects(e->rect)) {
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
    for (TextObj* text : saveState.TextObjects) {
        delete text;
    }
    saveState.Entities.clear();
    saveState.TextObjects.clear();

    // Copy entities 
    for (Entity* e : GameManager::Entities) {
        Entity* copy = new Entity(*e);
        saveState.Entities.push_back(copy);
    }
    for (TextObj* text : GameManager::TextObjects) {
        TextObj* copy = new TextObj(*text);
        saveState.TextObjects.push_back(copy);
    }
    saveState.CameraPos = window.getView().getCenter();
    saveState.CameraSize = window.getView().getSize();
}

void GameManager::RestoreEditorState(sf::RenderWindow& window) {
    for (Entity* e : GameManager::Entities) {
        delete e;
    }
    for (TextObj* text : GameManager::TextObjects) {
        delete text;
    } 
    GameManager::Entities.clear();
    GameManager::TextObjects.clear();

    for (Entity* e : saveState.Entities) {
        Entity* copy = new Entity(*e);
        if (copy->isPlayer) {
            GameManager::player = copy;
        }
        GameManager::Entities.push_back(copy);
    }
    for (TextObj* text : saveState.TextObjects) {
        TextObj* copy = new TextObj(*text);
        GameManager::TextObjects.push_back(copy);
    }
    sf::View camera(saveState.CameraPos, saveState.CameraSize);
    window.setView(camera);
}

void GameManager::ConsoleWrite(std::string text) {
    std::string newText = text + "\n";
    GameManager::ConsoleLog.append(newText.c_str());
}

Entity* GameManager::FindEntityByID(int id) {
    for (Entity* e : GameManager::Entities) {
        if (e->ID == id) return e;
    }
    return nullptr;
}

void GameManager::RunLuaUpdates() {
    for (Entity* e : GameManager::Entities) {
        for (ScriptItem script : e->lua_scripts) {
            luaL_dofile(GameManager::LuaState, script.path.c_str());
        }
    }
}