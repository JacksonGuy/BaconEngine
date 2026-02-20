#include "file.h"

#include <filesystem>
#include <fstream>

#include "json.hpp"
#include "nfd.h"
#include "nfd.hpp"

#include "core/2D/entity.h"
#include "core/2D/game_object.h"
#include "core/game_state.h"
#include "editor/ui/editor_ui.h"
#include "core/globals.h"
#include "core/util.h"

namespace bacon
{
	namespace file
	{
		nfdresult_t save_project()
		{
			namespace fs = std::filesystem;
			using json = nlohmann::json;

			std::ofstream outfile(globals::project_file);
			json project_data;

			project_data["settings"]["version"] = globals::engine_version;
			project_data["settings"]["gravity"] = GameState::scene.get_gravity();
			project_data["settings"]["title"] = globals::project_title;

			const std::vector<GameObject*>& objects =
				GameState::scene.get_objects();
			for (size_t i = 0; i < objects.size(); i++)
			{
				GameObject* object = objects[i];
				object->save_to_json(project_data["objects"][i]);
			}

			outfile << std::setw(4) << project_data;

			globals::has_unsaved_changes = false;
			globals::update_window_title();

			debug_log("Project saved.");

			return NFD_OKAY;
		}

		void parse_project_settings(nlohmann::json json)
		{
			for (auto it = json.begin(); it != json.end(); it++)
			{
				std::string key = it.key();
				auto value = *it;

				if (key == "version")
				{
					globals::engine_version = value;
				}
				else if (key == "gravity")
				{
					float gravity = value;
					GameState::scene.set_gravity(gravity);
				}
				else if (key == "title")
				{
					globals::project_title = value;
				}
			}
		}

		void parse_project_objects(nlohmann::json json)
		{
			for (auto& object : json)
			{
				if (object.is_null())
				{
					debug_log("Null object encountered.");
					continue;
				}

				if (object["type"] == ObjectType::ENTITY)
				{
					Entity* entity = new Entity();
					GameState::scene.add_entity(entity);

					entity->load_from_json(object);
				}
				else if (object["type"] == ObjectType::TEXT)
				{
					TextObject* text = new TextObject();
					GameState::scene.add_text_object(text);

					text->load_from_json(object);
				}
				else if (object["type"] == ObjectType::CAMERA)
				{
					CameraObject* camera = new CameraObject();
					GameState::scene.add_camera(camera);

					camera->load_from_json(object);
				}
			}

			// Add objects to correct render layers
			for (GameObject* object : GameState::scene.get_objects())
			{
				if (object->object_type == ObjectType::CAMERA)
					continue;

				object->set_layer(object->get_layer());
			}
		}

		nfdresult_t load_project(bool show_dialog)
		{
			namespace fs = std::filesystem;
			using json = nlohmann::json;

			debug_log("Loading project...");
			std::string file_path = globals::project_file;

			if (show_dialog)
			{
				nfdu8char_t* path = NULL;
				nfdu8filteritem_t filters[1] = {{"Project File", "json"}};
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
			globals::project_directory =
				entry_path.parent_path().generic_string();
			globals::is_project_loaded = true;

			// Recreate scene
			GameState::scene.reset();

			// Load JSON data and parse
			json file_data = json::parse(infile);
			for (auto it = file_data.begin(); it != file_data.end(); it++)
			{
				std::string key = it.key();
				auto value = *it;

				if (key == "settings")
				{
					parse_project_settings(file_data["settings"]);
				}
				else if (key == "objects")
				{
					parse_project_objects(file_data["objects"]);
				}
			}

			globals::has_unsaved_changes = false;
			globals::update_window_title();

			ui::set_input_buffers();

			return NFD_OKAY;
		}

		nfdresult_t create_new_project()
		{
			using json = nlohmann::json;
			namespace fs = std::filesystem;

			std::ofstream outfile(globals::project_file);
			json data;

			// Write basic data to project file
			data["settings"]["gravity"] = GameState::scene.get_gravity();
			data["settings"]["version"] = globals::engine_version;
			data["settings"]["title"] = globals::project_title;
			outfile << std::setw(4) << data;

			// Create folders in project directory
			fs::create_directory(globals::project_directory +
								 std::string("/sprites"));
			fs::create_directory(globals::project_directory +
								 std::string("/scripts"));
			fs::create_directory(globals::project_directory +
								 std::string("/fonts"));
			fs::create_directory(globals::project_directory +
								 std::string("/prefabs"));
			fs::create_directory(globals::project_directory +
								 std::string("/sounds"));

			globals::has_unsaved_changes = false;
			globals::update_window_title();

			ui::set_input_buffers();

			return NFD_OKAY;
		}

		asset_t load_asset_nfd(AssetType type)
		{
			asset_t return_val = {
				.type = AssetType::NONE,
				.path = "",
			};
			nfdu8char_t* path = NULL;

			nfdu8filteritem_t filters;
			switch (type)
			{
				case AssetType::TEXTURE:
				{
					filters = texture_types;
					break;
				}

				case AssetType::FONT:
				{
					filters = font_types;
					break;
				}

				default:
				{
					debug_error("Invalid asset type!");
					return return_val;
				}
			}

			nfdopendialogu8args_t args = {0};
			args.filterCount = 1;
			args.filterList = &filters;
			nfdresult_t result = NFD_OpenDialogU8_With(&path, &args);

			if (result == NFD_ERROR)
			{
				debug_error("Failed to load asset!");
			}
			else if (result == NFD_OKAY)
			{
				return_val.type = type;
				return_val.path = std::string(path);
			}

			free(path);
			return return_val;
		}

		std::string abs_path_to_relative(std::string abs_path)
		{
			return std::filesystem::relative(abs_path,
											 globals::project_directory);
		}

		std::string relative_path_to_abs(std::string path)
		{
			return std::filesystem::absolute(path);
		}
	} // namespace file
} // namespace bacon
