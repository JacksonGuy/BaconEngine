#pragma once

#include <string>
#include <vector>

#include "lib/byte_stream.h"
#include "nlohmann/json.hpp"

#include "uuid.h"

namespace bacon
{
	class GameObject
	{
	public:
		GameObject();
		GameObject(const GameObject& object);
		GameObject& operator=(const GameObject& object);
		GameObject(GameObject&& object) = delete;
		GameObject& operator=(GameObject&& object) = delete;
		virtual ~GameObject() = default;

		virtual void destroy() = 0;
		virtual void copy(const GameObject& object);
		virtual GameObject* clone() const = 0;
		virtual GameObject* clone_unique() const = 0;

		virtual void add_to_scene() = 0;
		virtual void remove_from_scene() = 0;
		virtual void add_children_to_scene() = 0;
		virtual void remove_children_from_scene() = 0;

		// virtual void set_parent(GameObject* parent) = 0;
		// virtual void add_child(GameObject* child) = 0;
		// virtual void remove_child(GameObject* child) = 0;

		virtual void update_ui_buffer() const;
		virtual void update_from_ui_buffer();

		virtual void draw_properties_editor() = 0;
		virtual void save_to_json(nlohmann::json& json) const;
		virtual void load_from_json(const nlohmann::json& json);
		virtual ByteStream serialize() const = 0;

		UUID get_uuid() const 				{ return m_uuid; };
		std::string get_name() const 		{ return m_name; };
		bool get_in_scene() const 			{ return m_in_scene; };
		void set_in_scene(bool in_scene) 	{ m_in_scene = in_scene; };
		std::string get_tag() const 		{ return m_tag; };
		void set_name(std::string name) 	{ m_name = std::move(name); };
		void set_tag(std::string tag) 		{ m_tag = std::move(tag); };
		void set_uuid(UUID uuid) { m_uuid = std::move(uuid); };

	protected:
		virtual void deserialize(ByteStream& bytes);

	private:
		UUID m_uuid;
		std::string m_name;
		std::string m_tag;
		bool m_in_scene;
	};
} // namespace bacon
