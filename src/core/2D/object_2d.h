#pragma once

#include <cstdint>
#include <stdint.h>

#include <string>
#include "raylib.h"

#include "core/uuid.h"
#include "core/game_object.h"
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

	class Object2D : public GameObject
	{
	public:
		friend class Renderer;

		static Object2D* create_object(ByteStream& bytes);

		Object2D();
		Object2D(const Object2D& obj);
		Object2D& operator=(const Object2D& obj);
		Object2D(Object2D&& obj) = delete;
		Object2D& operator=(Object2D&& obj) = delete;
		virtual ~Object2D() = 0;

		virtual void destroy() override;
		virtual void copy(const GameObject& object) override;
		virtual Object2D* clone() const override;
		virtual Object2D* clone_unique() const override;

		void add_children_to_scene() override;
		void remove_children_from_scene() override;

		Object2D* get_parent() const { return m_parent; };
		void set_parent(Object2D* parent);
		const std::vector<Object2D*>& get_children() const { return m_children; };
		void add_child(Object2D* child);
		void remove_child(Object2D* child);
		void delete_children();
		void clone_children(const Object2D& obj, bool add_to_scene = true);

		virtual void draw_outline() const;
		virtual bool contains_point(Vector2 point);

		virtual void update_ui_buffer() const override;
		virtual void update_from_ui_buffer() override;

		virtual void draw() const = 0;
		virtual void draw_properties_editor() override;
		virtual void save_to_json(nlohmann::json& data) const override;
		virtual void load_from_json(const nlohmann::json& data) override;
		virtual ByteStream serialize() const override;

		void set_position(Vector2 position);
		void set_size(Vector2 size);
		void set_rotation(float rotation);
		void set_visibility(bool visibility);
		void set_layer(size_t layer);
		void update_child_positions(Vector2 delta);
		Vector2 get_position() const 	{ return m_position; }
		Vector2 get_size() const 		{ return m_size; }
		float get_rotation() const 		{ return m_rotation; }
		bool get_visible() const 		{ return m_is_visible; }
		size_t get_layer() const 		{ return m_layer; }

	protected:
		virtual void deserialize(ByteStream& bytes) override;

		ObjectType m_object_type;

	private:
		Vector2 m_position;
		Vector2 m_size;
		float m_rotation;
		bool m_is_visible;
		size_t m_layer;
		Object2D* m_parent;
		std::vector<Object2D*> m_children;
	};
} // namespace bacon
