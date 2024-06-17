#include <iostream>
#include <fstream>

#include "File.hpp"
#include "GameManager.hpp"
#include "json.hpp"

using json = nlohmann::json;

void save(std::string filename) {
    std::ofstream outfile("./Projects/" + filename);
    json level_data;

    level_data["Version"] = GameManager::config.version;

    for (Entity* e : GameManager::Entities) {
        level_data["Entities"][e->ID] = {
            {"id", e->ID},
            {"name", e->name},
            {"texturePath", e->texturePath},
            {"position", {e->position.x, e->position.y}},
            {"scale", {e->scale.x, e->scale.y}},
            {"width", e->width},
            {"height", e->height},
            {"rotation", e->rotation},
            {"isPlayer", e->isPlayer},
            {"isSolid", e->isSolid},
            {"physicsObject", e->physicsObject},
            {"hitboxSize", e->hitboxSize},
            {"speed", e->speed},
            {"mass", e->mass}
        };
    }

    level_data["Settings"]["Gravity"] = GameManager::gravity;
    level_data["Settings"]["InputMode"] = GameManager::PlayerInputMode;

    outfile << std::setw(4) << level_data;
}

bool load(std::string filename) {
    std::string projectName = "./Projects/" + filename;
    //std::cout << "[DEBUG] Loading Project " << projectName << std::endl;
    GameManager::ConsoleWrite("[DEBUG] Loading Project: " + projectName);

    std::ifstream infile(projectName);
    if (!infile.is_open()) {
        //std::cout << "[ERROR] Project " << filename << " does not exist" << std::endl;
        GameManager::ConsoleWrite("[ERROR] Failed to load project (Project doesn't exist)");
        return false;
    }

    json level_data = json::parse(infile);

    GameManager::ConsoleWrite("[DEBUG] Creating Entities...");
    for (auto& entity : level_data["Entities"]) {
        Entity* e = new Entity(sf::Vector2f(entity["position"][0], entity["position"][1]));
        e->name = entity["name"];
        e->scale = sf::Vector2f(entity["scale"][0], entity["scale"][1]);
        e->width = entity["width"];
        e->height = entity["height"];
        e->rotation = entity["rotation"];
        e->UpdateRect();
        e->SetSprite(entity["texturePath"], false);
        e->isPlayer = entity["isPlayer"];
        if (e->isPlayer) {
            GameManager::player = e;
        }
        e->isSolid = entity["isSolid"];
        e->physicsObject = entity["physicsObject"];
        e->hitboxSize = entity["hitboxSize"];
        e->speed = entity["speed"];
        e->mass = entity["mass"];
    }

    GameManager::gravity = level_data["Settings"]["Gravity"];
    GameManager::PlayerInputMode = level_data["Settings"]["InputMode"];

    return true;
}

void saveConfig(ConfigState state) {
    std::ofstream outfile("config.json");
    json settings;

    settings["version"] = state.version;
    settings["resolution"] = state.resolution;

    outfile << std::setw(4) << settings;

    GameManager::config = state;
}

ConfigState loadConfig() {
    std::ifstream infile("config.json");
    if (!infile.is_open()) {
        GameManager::ConsoleWrite("[ERROR] Failed to load editor settings");
        return (ConfigState){"0","800x600"};
    }

    json settings = json::parse(infile);
    ConfigState state = {
        settings["version"],
        settings["resolution"]
    };

    GameManager::config = state;
    return state;
}