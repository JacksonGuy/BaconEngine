#include "scene_2d.h"

#include "box2d/box2d.h"

#include "box2d/id.h"
#include "core/2D/camera_object.h"
#include "core/game_state.h"
#include "core/util.h"
#include <memory>

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

	Scene2D::~Scene2D()
	{
		this->reset();
	}

	const std::vector<GameObject*>& Scene2D::get_objects() const
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
		bool found = false;

		// Remove from objects list
		for (auto it = m_objects.begin(); it != m_objects.end(); it++)
		{
			GameObject* object = *it;
			if (object->uuid == entity->uuid)
			{
				m_objects.erase(it);
				found = true;
				break;
			}
		}
		if (!found)
		{
			debug_error("Failed to remove entity from objects list!");
		}

		found = false;

		// Remove from entity list
		for (auto it = m_entities.begin(); it != m_entities.end(); it++)
		{
			Entity* ent = *it;
			if (ent->uuid == entity->uuid)
			{
				m_entities.erase(it);
				found = true;
				break;
			}
		}
		if (!found)
		{
			debug_error("Failed to remove entity from entity list!");
		}

		// Destroy physics body
		b2DestroyBody(entity->physics_body);
		entity->physics_body = b2_nullBodyId;

		// Remove from render layer
		GameState::renderer->remove_from_layer(entity);
	}

	void Scene2D::add_text_object(TextObject* text)
	{
		m_objects.push_back(text);
		m_text_objects.push_back(text);
	}

	void Scene2D::remove_text_object(TextObject* text)
	{
		bool found = false;

		// Remove from objects list
		for (auto it = m_objects.begin(); it != m_objects.end(); it++)
		{
			GameObject* object = *it;
			if (object->uuid == text->uuid)
			{
				m_objects.erase(it);
				found = true;
				break;
			}
		}
		if (!found)
		{
			debug_error("Failed to remove text object from objects list!");
		}

		found = false;

		// Remove from text objects list
		for (auto it = m_text_objects.begin(); it != m_text_objects.end(); it++)
		{
			TextObject* txt = *it;
			if (txt->uuid == text->uuid)
			{
				m_text_objects.erase(it);
				found = true;
				break;
			}
		}
		if (!found)
		{
			debug_error("Failed to remove text object from text objects list!");
		}
	}

	void Scene2D::add_camera(CameraObject* camera)
	{
		m_objects.push_back(camera);
		m_camera_objects.push_back(camera);
	}

	void Scene2D::remove_camera(CameraObject* camera)
	{
		if (camera->is_active)
		{
			m_camera = nullptr;
		}

		bool found = false;

		// Remove from objects list
		for (auto it = m_objects.begin(); it != m_objects.end(); it++)
		{
			GameObject* object = *it;
			if (object->uuid == camera->uuid)
			{
				m_objects.erase(it);
				found = true;
				break;
			}
		}
		if (!found)
		{
			debug_error("Failed to remove camera from objects list!");
		}

		found = false;

		// Remove from text objects list
		for (auto it = m_camera_objects.begin(); it != m_camera_objects.end();
			 it++)
		{
			CameraObject* cam = *it;
			if (cam->uuid == camera->uuid)
			{
				m_camera_objects.erase(it);
				found = true;
				break;
			}
		}
		if (!found)
		{
			debug_error("Failed to remove camera from cameras list!");
		}
	}

	GameObject* Scene2D::find_object_by_uuid(std::string uuid) const
	{
		// for (GameObject* object : m_objects)
		// {
		// 	if (object->uuid.get_uuid() == uuid)
		// 	{
		// 		return object;
		// 	}
		// }

		for (auto it = Entity::_allocator.start(); it != Entity::_allocator.end(); it = it->next)
		{
		    for (size_t i = 0; i < it->size; i++)
		    {
				Entity* entity = it->get(i);

				if (entity->uuid.get_uuid() == uuid)
				{
				    return entity;
				}
			}
		}

		for (auto it = TextObject::_allocator.start(); it != TextObject::_allocator.end(); it = it->next)
		{
		    for (size_t i = 0; i < it->size; i++)
		    {
				TextObject* text = it->get(i);

				if (text->uuid.get_uuid() == uuid)
				{
				    return text;
				}
			}
		}

		for (auto it = CameraObject::_allocator.start(); it != CameraObject::_allocator.end(); it = it->next)
		{
		    for (size_t i = 0; i < it->size; i++)
		    {
				CameraObject* camera = it->get(i);

				if (camera->uuid.get_uuid() == uuid)
				{
				    return camera;
				}
			}
		}

		return nullptr;
	}

	GameObject* Scene2D::find_object_by_uuid(UUID uuid) const
	{
		// for (GameObject* object : m_objects)
		// {
		// 	if (object->uuid == uuid)
		// 	{
		// 		return object;
		// 	}
		// }

		// debug_log("Attempting to find object with UUID: %s", uuid.get_uuid().c_str());

		for (auto it = Entity::_allocator.start(); it != Entity::_allocator.end(); it = it->next)
		{
		    for (size_t i = 0; i < it->size; i++)
		    {
				Entity* entity = it->get(i);

				// debug_log("Testing UUID: %s", entity->uuid.get_uuid().c_str());

				if (entity->uuid == uuid)
				{
				    return entity;
				}
			}
		}

		for (auto it = TextObject::_allocator.start(); it != TextObject::_allocator.end(); it = it->next)
		{
		    for (size_t i = 0; i < it->size; i++)
		    {
				TextObject* text = it->get(i);

				if (text->uuid == uuid)
				{
				    return text;
				}
			}
		}

		for (auto it = CameraObject::_allocator.start(); it != CameraObject::_allocator.end(); it = it->next)
		{
		    for (size_t i = 0; i < it->size; i++)
		    {
				CameraObject* camera = it->get(i);

				if (camera->uuid == uuid)
				{
				    return camera;
				}
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
			entity->create_body(m_world);
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

		this->create_physics_world();
	}

	void Scene2D::simulation_step()
	{
		float delta_time = GetFrameTime();
		b2World_Step(this->m_world, delta_time, 4);

		// Perform entity updates
		for (Entity* entity : this->m_entities)
		{
			b2Vec2 pos = b2Body_GetPosition(entity->physics_body);
			b2Rot rotation = b2Body_GetRotation(entity->physics_body);
			float radians = b2Rot_GetAngle(rotation);

			entity->position = {pos.x, pos.y};
			entity->rotation = radians * RAD2DEG;
		}
	}

	void Scene2D::draw_entities(Camera2D* camera) const
	{
		if (camera != nullptr)
		{
			GameState::renderer->draw(camera);
			return;
		}

		if (this->m_camera == nullptr)
		{
			debug_error("Scene does not have a camera.");
			return;
		}

		GameState::renderer->draw(&this->m_camera->camera);
	}

	void Scene2D::reset()
	{
		GameState::renderer->reset();

		m_camera = nullptr;
		this->create_physics_world();
		m_lua_state.reset();

		m_objects.clear();
		m_entities.clear();
		m_camera_objects.clear();
		m_text_objects.clear();
	}
} // namespace bacon
