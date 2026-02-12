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
		GameObject(GameObject&& obj) = delete;
		GameObject& operator=(const GameObject& obj) = delete;
		GameObject& operator=(GameObject&& obj) = delete;
		virtual ~GameObject() = default;

		const GameObject* get_parent() const;
		const std::vector<GameObject*>& get_children() const;
		const size_t get_layer() const;
		void set_layer(size_t layer);

		virtual void draw() const = 0;
		virtual void draw_properties_editor();
		virtual void save_to_json(nlohmann::json& data) const;
		virtual void load_from_json(nlohmann::json& data);
		virtual size_t calculate_size() const;
		virtual uint8_t* serialize() const;
		virtual void deserialize(uint8_t* bytes);

	protected:
		GameObject* parent;
		std::vector<GameObject*> children;
		size_t layer;
	};
} // namespace bacon
