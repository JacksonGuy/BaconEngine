#include <iostream>
#include <fstream>

#include "File.hpp"
#include "GameManager.hpp"
#include "json.hpp"

using json = nlohmann::json;

void save(std::string filename) {
    std::ofstream outfile("./Projects/" + filename);
    json level_data;

    for (Entity* e : GameManager::Entities) {
        level_data[e->ID] = {
            {"id", e->ID},
            {"name", e->name},
            {"texturePath", e->texturePath},
            {"position", {e->position.x, e->position.y}},
            {"scale", {e->scale.x, e->scale.y}},
            {"width", e->width},
            {"height", e->height},
            {"rotation", e->rotation}
        };
    }

    outfile << std::setw(4) << level_data;
}

void load(std::string filename) {
    std::string projectName = "./Projects/" + filename;
    std::cout << "[DEBUG] Loading Project " << projectName << std::endl;

    std::ifstream infile(projectName);
    json level_data = json::parse(infile);

    for (auto& entity : level_data) {
        std::cout << "[DEBUG] Creating entity..." << std::endl;

        std::string name = entity["name"];
        //name.erase(name.end()-1); // Remove quotes from start and end of string
        //name.erase(name.begin());
        std::string texturePath = entity["texturePath"];
        //texturePath.erase(texturePath.end()-1);
        //texturePath.erase(texturePath.begin());
        sf::Vector2f position = {entity["position"][0], entity["position"][1]};
        sf::Vector2f scale = {entity["scale"][0], entity["scale"][1]};
        int width = entity["width"];
        int height = entity["height"];
        float rotation = entity["rotation"];

        std::cout << "[DEBUG] Loaded Variables..." << std::endl;

        Entity* e = new Entity(position);
        e->scale = scale;
        e->width = width;
        e->height = height;
        e->rotation = rotation;
        e->UpdateRect();
        e->SetSprite(texturePath, false);

        std::cout << "[DEBUG] Successfully Created Entity" << std::endl;
    }
}