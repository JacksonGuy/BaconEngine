#pragma once

#include <string>
#include <vector>

#include "lib/byte_stream.h"
#include "nlohmann/json.hpp"

#include "uuid.h"

namespace bacon
{
	enum class TypeID: uint32_t
	{
		GAME_OBJECT = 0,
		OBJECT_2D_START = 100,
			OBJECT_2D = 101,
			ENTITY_2D = 102,
			TEXT_2D = 103,
			CAMERA_2D = 104,
		OBJECT_2D_END = 199,
		OBJECT_3D_START = 200,
			OBJECT_3D = 201,
			ENTITY_3D = 202,
			TEXT_3D = 203,
			CAMERA_3D = 204,
		OBJECT_3D_END = 299,
	};

	template <typename T, typename U>
	T* dynamic_cast_to(U* object)
	{
		if (object && T::classof(object))
			return static_cast<T*>(object);
		return nullptr;
	}

	template <typename T, typename U>
	T* assured_cast(U* object)
	{
		assert(object && T::classof(object) && "assured_cast: incorrect type");
		return static_cast<T*>(object);
	}

	class GameObject
	{
	public:
		static GameObject* create_game_object(ByteStream& bytes);

		// Everything created is a GameObject
		static bool classof(const GameObject* object) { return true; }

		static constexpr TypeID static_type_id = TypeID::GAME_OBJECT;

		GameObject();
		GameObject(const GameObject& object);
		GameObject& operator=(const GameObject& object) = delete;
		GameObject(GameObject&& object) = delete;
		GameObject& operator=(GameObject&& object) = delete;
		virtual ~GameObject() = default;

		virtual void destroy();
		virtual void copy(const GameObject& object);
		virtual GameObject* clone() const = 0;
		virtual GameObject* clone_unique() const = 0;

		virtual void add_to_scene() = 0;
		virtual void remove_from_scene() = 0;
		void add_children_to_scene();
		void remove_children_from_scene();

		virtual void set_parent(GameObject* parent);
		virtual void add_child(GameObject* child);
		virtual void remove_child(GameObject* child);
		virtual GameObject* get_parent() const { return m_parent; };
		virtual const std::vector<GameObject*>& get_children() const { return m_children; };
		virtual void delete_children();
		virtual void clone_children(const GameObject& object, bool add_to_scene);

		virtual void update_ui_buffer() const;
		virtual void update_from_ui_buffer();

		virtual void draw_properties_editor() = 0;
		virtual void save_to_json(nlohmann::json& json) const;
		virtual void load_from_json(const nlohmann::json& json);
		virtual ByteStream serialize() const = 0;

		template <typename T>
		bool is() const { return m_type_id == T::static_type_id; }

		TypeID get_type_id() const 			{ return m_type_id; };
		UUID get_uuid() const 				{ return m_uuid; };
		std::string get_name() const 		{ return m_name; };
		bool get_in_scene() const 			{ return m_in_scene; };
		void set_in_scene(bool in_scene) 	{ m_in_scene = in_scene; };
		std::string get_tag() const 		{ return m_tag; };
		void set_name(std::string name) 	{ m_name = std::move(name); };
		void set_tag(std::string tag) 		{ m_tag = std::move(tag); };
		void set_uuid(UUID uuid) 			{ m_uuid = std::move(uuid); };

	protected:
		virtual void deserialize(ByteStream& bytes);

		TypeID m_type_id;
		GameObject* m_parent;
		std::vector<GameObject*> m_children;

	private:
		UUID m_uuid;
		std::string m_name;
		std::string m_tag;
		bool m_in_scene;
	};
} // namespace bacon
