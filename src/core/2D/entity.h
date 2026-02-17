#pragma once

#include <string>

#include "box2d/id.h"
#include "raylib.h"

#include "game_object.h"
#include "lib/pool_allocator.h"
#include "lib/arena.h"

namespace bacon
{
	enum BodyType : uint8_t
	{
		NONE = 0,
		STATIC,
		DYNAMIC,
		KINEMATIC
	};

	class Entity : public GameObject
	{
	public:
		friend class Scene2D;
		static Arena<Entity> _allocator;

		Entity();
		Entity(uint8_t* bytes);
		Entity(const Entity& entity);
		Entity& operator=(const Entity& entity);
		Entity(Entity&& entity) = delete;
		Entity& operator=(Entity&& entity) = delete;
		~Entity() = default;

		void copy(const GameObject& object) override;
		Entity* clone() const override;

		void add_to_state() override;

		void set_texture(const std::string& path);
		void set_size(float width, float height);

		void draw() const override;
		void draw_properties_editor() override;
		void save_to_json(nlohmann::json& data) const override;
		void load_from_json(nlohmann::json& data) override;
		size_t calculate_size() const override;
		uint8_t* serialize() const override;
		void deserialize(uint8_t* bytes) override;

	protected:
		b2BodyId physics_body;
		BodyType body_type;

		void create_body(b2WorldId world_id);

	private:
		std::shared_ptr<Texture2D> m_texture;
		std::string m_texture_path;
	};
} // namespace bacon
