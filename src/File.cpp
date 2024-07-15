#include <iostream>
#include <fstream>
#include <filesystem>

#include "File.hpp"
#include "GameManager.hpp"
#include "json.hpp"

using json = nlohmann::json;

void save(std::string filename) {
    namespace fs = std::filesystem;
    
    std::ofstream outfile(filename);
    json level_data;

    level_data["Version"] = GameManager::config.version;

    for (size_t index = 0; index < GameManager::Entities.size(); index++) {
        Entity* e = GameManager::Entities[index];
        std::string texturePath = fs::relative(e->texturePath, GameManager::entryPoint).generic_string();
        //texturePath.erase(0, 3);

        level_data["Entities"][index] = {
            {"id", e->ID},
            {"name", e->name},
            {"entity_type", e->entity_type},
            {"texturePath", texturePath},
            {"position", {e->position.x, e->position.y}},
            {"scale", {e->scale.x, e->scale.y}},
            {"width", e->width},
            {"height", e->height},
            {"rotation", e->rotation},
            {"isPlayer", e->isPlayer},
            {"isSolid", e->isSolid},
            {"physicsObject", e->physicsObject},
            {"hitboxSize", e->hitboxSize},
            {"mass", e->mass},
            {"isVisible", e->isVisible}
        };

        int i = 0;
        for (ScriptItem script : e->lua_scripts) {
            std::string scriptPath = fs::relative(script.path, GameManager::entryPoint).generic_string();
            //scriptPath.erase(0, 3);
            level_data["Entities"][index]["scripts"][i++] = scriptPath;
        }

        i = 0;
        for (auto it = e->entity_variables.begin(); it != e->entity_variables.end(); it++) {
            std::string key = it->second;
            if (e->entity_numbers.find(key) != e->entity_numbers.end()) {
                level_data["Entities"][index]["variables"][i++] = {
                    {"name", key},
                    {"type", "number"},
                    {"value", e->entity_numbers[key]}
                };
            }
            else if (e->entity_strings.find(key) != e->entity_strings.end()) {
                level_data["Entities"][index]["variables"][i++] = {
                    {"name", key},
                    {"type", "string"},
                    {"value", e->entity_strings[key]}
                };
            }
        }

        if (e->parent == nullptr) {
            level_data["Entities"][index]["parent"] = -1;
        }
        else {
            level_data["Entities"][index]["parent"] = e->parent->ID;
        }

        i = 0;
        for (Entity* child : e->children) {
            level_data["Entities"][index]["children"][i++] = child->ID;
        }
    }

    for (size_t index = 0; index < GameManager::TextObjects.size(); index++) {
        TextObj* obj = GameManager::TextObjects[index];
        
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

        level_data["Text"][index] = {
            {"id", obj->ID},
            {"name", obj->name},
            {"position", {obj->position.x, obj->position.y}},
            {"size", obj->text.getCharacterSize()},
            {"rotation", obj->text.getRotation()},
            {"color", {color.r, color.g, color.b, color.a}},
            {"mode", mode},
            {"entity_id", entity_id},
            {"text", obj->text.getString()},
            {"isVisible", obj->isVisible}
        };
    }

    level_data["Settings"]["Gravity"] = GameManager::gravity;

    outfile << std::setw(4) << level_data;

    GameManager::ConsoleWrite("[ENGINE] Successfully saved project.");
}

bool load(std::string filename) {
    namespace fs = std::filesystem;

    GameManager::ConsoleWrite("[ENGINE] Loading Project: " + filename);

    std::ifstream infile(filename);
    if (!infile.is_open()) {
        GameManager::ConsoleWrite("[ERROR] Failed to load project (Project doesn't exist)");
        return false;
    }

    fs::path entryPath = fs::path(filename);
    GameManager::entryPoint = entryPath.parent_path().generic_string();
    json level_data = json::parse(infile);

    GameManager::ConsoleWrite("[ENGINE] Creating Entities...");
    for (auto& entity : level_data["Entities"]) {
        Entity* e = new Entity(sf::Vector2f(entity["position"][0], entity["position"][1]));
        e->ID = entity["id"];
        Entity::IDNum = e->ID; // Set to max to preserve IDs
        e->name = entity["name"];
        e->entity_type = entity["entity_type"];
        e->scale = sf::Vector2f(entity["scale"][0], entity["scale"][1]);
        e->width = entity["width"];
        e->height = entity["height"];
        e->rotation = entity["rotation"];
        e->UpdateRect();
        e->SetSprite(toAbsolute(entity["texturePath"]), false);
        e->isPlayer = entity["isPlayer"];
        if (e->isPlayer) {
            GameManager::player = e;
        }
        e->isSolid = entity["isSolid"];
        e->physicsObject = entity["physicsObject"];
        e->hitboxSize = entity["hitboxSize"];
        e->mass = entity["mass"];
        e->isVisible = entity["isVisible"];

        for (json::iterator it = entity["scripts"].begin(); it != entity["scripts"].end(); ++it) {
            ScriptItem script;
            script.path = toAbsolute(*it);
            script.showDetails = false;
            e->lua_scripts.push_back(script);
        }

        for (auto& it : entity["variables"].items()) {
            int order = stoi(it.key());
            std::string name = it.value()["name"];
            std::string type = it.value()["type"];

            e->entity_variables[order] = name;
            if (type == "number") {
                double value = it.value()["value"];
                e->entity_numbers[name] = value;
            }
            else if (type == "string") {
                std::string value = it.value()["value"];
                e->entity_strings[name] = value.data();
            }
        }
    }

    Entity::IDNum++; // For the next Entity we create 

    GameManager::ConsoleWrite("[ENGINE] Constructing Entity Tree...");
    for (auto& entity : level_data["Entities"]) {
        unsigned int childID = entity["id"];
        unsigned int parentID = entity["parent"];
        if (parentID != -1) {
            Entity* child = GameManager::FindEntityByID(childID);
            Entity* parent = GameManager::FindEntityByID(parentID);
            parent->children.push_back(child);
            child->parent = parent;
        }
    }

    GameManager::ConsoleWrite("[ENGINE] Creating Text...");
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
        text->isVisible = obj["isVisible"];
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

    return true;
}

std::string toAbsolute(std::string path) {
    namespace fs = std::filesystem;
    std::string absPath = GameManager::entryPoint + "/" + path;
    return absPath; 
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

void savePrefab(std::string filename, Entity* e) {
    std::ofstream outfile(filename);
    json data;

    data["name"] = e->name;
    data["entity_type"] = e->entity_type;
    data["texturePath"] = e->texturePath;
    data["position"] = {e->position.x, e->position.y};
    data["scale"] = {e->scale.x, e->scale.y};
    data["width"] = e->width;
    data["height"] = e->height;
    data["rotation"] = e->rotation;
    data["isPlayer"] = e->isPlayer;
    data["isSolid"] = e->isSolid;
    data["physicsObject"] = e->physicsObject;
    data["hitboxSize"] = e->hitboxSize;
    data["mass"] = e->mass;
    data["isVisible"] = e->isVisible;

    for (size_t i = 0; i < e->lua_scripts.size(); i++) {
        data["scripts"][i] = e->lua_scripts[i].path;
    }

    int i = 0;
    for (auto it = e->entity_variables.begin(); it != e->entity_variables.end(); it++) {
        std::string key = it->second;
        if (e->entity_numbers.find(key) != e->entity_numbers.end()) {
            data["variables"][i++] = {
                {"name", key},
                {"type", "number"},
                {"value", e->entity_numbers[key]}
            };
        }
        else if (e->entity_strings.find(key) != e->entity_strings.end()) {
            data["variables"][i++] = {
                {"name", key},
                {"type", "string"},
                {"value", e->entity_strings[key]}
            };
        }
    }

    outfile << std::setw(4) << data;
}

Entity* loadPrefab(std::string filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        GameManager::ConsoleWrite("[ERROR] Failed to open prefab");
        return nullptr;
    }

    json data = json::parse(infile);
    Entity* e = new Entity();
    e->position = sf::Vector2f(data["position"][0], data["position"][1]);
    e->name = data["name"];
    e->entity_type = data["entity_type"];
    e->scale = sf::Vector2f(data["scale"][0], data["scale"][1]);
    e->width = data["width"];
    e->height = data["height"];
    e->rotation = data["rotation"];
    e->texturePath = data["texturePath"];
    e->SetSprite(data["texturePath"]);
    e->SetPosition(e->position);
    e->isPlayer = data["isPlayer"];
    if (e->isPlayer) {
        GameManager::player = e;
    }
    e->isSolid = data["isSolid"];
    e->physicsObject = data["physicsObject"];
    e->hitboxSize = data["hitboxSize"];
    e->mass = data["mass"];
    e->isVisible = data["isVisible"];

    for (json::iterator it = data["scripts"].begin(); it != data["scripts"].end(); ++it) {
        ScriptItem script;
        script.path = *it;
        script.showDetails = false;
        e->lua_scripts.push_back(script);
    }

    int index = 0;
    for (auto& it : data["variables"].items()) {
        std::string name = it.value()["name"];
        std::string type = it.value()["type"];

        e->entity_variables[index] = name;
        if (type == "number") {
            double value = it.value()["value"];
            e->entity_numbers[name] = value;
        }
        else if (type == "string") {
            std::string value = it.value()["value"];
            e->entity_strings[name] = value;
        }
    }

    return e;
}