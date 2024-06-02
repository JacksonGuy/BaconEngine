#include <iostream>
#include <fstream>
#include "../src/json.hpp"

using json = nlohmann::json;

struct Entity {
    int id;
    std::string name;
    float position[2];
};

int main () {
    /*
    std::ifstream f("test.json");

    json data = json::parse(f);
    std::cout << data["pi"] << std::endl;

    struct Entity player = {0, "Carter", {10.0f, 20.0f}};

    json to_write = {
        {"id", player.id},
        {"name", player.name},
        {"position", {player.position[0], player.position[1]}}
    };

    std::ofstream write_file("leveldata.json");
    write_file << std::setw(4) << to_write << std::endl;
    */

    std::ifstream inFile("leveldata.json");
    json level_data = json::parse(inFile);

    for (auto& entity : level_data) {
        std::cout << entity["name"] << "\n";
        std::cout << entity["position"][0] << ", " << entity["position"][1] << "\n\n";
    }
}