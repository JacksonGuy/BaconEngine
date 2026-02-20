#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "json.hpp"
#include "raylib.h"

#include "core/uuid.h"

namespace bacon
{
	enum ObjectType : uint8_t
	{
		OBJECT = 0,
		ENTITY,
		TEXT,
		CAMERA,
	};

	typedef struct
	{
		std::string name;
		std::string tag;
		float position[2];
		float size[2];
		float rotation;
		bool is_visible;
		size_t layer;
	} ObjectBuffers;

	class GameObject
	{
	public:
		friend class Renderer;

		ObjectType object_type;
		UUID uuid;
		std::string name;
		std::string tag;
		Vector2 position;
		Vector2 size;
		float rotation;
		bool is_visible;

		GameObject();
		GameObject(uint8_t* bytes);
		GameObject(const GameObject& obj);
		GameObject& operator=(const GameObject& obj);
		GameObject(GameObject&& obj) = delete;
		GameObject& operator=(GameObject&& obj) = delete;
		virtual ~GameObject() = default;

		virtual void copy(const GameObject& obj);
		virtual GameObject* clone() const = 0;

		virtual void add_to_state() = 0;

		GameObject* get_parent() const;
		void set_parent(GameObject* object);
		void add_child(GameObject* object);
		void remove_child(GameObject* object);
		const std::vector<GameObject*>& get_children() const;

		const size_t get_layer() const;
		void set_layer(size_t layer);

		virtual void draw_outline() const;
		virtual bool contains_point(Vector2 point);

		virtual void update_buffers() = 0;
		virtual void update_from_buffers() = 0;

		virtual void draw() const = 0;
		virtual void draw_properties_editor() = 0;
		virtual void save_to_json(nlohmann::json& data) const;
		virtual void load_from_json(nlohmann::json& data);
		virtual size_t calculate_size() const;
		virtual uint8_t* serialize() const;
		virtual void deserialize(uint8_t* bytes);

	protected:
		GameObject* parent;
		std::vector<GameObject*> children;
		size_t layer;

		void update_base_buffers();
		void update_from_base_buffers();

		bool draw_base_properties();

	private:
		ObjectBuffers m_buffers;
	};
} // namespace bacon
