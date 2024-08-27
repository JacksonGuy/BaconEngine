#include "File.h"
#include "GameManager.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdlib.h>

#include <nfd.h>
#include "json.hpp"

namespace File {
    bool SaveProject(std::string filename) {
        namespace fs = std::filesystem;
        using json = nlohmann::json;

        std::ofstream outfile(filename);
        json data;

        data["Version"] = GameManager::engineVersion;
        data["Settings"]["Gravity"] = {0, GameManager::gravity};

        for (size_t i = 0; i < GameManager::Entities.size(); i++) {
            Entity* e = GameManager::Entities[i];
            e->SaveEntityJson(data["Entities"][i]);
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
    bool LoadProject() {

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

            data["Settings"]["Gravity"] = {0, 0.2}; // Default gravity value here
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