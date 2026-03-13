#include "scene_2d.h"

#include <memory>

#include "box2d/box2d.h"
#include "box2d/id.h"

#include "core/2D/camera_object.h"
#include "core/game_state.h"
#include "core/util.h"
#include "raylib.h"

namespace bacon
{
	Scene2D::Scene2D()
	{
		m_camera = nullptr;

		m_length_units_per_meter = 128.0f;
		m_gravity = 9.8f * m_length_units_per_meter;

		this->create_physics_world();

		m_lua_state = std::make_unique<sol::state>();
	}

	const std::vector<Object2D*>& Scene2D::get_objects() const
	{
		return m_objects;
	}

	const std::vector<Entity*>& Scene2D::get_entities() const
	{
		return m_entities;
	}

	const std::vector<TextObject*>& Scene2D::get_text_objects() const
	{
		return m_text_objects;
	}

	const std::vector<CameraObject*>& Scene2D::get_cameras() const
	{
		return m_camera_objects;
	}

	void Scene2D::add_entity(Entity* entity)
	{
		this->m_objects.push_back(entity);
		this->m_entities.push_back(entity);
	}

	void Scene2D::remove_entity(Entity* entity)
	{
		if (!entity->get_in_scene()) return;

		bool found = false;

		// Remove from objects list
		for (auto it = m_objects.begin(); it != m_objects.end(); it++)
		{
			Object2D* object = *it;
			if (object->get_uuid() == entity->get_uuid())
			{
				m_objects.erase(it);
				found = true;
				break;
			}
		}
		if (!found)
		{
			return;
		}

		found = false;

		// Remove from entity list
		for (auto it = m_entities.begin(); it != m_entities.end(); it++)
		{
			Entity* ent = *it;
			if (ent->get_uuid() == entity->get_uuid())
			{
				m_entities.erase(it);
				found = true;
				break;
			}
		}
		if (!found)
		{
			return;
		}

		// Destroy physics body
		if (b2Body_IsValid(entity->get_body_id()))
		{
			entity->destroy_body();
		}

		// Remove from render layer
		if (GameState::state_2d != nullptr && GameState::state_2d->renderer != nullptr)
		{
			GameState::state_2d->renderer->remove_from_layer(entity);
		}
	}

	void Scene2D::add_text_object(TextObject* text)
	{
		m_objects.push_back(text);
		m_text_objects.push_back(text);
	}

	void Scene2D::remove_text_object(TextObject* text)
	{
		if (!text->get_in_scene()) return;

		bool found = false;

		// Remove from objects list
		for (auto it = m_objects.begin(); it != m_objects.end(); it++)
		{
			Object2D* object = *it;
			if (object->get_uuid() == text->get_uuid())
			{
				m_objects.erase(it);
				found = true;
				break;
			}
		}
		if (!found)
		{
			return;
		}

		found = false;

		// Remove from text objects list
		for (auto it = m_text_objects.begin(); it != m_text_objects.end(); it++)
		{
			TextObject* txt = *it;
			if (txt->get_uuid() == text->get_uuid())
			{
				m_text_objects.erase(it);
				found = true;
				break;
			}
		}
		if (!found)
		{
			return;
		}
	}

	void Scene2D::add_camera(CameraObject* camera)
	{
		m_objects.push_back(camera);
		m_camera_objects.push_back(camera);
	}

	void Scene2D::remove_camera(CameraObject* camera)
	{
		if (!camera->get_in_scene()) return;

		if (camera->is_active)
		{
			m_camera = nullptr;
		}

		bool found = false;

		// Remove from objects list
		for (auto it = m_objects.begin(); it != m_objects.end(); it++)
		{
			Object2D* object = *it;
			if (object->get_uuid() == camera->get_uuid())
			{
				m_objects.erase(it);
				found = true;
				break;
			}
		}
		if (!found)
		{
			return;
		}

		found = false;

		// Remove from camera objects list
		for (auto it = m_camera_objects.begin(); it != m_camera_objects.end();
			 it++)
		{
			CameraObject* cam = *it;
			if (cam->get_uuid() == camera->get_uuid())
			{
				m_camera_objects.erase(it);
				found = true;
				break;
			}
		}
		if (!found)
		{
			return;
		}
	}

	Object2D* Scene2D::find_object_by_uuid(std::string uuid) const
	{
		for (Object2D* object : m_objects)
		{
			if (object->get_uuid().as_string() == uuid)
			{
				return object;
			}
		}

		return nullptr;
	}

	Object2D* Scene2D::find_object_by_uuid(UUID uuid) const
	{
		for (Object2D* object : m_objects)
		{
			if (object->get_uuid() == uuid)
			{
				return object;
			}
		}

		return nullptr;
	}

	void Scene2D::set_active_camera(CameraObject* camera)
	{
		if (m_camera != nullptr)
		{
			m_camera->is_active = false;
		}

		camera->is_active = true;
		m_camera = camera;
	}

	CameraObject* Scene2D::get_active_camera() const
	{
		return m_camera;
	}

	void Scene2D::create_physics_bodies()
	{
		if (!b2World_IsValid(m_world))
		{
			this->create_physics_world();
		}

		for (Entity* entity : m_entities)
		{
			if (entity->get_body_type() != BodyType::NONE)
			{
				entity->create_body(m_world);
			}
		}
	}

	void Scene2D::create_physics_world()
	{
		if (b2World_IsValid(m_world))
		{
			b2DestroyWorld(m_world);
			m_world = b2_nullWorldId;
		}

		b2WorldDef world_def = b2DefaultWorldDef();

		world_def.gravity = (b2Vec2){0.0f, this->m_gravity};
		world_def.enableSleep = false;

		this->m_world = b2CreateWorld(&world_def);
	}

	float Scene2D::get_gravity() const
	{
		return this->m_gravity / this->m_length_units_per_meter;
	}

	void Scene2D::set_gravity(float gravity)
	{
		this->m_gravity = gravity * this->m_length_units_per_meter;

		b2World_SetGravity(m_world, (b2Vec2){0.0f, m_gravity});
	}

	float Scene2D::get_unit_length() const
	{
		return m_length_units_per_meter;
	}

	void Scene2D::set_unit_length(float pixels_per_meter)
	{
		m_length_units_per_meter = pixels_per_meter;

		set_gravity(m_gravity);
	}

	void Scene2D::simulation_step()
	{
		float delta_time = GetFrameTime();
		b2World_Step(this->m_world, delta_time, this->physics_steps);

		// Perform entity updates
		for (Entity* entity : this->m_entities)
		{
			if (entity->get_body_type() == BodyType::NONE)
				continue;

			b2Vec2 pos = b2Body_GetPosition(entity->get_body_id());
			b2Rot rotation = b2Body_GetRotation(entity->get_body_id());
			float radians = b2Rot_GetAngle(rotation);

			entity->set_position({pos.x, pos.y});
			entity->set_rotation(radians * RAD2DEG);
		}
	}

	void Scene2D::draw_entities(Camera2D* camera) const
	{
		assert(GameState::state_2d != nullptr);
		assert(GameState::state_2d->renderer != nullptr);

		if (camera != nullptr)
		{
			GameState::state_2d->renderer->draw(camera);
			return;
		}

		if (m_camera == nullptr)
		{
			debug_error("Scene does not have a camera.");
			return;
		}

		GameState::state_2d->renderer->draw(&m_camera->camera);
	}

	void Scene2D::reset()
	{
		if (GameState::state_2d != nullptr && GameState::state_2d->renderer != nullptr)
		{
			GameState::state_2d->renderer->reset();
		}

		// Delete objects
		// For some reason, using range-based for-loop
		// here causes a segfault, but using an iterator doesn't?
		for (auto it = m_objects.begin(); it != m_objects.end(); it++)
		{
			Object2D* object = *it;
			delete object;
		}
		m_objects.clear();
		m_entities.clear();
		m_camera_objects.clear();
		m_text_objects.clear();

		m_camera = nullptr;

		this->create_physics_world();
		m_lua_state.reset();
	}

	void Scene2D::cleanup()
	{
		// Destroy bodies
		for (Entity* entity : m_entities)
		{
			if (b2Body_IsValid(entity->get_body_id()))
			{
				b2DestroyBody(entity->get_body_id());
			}
		}
		b2DestroyWorld(m_world);

		// Delete objects
		for (GameObject* object : m_objects)
		{
			delete object;
		}

		m_objects.clear();
		m_entities.clear();
		m_camera_objects.clear();
		m_text_objects.clear();
	}
} // namespace bacon
