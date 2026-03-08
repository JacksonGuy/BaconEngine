#include "file.h"

#include <filesystem>
#include <fstream>

#include "core/game_state.h"
#include "core/uuid.h"
#include "nlohmann/json.hpp"
#include "nfd.h"

#include "core/2D/entity.h"
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
			project_data["settings"]["title"] = globals::project_title;

			if (GameState::game_type == GameState::GameType::GAME_2D)
			{
				project_data["settings"]["type"] = "2D";
				project_data["settings"]["gravity"] = GameState::state_2d->scene->get_gravity();

				const std::vector<Object2D*>& objects = GameState::state_2d->scene->get_objects();
				for (Object2D* object : objects)
				{
					if (object->get_parent() == nullptr)
					{
						json obj_data;
						object->save_to_json(obj_data);
						project_data["objects"].push_back(obj_data);
					}
				}
			}
			else if (GameState::game_type == GameState::GameType::GAME_3D)
			{
				project_data["settings"]["type"] = "3D";
			}

			outfile << std::setw(4) << project_data;

			globals::has_unsaved_changes = false;
			globals::update_window_title();

			debug_log("Project saved.");

			return NFD_OKAY;
		}

		void parse_project_settings(const nlohmann::json& json)
		{
			GameState::game_type = (GameState::GameType)json_read_uint8(json, "type");
			globals::engine_version = json_read_string(json, "version");
			globals::project_title = json_read_string(json, "title");

			if (GameState::game_type == GameState::GameType::GAME_2D)
			{
				if (GameState::state_2d != nullptr)
				{
					delete GameState::state_2d;
				}
				GameState::state_2d = new GameState2D();

				GameState::state_2d->scene->set_gravity(json_read_float(json, "gravity"));
			}
			else if (GameState::game_type == GameState::GameType::GAME_3D)
			{
				// TODO
				// GameState::state_3d = new GameState3D();
			}
		}

		void parse_project_objects(const nlohmann::json& json)
		{
			for (auto& object : json)
			{
				if (object.is_null())
				{
					debug_log("Null object encountered.");
					continue;
				}

				if (object["type"] == "entity")
				{
					Entity* entity = new Entity();
					entity->load_from_json(object);
					entity->add_to_scene();
				}
				else if (object["type"] == "text")
				{
					TextObject* text = new TextObject();
					text->load_from_json(object);
					text->add_to_scene();
				}
				else if (object["type"] == "camera")
				{
					CameraObject* camera = new CameraObject();
					camera->load_from_json(object);
					camera->add_to_scene();
				}
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

			// Delete scene data
			if (GameState::game_type == GameState::GameType::GAME_2D)
			{
				GameState::state_2d->scene->reset();
			}

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
			data["settings"]["gravity"] = 9.80;
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

		nfdresult_t save_object_prefab(const GameObject& object)
		{
			using json = nlohmann::json;
			namespace fs = std::filesystem;

			nfdu8char_t* path = NULL;
			nfdu8filteritem_t filters[] = {{"Prefab Files", "json"}};
			nfdsavedialogu8args_t args = {0};
			args.filterCount = 1;
			args.filterList = filters;
			args.defaultName = "prefab.json";
			nfdresult_t result = NFD_SaveDialogU8_With(&path, &args);

			if (result == NFD_ERROR)
			{
				NFD_FreePathU8(path);
				return result;
			}
			else if (result == NFD_CANCEL)
			{
				NFD_FreePathU8(path);
				return result;
			}

			std::ofstream outfile(path);
			json data;
			object.save_to_json(data);
			outfile << std::setw(4) << data;

			debug_log("Saved object to prefab file.");

			NFD_FreePathU8(path);
			return NFD_OKAY;
		}

		nfdresult_t load_from_prefab(const std::string& path, GameObject& object)
		{
			using json = nlohmann::json;

			std::ifstream infile(path);
			if (!infile.is_open())
			{
				debug_error("Failed to open prefab file!");
				return NFD_ERROR;
			}

			json data = json::parse(infile);
			object.load_from_json(data);

			// Create new UUIDs for things.
			// load_from_json preserves UUIDs.
			object.set_uuid(UUID());
			if (GameState::game_type == GameState::GameType::GAME_2D)
			{
				Object2D& object2d = (Object2D&)object;

				for (Object2D* child : object2d.get_children())
				{
					child->set_uuid(UUID());
				}
			}

			debug_log("Object loaded from prefab.");

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
