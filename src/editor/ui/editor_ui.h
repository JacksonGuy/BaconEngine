#pragma once

#include "core/2D/game_object.h"
#include "core/2D/text_object.h"
#include "core/game_state.h"
#include "editor/editor.h"

namespace bacon
{
	namespace ui
	{
		enum class LastEditorAction : uint16_t
		{
			NONE = 0,
			PROJECT_NEW,
			PROJECT_SAVE,
			PROJECT_SAVE_AS,
			PROJECT_OPEN,
			PROGRAM_EXIT,
			GAME_PLAY,
			GAME_EXIT,
		};

		namespace settings
		{
			inline std::string project_title;
			inline std::string editor_font;
			inline uint32_t framerate_limit;

			inline float gravity;
			inline int physics_steps;
			inline float pixels_per_meter;
		} // namespace settings

		typedef struct ObjectFields
		{
			// GameObject
			std::string name;
			std::string tag;
			float position[2];
			float size[2];
			float rotation;
			bool is_visible;
			size_t layer;

			// Entity
			std::string texture_path;
			BodyType body_type;
			float body_center[2];
			float linear_damping;
			float angular_damping;
			float gravity_scale;
			bool is_sleeping;
			bool disabled;
			bool fixed_rotation;
			bool is_bullet;
			float mass;
			float density;
			float rotational_inertia;
			float friction;
			float restitution;

			// TextObject
			std::string text;
			std::string font_path;
			int32_t font_size;
			int32_t char_spacing;
			Color color;

			// CameraObject
			bool is_active;
			float zoom;
		} ObjectFields;

		inline ObjectFields obj_properties;
		inline GameObject* inspect_object_copy;

		inline bool show_test = true;
		inline bool show_object_properties = true;
		inline bool show_object_tree = true;
		inline bool show_scene_display = true;
		inline bool show_console = true;
		inline bool show_settings = false;
		inline bool show_general_info = true;

		inline bool show_entity_create = false;
		inline bool show_text_create = false;
		inline bool show_camera_create = false;

		inline LastEditorAction last_action = LastEditorAction::NONE;
		inline bool show_create_new_project = false;
		inline bool show_save_confirm_popup = false;
		inline bool show_save_as_popup = false;

		inline GameObject* view_properties_object = nullptr;
		inline ImVec2 window_position;
		inline ImVec2 window_size;
		inline Vector2 window_mouse_position;

		inline ImGuiWindowFlags global_window_flags;
		inline bool move_windows = false;

		void init(uint32_t width, uint32_t height);
		void set_input_buffers();

		void draw_top_bar(Editor* editor);
		void draw_object_properties();
		void draw_object_tree();
		void draw_scene_display();
		void draw_engine_console(Editor* editor);
		void draw_settings();
		void draw_general_info_display(Editor* editor);

		void draw_entity_create();
		void draw_text_create();
		void draw_camera_create();

		void draw_save_confirm_popup(Editor* editor);
		void draw_save_as_popup(Editor* editor);
		void draw_create_project_popup(Editor* editor);

		void game_object_tree_recurse(GameObject* object);

		void editor_new_project();
		void editor_save_project(Editor* editor);
		void editor_save_project_as();
		void editor_open_project(Editor* editor);

		void editor_create_from_prefab();

		void SetImGuiStyle();
	} // namespace ui
} // namespace bacon
