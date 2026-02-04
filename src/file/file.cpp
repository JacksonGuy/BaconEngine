#include "file.h"

#include <filesystem>
#include <fstream>

#include "core/2D/game_object.h"
#include "json.hpp"
#include "nfd.h"
#include "nfd.hpp"

#include "core/globals.h"
#include "core/util.h"

namespace bacon {
    namespace file {
        nfdresult_t save_project(const GameManager& manager) {
            namespace fs = std::filesystem;
            using json = nlohmann::json;

            if (!globals::is_project_loaded)
            {
                nfdresult_t result = create_new_project(manager);
                if (result == NFD_ERROR) {
                    debug_error("Failed to create new project.");
                    return result;
                }
                else if (result == NFD_CANCEL)
                {
                    return result;
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

            return NFD_OKAY;
        }

        nfdresult_t load_project(GameManager& manager, bool show_dialog) {
            namespace fs = std::filesystem;
            using json = nlohmann::json;

            debug_log("Loading project...");
            std::string file_path = globals::project_file;

            if (show_dialog)
            {
                nfdu8char_t* path = NULL;
                nfdu8filteritem_t filters[1] = {
                    {"Project File", "json"}
                };
                nfdopendialogu8args_t args = {0};
                args.filterCount = 1;
                args.filterList = filters;
                nfdresult_t result = NFD_OpenDialogU8_With(&path, &args);

                if (result == NFD_ERROR)
                {
                    free(path);
                    debug_error("Failed to load project.");
                    return result;
                }
                else if (result == NFD_CANCEL)
                {
                    free(path);
                    return result;
                }
                else
                {
                    file_path = std::string(path);
                    free(path);
                }
            }

            // Open project file
            std::ifstream infile(file_path);
            if (!infile.is_open())
            {
                debug_error("Failed to load project: file doesn't exist");
                return NFD_ERROR;
            }

            // Save path globals
            fs::path entry_path = fs::path(file_path);
            globals::project_file = file_path;
            globals::project_directory = entry_path.parent_path().generic_string();
            globals::is_project_loaded = true;

            // Load JSON data and parse
            json file_data = json::parse(infile);

            globals::engine_version = file_data["settings"]["version"];
            float gravity = file_data["settings"]["gravity"];
            manager.set_gravity(gravity);

            for (auto& object : file_data["objects"])
            {
                if (object.is_null())
                {
                    debug_log("Null object encountered.");
                    continue;
                }

                if (object["type"] == ObjectType::ENTITY)
                {
                    BodyType body_type(object["body_type"]);
                    Entity* entity = manager.instantiate_entity(body_type);
                    entity->load_from_json(object);
                }
                else if (object["type"] == ObjectType::TEXT)
                {
                    TextObject* text = manager.instantiate_text();
                    text->load_from_json(object);
                }
                else if (object["type"] == ObjectType::CAMERA)
                {
                    CameraObject* camera = manager.instantiate_camera();
                    camera->load_from_json(object);
                }
            }

            // Add objects to correct render layers
            const std::vector<GameObject*>& objects = manager.get_objects();
            for (GameObject* object : objects) {
                if (object->object_type != ObjectType::ENTITY) continue;

                manager.set_object_layer(object, object->get_layer());
            }

            return NFD_OKAY;
        }

        nfdresult_t create_new_project(const GameManager& manager) {
            using json = nlohmann::json;
            namespace fs = std::filesystem;

            nfdu8char_t* outpath = NULL;
            nfdpickfolderu8args_t args = {0};
            nfdresult_t result = NFD_PickFolderU8_With(&outpath, &args);

            if (result == NFD_ERROR)
            {
                free(outpath);
                debug_error("Failed to create new project.");
                return result;
            }
            else if (result == NFD_CANCEL)
            {
                free(outpath);
                return result;
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
            return NFD_OKAY;
        }

        std::string abs_path_to_relative(std::string abs_path) {
            return std::filesystem::relative(abs_path, globals::project_directory);
        }

        std::string relative_path_to_abs(std::string path) {
            return std::filesystem::absolute(path);
        }
    }
}
