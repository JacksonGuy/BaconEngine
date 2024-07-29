#include <iostream>
#include <fstream>
#include <filesystem>
#include <nfd.h>

#include "File.hpp"
#include "GameManager.hpp"
#include "json.hpp"
#include "Rendering.hpp"

using json = nlohmann::json;

namespace File {

/**
 * @brief Saves the game project
 * 
 * @param filename File to save to
 */
void save(std::string filename) {
    namespace fs = std::filesystem;
    
    std::ofstream outfile(filename);
    json level_data;

    level_data["Version"] = GameManager::config.version;

    // Save Entities
    for (size_t index = 0; index < GameManager::Entities.size(); index++) {
        Entity* e = GameManager::Entities[index];
        std::string texturePath = fs::relative(e->texturePath, GameManager::entryPoint).generic_string();
        //texturePath.erase(0, 3);

        level_data["Entities"][index] = {
            {"id", e->ID},
            {"name", e->name},
            {"tag", e->tag},
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
            {"isVisible", e->isVisible},
            {"layer", e->layer}
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
        for (GameObject* child : e->children) {
            level_data["Entities"][index]["children"][i++] = child->ID;
        }
    }

    // Save TextObjects
    for (size_t index = 0; index < GameManager::TextObjects.size(); index++) {
        TextObj* obj = GameManager::TextObjects[index];
        
        sf::Color color = obj->text.getFillColor();

        level_data["Text"][index] = {
            {"id", obj->ID},
            {"name", obj->name},
            {"position", {obj->position.x, obj->position.y}},
            {"size", obj->text.getCharacterSize()},
            {"rotation", obj->text.getRotation()},
            {"color", {color.r, color.g, color.b, color.a}},
            {"text", obj->text.getString()},
            {"isVisible", obj->isVisible},
            {"layer", obj->layer},
            {"tag", obj->tag}
        };

        if (obj->parent == nullptr) {
            level_data["Text"][index]["parent"] = -1;
        }
        else {
            level_data["Text"][index]["parent"] = obj->parent->ID;
        }

        int i = 0;
        for (GameObject* child : obj->children) {
            level_data["Text"][index]["children"][i++] = child->ID;
        }
    }

    level_data["Settings"]["Gravity"] = GameManager::gravity;

    outfile << std::setw(4) << level_data;

    GameManager::ConsoleWrite("[ENGINE] Successfully saved project.");
}

/**
 * @brief Loads game project from a file
 * 
 * @param filename Name of the file
 * @return boolean: if loading the project failed
 */
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
        Entity* e = new Entity();
        e->ID = entity["id"];
        if (e->ID > GameObject::IDCount) {
            GameObject::IDCount = e->ID; // Set to max to preserve IDs
        }
        e->name = entity["name"];
        e->tag = entity["tag"];
        e->position.x = entity["position"][0];
        e->position.y = entity["position"][1];
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
        e->layer = entity["layer"];

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

    GameManager::ConsoleWrite("[ENGINE] Creating Text...");
    for (auto& obj : level_data["Text"]) {
        TextObj* text = new TextObj();
        text->ID = obj["id"];
        
        if (text->ID > GameObject::IDCount) {
            GameObject::IDCount = text->ID; // Set to max to preserve IDs
        }
    
        text->name = obj["name"];
        text->position = sf::Vector2f(obj["position"][0], obj["position"][1]);
        text->text.setPosition(text->position);
        text->text.setCharacterSize(obj["size"]);
        text->text.setRotation(obj["rotation"]);
        text->text.setFillColor(sf::Color(
            obj["color"][0], obj["color"][1], obj["color"][2], obj["color"][3])
        );
        text->isVisible = obj["isVisible"];
        text->layer = obj["layer"];
        std::string str = obj["text"];
        text->text.setString(str);
        text->tag = obj["tag"];
    }

    GameManager::ConsoleWrite("[ENGINE] Sorting GameObjects...");
    GameManager::SortObjectsByID();

    GameManager::ConsoleWrite("[ENGINE] Constructing Entity Tree...");
    for (auto& entity : level_data["Entities"]) {
        unsigned int childID = entity["id"];
        int parentID = entity["parent"];
        if (parentID != -1) {
            GameObject* child = GameManager::FindObjectByID(childID);
            GameObject* parent = GameManager::FindObjectByID((unsigned int)parentID);
            if (child != nullptr && parent != nullptr) {
                parent->children.push_back(child);
                child->parent = parent;
            }
        }
    }
    for (auto& text : level_data["Text"]) {
        unsigned int childID = text["id"];
        int parentID = text["parent"]; 
        if (parentID != -1) {
            GameObject* child = GameManager::FindObjectByID(childID);
            GameObject* parent = GameManager::FindObjectByID((unsigned int)parentID);
            if (child != nullptr && parent != nullptr) {
                parent->children.push_back(child);
                child->parent = parent;
            }
        }
    }

    GameManager::ConsoleWrite("[ENGINE] Creating layers...");
    for (GameObject* obj : GameManager::GameObjects) {
        Rendering::AddToLayer(obj);
    }

    GameManager::gravity = level_data["Settings"]["Gravity"];

    return true;
}

bool CreateNew(std::string& path) {
    nfdchar_t* outpath = NULL;
    nfdresult_t result = NFD_PickFolder(NULL, &outpath);

    if (result == NFD_OKAY) {
        std::string _path = outpath + std::string("\\Game.json");
        GameManager::entryPoint = outpath;
        std::ofstream outfile(_path);
        json data;

        data["Settings"]["Gravity"] = GameManager::gravity;
        data["Version"] = GameManager::config.version;
        outfile << std::setw(4) << data;

        std::filesystem::create_directory(outpath + std::string("/sprites"));
        std::filesystem::create_directory(outpath + std::string("/scripts"));
        std::filesystem::create_directory(outpath + std::string("/fonts"));
        std::filesystem::create_directory(outpath + std::string("/prefabs"));
        std::filesystem::create_directory(outpath + std::string("/sounds"));

        free(outpath);
        path = _path;
        return true;
    }

    free(outpath);
    return false;
}

/**
 * @brief Converts a relative path to an absolute system path
 * 
 * @param path the relative file path
 * @return std::string absolute system path
 */
std::string toAbsolute(std::string path) {
    namespace fs = std::filesystem;
    std::string absPath = GameManager::entryPoint + "/" + path;
    return absPath; 
}

/**
 * @brief Save editor settings
 * 
 * @param state Editor settings
 */
void saveConfig(ConfigState state) {
    std::ofstream outfile("config.json");
    json settings;

    settings["version"] = state.version;
    settings["resolution"] = state.resolution;

    outfile << std::setw(4) << settings;

    GameManager::config = state;
}

/**
 * @brief Loads and applies editor settings
 * 
 * @return ConfigState the loaded settings
 */
ConfigState loadConfig() {
    std::ifstream infile("config.json");
    if (!infile.is_open()) {
        GameManager::ConsoleWrite("[ERROR] Failed to load editor settings");
        
        ConfigState state;
        state.version = "0";
        state.resolution = "800x600";
        return state;
    }

    json settings = json::parse(infile);
    ConfigState state = {
        settings["version"],
        settings["resolution"]
    };

    GameManager::config = state;
    return state;
}

/**
 * @brief Saves an Entity as a prefab file
 * 
 * @param filename The file to save the prefab to
 * @param e the entity to save
 */
void savePrefab(std::string filename, Entity* e) {
    std::ofstream outfile(filename);
    json data;

    data["name"] = e->name;
    data["tag"] = e->tag;
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

/**
 * @brief Creates an Entity from a prefab file
 * 
 * @param filename the file to read from
 * @return Entity* the created Entity
 */
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
    e->tag = data["tag"];
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
}