#include "file.h"

#include <filesystem>
#include <fstream>

#include "json.hpp"
#include "nfd.h"
#include "nfd.hpp"

#include "core/globals.h"
#include "core/util.h"

namespace bacon {
    namespace file {
        bool save_project(const GameManager& manager) {
            namespace fs = std::filesystem;
            using json = nlohmann::json;

            if (!globals::is_project_loaded)
            {
                bool result = create_new_project(manager);
                if (!result) {
                    debug_error("Failed to create new project.");
                    return false;
                }
            }

            std::ofstream outfile(globals::project_file);
            json project_data;

            project_data["settings"]["version"] = globals::engine_version;
            project_data["settings"]["gravity"] = manager.get_gravity();

            const std::vector<GameObject*>& objects = manager.get_objects();
            for (size_t i = 0; i < objects.size(); i++) {
                GameObject* object = objects[i];
                object->save_to_json(project_data["objects"][i]);
            }

            outfile << std::setw(4) << project_data;

            return true;
        }

        bool load_project(GameManager& manager) {
            namespace fs = std::filesystem;
            using json = nlohmann::json;

            nfdu8char_t* path = NULL;
            nfdu8filteritem_t filters[1] = {
                {"Project File", "json"}
            };
            nfdopendialogu8args_t args = {0};
            args.filterCount = 1;
            args.filterList = filters;
            nfdresult_t result = NFD_OpenDialogU8_With(&path, &args);

            if (result != NFD_OKAY)
            {
                free(path);
                debug_error("Failed to load project.");
                return false;
            }

            // Open project file
            std::ifstream infile(path);
            if (!infile.is_open())
            {
                debug_error("Failed to load project: file doesn't exist");
                return false;
            }

            // Save path globals
            fs::path entry_path = fs::path(path);
            globals::project_file = path;
            globals::project_directory = entry_path.parent_path().generic_string();
            globals::is_project_loaded = true;

            // Load JSON data and parse
            json file_data = json::parse(infile);
            for (auto it = file_data.begin(); it != file_data.end(); it++)
            {
                std::string key = it.key();
                auto value = *it;

                if (key == "version")
                {
                    globals::engine_version = value;
                }
                else if (key == "objects")
                {
                    for (auto& object : file_data["objects"])
                    {
                        if (object.is_null())
                        {
                            continue;
                        }

                        if (object["type"] == "entity")
                        {
                            body_t body_type(object["physics_body_type"]);
                            Entity* entity = manager.instantiate_entity(body_type);
                            entity->load_from_json(object);
                        }
                        else if (object["type"] == "text")
                        {
                            debug_error("This has not been implemented yet");
                        }
                        else if (object["type"] == "camera")
                        {
                            debug_error("This has not been implemented yet.");
                        }
                    }
                }
            }

            free(path);
            return true;
        }

        bool create_new_project(const GameManager& manager) {
            using json = nlohmann::json;
            namespace fs = std::filesystem;

            nfdu8char_t* outpath = NULL;
            nfdpickfolderu8args_t args = {0};
            nfdresult_t result = NFD_PickFolderU8_With(&outpath, &args);

            if (result != NFD_OKAY)
            {
                free(outpath);
                debug_error("Failed to create new project.");
                return false;
            }

            // Set globals
            globals::project_directory = outpath;
            globals::project_file = outpath + std::string("//game.json");
            globals::is_project_loaded = true;

            std::ofstream outfile(globals::project_file);
            json data;

            // Write basic data to project file
            data["settings"]["gravity"] = manager.get_gravity();
            data["settings"]["version"] = globals::engine_version;
            outfile << std::setw(4) << data;

            // Create folders in project directory
            fs::create_directory(outpath + std::string("/sprites"));
            fs::create_directory(outpath + std::string("/scripts"));
            fs::create_directory(outpath + std::string("/fonts"));
            fs::create_directory(outpath + std::string("/prefabs"));
            fs::create_directory(outpath + std::string("/sounds"));

            free(outpath);
            return true;
        }

        std::string abs_path_to_relative(std::string abs_path) {
            return std::filesystem::relative(abs_path, globals::project_directory);
        }

        std::string relative_path_to_abs(std::string path) {
            return std::filesystem::absolute(path);
        }
    }
}
