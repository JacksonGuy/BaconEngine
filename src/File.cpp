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

    int index = 0;
    for (Entity* e : GameManager::Entities) {
        level_data["Entities"][index] = {
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

        int i = 0;
        for (ScriptItem script : e->lua_scripts) {
            level_data["Entities"][index]["scripts"][i] = script.path;
        }

        index++;
    }

    index = 0;
    for (TextObj* obj : GameManager::TextObjects) {
        int entity_id = -1;
        sf::Color color = obj->text.getFillColor();
        std::string mode;
        switch (obj->mode) {
            case Absolute: 
                mode = "Absolute";
                break;
            case Relative:
                mode = "Relative";
                entity_id = obj->target->ID;
                break;
            case Screen:
                mode = "Screen";
                break;
        }

        level_data["Text"][index++] = {
            {"id", obj->ID},
            {"name", obj->name},
            {"position", {obj->position.x, obj->position.y}},
            {"size", obj->text.getCharacterSize()},
            {"rotation", obj->text.getRotation()},
            {"color", {color.r, color.g, color.b, color.a}},
            {"mode", mode},
            {"entity_id", entity_id},
            {"text", obj->text.getString()}
        };
    }

    level_data["Settings"]["Gravity"] = GameManager::gravity;
    level_data["Settings"]["InputMode"] = GameManager::PlayerInputMode;

    outfile << std::setw(4) << level_data;
}

bool load(std::string filename) {
    std::string projectName = "./Projects/" + filename;
    GameManager::ConsoleWrite("[DEBUG] Loading Project: " + projectName);

    std::ifstream infile(projectName);
    if (!infile.is_open()) {
        GameManager::ConsoleWrite("[ERROR] Failed to load project (Project doesn't exist)");
        return false;
    }

    json level_data = json::parse(infile);

    GameManager::ConsoleWrite("[DEBUG] Creating Entities...");
    for (auto& entity : level_data["Entities"]) {
        Entity* e = new Entity(sf::Vector2f(entity["position"][0], entity["position"][1]));
        e->ID = entity["id"];
        Entity::IDNum = e->ID; // Set to max to preserve IDs
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

        for (json::iterator it = entity["scripts"].begin(); it != entity["scripts"].end(); ++it) {
            ScriptItem script;
            script.path = *it;
            script.showDetails = false;
            e->lua_scripts.push_back(script);
        }
    }

    GameManager::ConsoleWrite("[DEBUG] Creating Text...");
    for (auto& obj : level_data["Text"]) {
        TextObj* text = new TextObj();
        text->ID = obj["id"];
        TextObj::IDNum = text->ID; // Set to max to preserve IDs
        text->name = obj["name"];
        text->position = sf::Vector2f(obj["position"][0], obj["position"][1]);
        text->text.setPosition(text->position);
        text->text.setCharacterSize(obj["size"]);
        text->text.setRotation(obj["rotation"]);
        text->text.setFillColor(sf::Color(
            obj["color"][0], obj["color"][1], obj["color"][2], obj["color"][3])
        );
        std::string str = obj["text"];
        text->text.setString(str);

        text->target = nullptr;
        if (obj["mode"] == "Absolute") text->mode = Absolute;
        else if (obj["mode"] == "Relative") {
            text->mode = Relative;
            text->target_id = obj["entity_id"];
            text->target = GameManager::FindEntityByID(obj["entity_id"]);
        }
        else if (obj["mode"] == "Screen") text->mode = Screen;
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