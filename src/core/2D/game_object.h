#pragma once

#include <cstdint>
#include <stdint.h>

#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "raylib.h"

#include "core/uuid.h"
#include "lib/byte_stream.h"

namespace bacon
{
	enum class ObjectType : uint8_t
	{
		OBJECT = 0,
		ENTITY,
		TEXT,
		CAMERA,
	};

	class GameObject
	{
	public:
		friend class Renderer;

		static GameObject* create_object(ByteStream& bytes);

		ObjectType object_type;
		UUID uuid;
		std::string name;
		std::string tag;
		Vector2 position;
		Vector2 size;
		float rotation;
		bool is_visible;

		GameObject();
		GameObject(const GameObject& obj);
		GameObject& operator=(const GameObject& obj);
		GameObject(GameObject&& obj) = delete;
		GameObject& operator=(GameObject&& obj) = delete;
		virtual ~GameObject() = default;

		virtual void destroy() = 0;
		virtual void copy(const GameObject& obj);
		virtual GameObject* clone() const = 0;
		virtual GameObject* clone_unique() const = 0;

		virtual void add_to_scene() = 0;
		virtual void remove_from_scene() = 0;
		void add_children_to_scene();
		void remove_children_from_scene();

		GameObject* get_parent() const;
		void set_parent(GameObject* object);
		void add_child(GameObject* object);
		void remove_child(GameObject* object);
		void delete_children();
		void clone_children(const GameObject& object, bool add_to_scene = true);
		const std::vector<GameObject*>& get_children() const;

		Vector2 get_position_abs() const;
		void set_position(Vector2 position);
		void update_child_positions(Vector2 delta);
		void set_rotation(float rotation);
		void set_visibility(bool visibility);

		size_t get_layer() const;
		bool is_in_scene() const { return in_scene; }

		virtual void draw_outline() const;
		virtual bool contains_point(Vector2 point);

		virtual void update_ui_buffer() = 0;
		virtual void update_from_ui_buffer() = 0;

		virtual void draw() const = 0;

		virtual void draw_properties_editor() = 0;
		virtual void save_to_json(nlohmann::json& data) const;
		virtual void load_from_json(nlohmann::json& data);
		virtual size_t calculate_size() const;
		virtual ByteStream serialize() const;

	protected:
		GameObject* parent;
		std::vector<GameObject*> children;
		size_t layer;
		bool in_scene;

		void base_update_ui_buffer();
		void base_update_from_ui_buffer();

		bool draw_base_properties();

		virtual void deserialize(ByteStream& bytes);
	};
} // namespace bacon
