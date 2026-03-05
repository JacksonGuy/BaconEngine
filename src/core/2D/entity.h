#pragma once

#include <string>

#include "box2d/id.h"
#include "raylib.h"

#include "game_object.h"
#include "lib/pool_allocator.h"

namespace bacon
{
	enum BodyType : uint8_t
	{
		NONE = 0,
		STATIC,
		DYNAMIC,
		KINEMATIC
	};

	typedef struct
	{
		BodyType type;
		float body_center[2];
		float mass;
		float density;
		float friction;
		float restitution;
		float rotational_inertia;
		float linear_damping;
		float angular_damping;
		float gravity_scale;
		bool is_sleeping;
		bool disabled;
		bool fixed_rotation;
		bool is_bullet;
	} PhysicsProperties;

	class Entity : public GameObject
	{
	public:
		static PoolAllocator<Entity> _allocator;
		static void* operator new(size_t size);
		static void* operator new(size_t size, void* ptr);
		static void operator delete(void* ptr);
		static void operator delete(void* ptr, size_t size);

		Entity();
		Entity(uint8_t* bytes);
		Entity(const Entity& entity);
		Entity& operator=(const Entity& entity);
		Entity(Entity&& entity) = delete;
		Entity& operator=(Entity&& entity) = delete;
		~Entity();

		void copy(const GameObject& object) override;
		Entity* clone() const override;
		Entity* clone_unique() const override;

		void add_to_scene() override;
		void remove_from_scene() override;

		void set_texture(const std::string& path);
		void set_size(float width, float height);

		b2BodyId get_body_id() const { return m_physics_body; }
		b2ShapeId get_shape_id() const { return m_physics_shape; }
		BodyType get_body_type() const { return m_physics_properties.type; }
		void create_body(b2WorldId world_id);
		void destroy_body();

		void update_ui_buffer() override;
		void update_from_ui_buffer() override;

		void draw() const override;
		void draw_properties_editor() override;
		void save_to_json(nlohmann::json& data) const override;
		void load_from_json(nlohmann::json& data) override;
		size_t calculate_size() const override;
		uint8_t* serialize() const override;
		void deserialize(uint8_t* bytes) override;

	private:
		std::shared_ptr<Texture2D> m_texture;
		std::string m_texture_path;
		b2BodyId m_physics_body;
		b2ShapeId m_physics_shape;
		PhysicsProperties m_physics_properties;
	};
} // namespace bacon
