#pragma once

#include "sol/sol.hpp"

#include "camera_object.h"
#include "entity.h"
#include "game_object.h"
#include "text_object.h"

namespace bacon
{
    typedef struct ObjectIterator
    {

    } ObjectIterator;

	class Scene2D
	{
	public:
	    int physics_steps = 4;

		Scene2D();
		Scene2D(const Scene2D& scene) = delete;
		Scene2D& operator=(const Scene2D& scene) = delete;
		Scene2D(Scene2D&& scene) = delete;
		Scene2D& operator=(Scene2D&& scene) = delete;
		~Scene2D();

		const std::vector<GameObject*>& get_objects() const;
		const std::vector<Entity*>& get_entities() const;
		const std::vector<TextObject*>& get_text_objects() const;
		const std::vector<CameraObject*>& get_cameras() const;
		ObjectIterator get_iterator() const;

		void add_entity(Entity* entity);
		void remove_entity(Entity* entity);
		void add_text_object(TextObject* text);
		void remove_text_object(TextObject* text);
		void add_camera(CameraObject* camera);
		void remove_camera(CameraObject* camera);

		GameObject* find_object_by_uuid(std::string uuid) const;
		GameObject* find_object_by_uuid(UUID uuid) const;

		void set_active_camera(CameraObject* camera);
		CameraObject* get_active_camera() const;

		void create_physics_bodies();
		void create_physics_world();

		float get_gravity() const;
		void set_gravity(float gravity);
		float get_unit_length() const;
		void set_unit_length(float pixels_per_meter);

		void simulation_step();
		void draw_entities(Camera2D* camera = nullptr) const;

		void reset();

	private:
		std::vector<GameObject*> m_objects;
		std::vector<Entity*> m_entities;
		std::vector<TextObject*> m_text_objects;
		std::vector<CameraObject*> m_camera_objects;

		CameraObject* m_camera;

		b2WorldId m_world;
		float m_length_units_per_meter;
		float m_gravity;

		std::unique_ptr<sol::state> m_lua_state;
	};
} // namespace bacon
