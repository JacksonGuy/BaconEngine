#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdlib.h>

#include <nfd.h>
#include "json.hpp"

#include "File.h"
#include "GameManager.h"
#include "Entity.h"

namespace File {
    /**
     * @brief Saves the current project
     * 
     * @param filename file to save project to
     * @return true if successfully saved
     * @return false if saving failed
     */
    bool SaveProject(std::string filename) {
        namespace fs = std::filesystem;
        using json = nlohmann::json;

        std::ofstream outfile(filename);
        json data;

        data["Version"] = GameManager::engineVersion;
        data["Settings"]["Gravity"] = GameManager::gravity;

        // Save GameObjects
        for (size_t i = 0; i < GameManager::GameObjects.size(); i++) {
            // Save general GameObject information
            GameObject* obj = GameManager::GameObjects[i];
            obj->SaveGameObjectJson(data["GameObjects"][i]);

            // Save information specific to object type
            if (obj->type == ENTITY) {
                Entity* e = (Entity*)obj;
                e->SaveEntityJson(data["GameObjects"][i]);
            }
            else if (obj->type == TEXT) {
                TextObject* text = (TextObject*)obj;
                text->SaveTextObjectJson(data["GameObjects"][i]);
            }
            else if (obj->type == CAMERA) {
                GameCamera* camera = (GameCamera*)obj;
                camera->SaveCameraJson(data["GameObjects"][i]);
            }
        }

        outfile << std::setw(4) << data;

        GameManager::ConsoleMessage("Successfully saved project.");
        return true;
    }

    /**
     * @brief Loads a new project
     * 
     * @return true if the project was successfully loaded
     * @return false if the project couldn't be loaded
     */
    bool LoadProject(std::string filename) {
        namespace fs = std::filesystem;
        using json = nlohmann::json;

        GameManager::ConsoleMessage("Loading Project...");

        std::ifstream infile(filename);
        if (!infile.is_open()) {
            GameManager::ConsoleError("Failed to load project");
            return false;
        }

        fs::path entryPath = fs::path(filename);
        GameManager::projectEntryPath = entryPath.parent_path().generic_string();
        json data = json::parse(infile);

        GameManager::ConsoleMessage("Creating Objects...");
        for (auto& obj : data["GameObjects"]) {
            if (obj["type"] == 1) {
               Entity* e = new Entity();
               e->LoadEntityJson(obj); 
            }
            else if (obj["type"] == 2) {
                TextObject* text = new TextObject();
                text->LoadTextObjectJson(obj);
            }
            else if (obj["type"] == 3) {
                GameCamera* camera = new GameCamera();
                camera->LoadCameraJson(obj);
            }
        }

        GameManager::ConsoleMessage("Constructing Object Tree...");
        for (auto& obj : data["GameObjects"]) {
            i64 parentID = obj["parent"];
            i64 childID = obj["ID"];

            if (parentID != -1) {
                GameObject* parent = GameManager::GameObjects[parentID];
                GameObject* child = GameManager::GameObjects[childID];
                if (parent != nullptr && child != nullptr) {
                    parent->children.push_back(child);
                    child->parent = parent;
                }
            }
        }

        GameManager::gravity = data["Settings"]["Gravity"];
        GameManager::engineVersion = data["Version"];

        return false;
    }
    
    /**
     * @brief Creates a new project file directory and loads it.
     * 
     * @param path The path of the new project. Chosen using NFD.
     * @param reset Whether to reset GameManager before creating project
     * @return true if the project was successfully created
     * @return false if failed to create project
     */
    bool CreateNewProject(std::string& path, bool reset) {
        using json = nlohmann::json; 
        namespace fs = std::filesystem;

        // In the event that we want to create a new project that overrides our current project
        if (reset) GameManager::Reset();

        nfdu8char_t* outpath = NULL;
        nfdpickfolderu8args_t args = {0};
        nfdresult_t result = NFD_PickFolderU8_With(&outpath, &args);

        if (result == NFD_OKAY) {
            path = outpath + std::string("\\Game.json");
            GameManager::projectEntryPath = outpath;

            std::ofstream outfile(path);
            json data;

            data["Settings"]["Gravity"] = 0.2; // Default gravity value here
            data["Version"] = GameManager::engineVersion;
            outfile << std::setw(4) << data;

            // Create project folders
            fs::create_directory(outpath + std::string("/sprites"));
            fs::create_directory(outpath + std::string("/scripts"));
            fs::create_directory(outpath + std::string("/fonts"));
            fs::create_directory(outpath + std::string("/prefabs"));
            fs::create_directory(outpath + std::string("/sounds"));

            free(outpath);
            return true;
        }

        free(outpath);
        return false;
    }
};