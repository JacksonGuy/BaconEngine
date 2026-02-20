#include "entity.h"

#include "box2d/box2d.h"
#include "box2d/collision.h"
#include "box2d/types.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "raylib.h"

#include "core/game_state.h"
#include "core/globals.h"
#include "core/util.h"
#include "file/file.h"
#include "editor/editor_event.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"

namespace bacon
{
	Arena<Entity> Entity::_allocator(globals::allocator_block_size);

	void* Entity::operator new(size_t size)
	{
		return Entity::_allocator.allocate();
	}

	void Entity::operator delete(void* ptr, size_t size)
	{
		Entity::_allocator.deallocate((Entity*)ptr);
	}

	void* Entity::operator new(size_t size, void* ptr)
	{
		return ptr;
	}

	Entity::Entity() : GameObject()
	{
		this->object_type = ObjectType::ENTITY;
		this->name = "Entity";
		this->m_texture = {0};
		this->m_texture_path = "";
		this->physics_body = {0};
		this->body_type = BodyType::NONE;

		this->update_buffers();
	}

	Entity::Entity(uint8_t* bytes)
	{
		this->deserialize(bytes);
	}

	Entity::Entity(const Entity& entity) : GameObject(entity)
	{
		this->copy(entity);
	}

	Entity& Entity::operator=(const Entity& entity)
	{
		this->copy(entity);

		return *this;
	}

	void Entity::copy(const GameObject& object)
	{
		GameObject::copy(object);

		const Entity& entity = static_cast<const Entity&>(object);

		this->object_type = ObjectType::ENTITY;

		this->body_type = entity.body_type;
		this->set_texture(entity.m_texture_path);
	}

	Entity* Entity::clone() const
	{
		return new Entity(*this);
	}

	void Entity::add_to_state()
	{
		GameState::scene.add_entity(this);
	}

	void Entity::set_texture(const std::string& path)
	{
		if (path.length() <= 0)
		{
			m_texture = nullptr;
			m_texture_path = "";
			return;
		}

		m_texture_path = path;
		m_texture = GameState::assets.load_texture(path);
	}

	void Entity::set_size(float width, float height)
	{
		this->size = (Vector2){width, height};
	}

	void Entity::create_body(b2WorldId world_id)
	{
		b2BodyDef body_def = b2DefaultBodyDef();
		body_def.rotation = b2MakeRot(this->rotation * DEG2RAD);
		// body_def.position = (b2Vec2){
		//     this->position.x + (this->size.x / 2),
		//     this->position.y + (this->size.y / 2)
		// };
		body_def.position = (b2Vec2){this->position.x, this->position.y};
		b2Polygon box = b2MakeBox(this->size.x / 2, this->size.y / 2);

		switch (this->body_type)
		{
			case STATIC:
			{
				body_def.type = b2_staticBody;
				break;
			}

			case DYNAMIC:
			{
				body_def.type = b2_dynamicBody;
				break;
			}

			case KINEMATIC:
			{
				body_def.type = b2_kinematicBody;
				break;
			}

			case NONE:
			default:
			{
				break;
			}
		}

		this->physics_body = b2CreateBody(world_id, &body_def);
		b2ShapeDef shape = b2DefaultShapeDef();
		b2CreatePolygonShape(this->physics_body, &shape, &box);
	}

	void Entity::update_buffers()
	{
		update_base_buffers();

		m_buffers.texture_path = m_texture_path;
		m_buffers.body_type = body_type;
	}

	void Entity::update_from_buffers()
	{
		update_from_base_buffers();

		this->set_size(size.x, size.y);
		this->set_texture(m_buffers.texture_path);
		this->body_type = BodyType(m_buffers.body_type);
	}

	void Entity::draw() const
	{
		if (!this->is_visible)
			return;

		if (m_texture == nullptr)
		{
			DrawRectanglePro(
				{this->position.x, this->position.y, this->size.x, this->size.y},
				{this->size.x * 0.5f, this->size.y * 0.5f},
				this->rotation,
				RED);
		}
		else
		{
			DrawTexturePro(
				*m_texture,
				(Rectangle){0, 0, (float)m_texture->width, (float)m_texture->height},
				(Rectangle){position.x, position.y, size.x, size.y},
				{this->size.x * 0.5f, this->size.y * 0.5f},
				rotation,
				WHITE);
		}
	}

	void Entity::draw_properties_editor()
	{
		// WARNING!!
		using namespace event;

		Entity copy_entity(*this);

		bool change_made = draw_base_properties();

		// Texture
		ImGui::ItemLabel("Texture", ItemLabelFlag::Left);
		ImGui::InputText("##texture", &m_buffers.texture_path);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}
		if (ImGui::Button("Select"))
		{
			file::asset_t result = file::load_asset_nfd(file::AssetType::TEXTURE);
			if (result.type != file::AssetType::NONE)
			{
				m_buffers.texture_path = result.path;

				globals::has_unsaved_changes = true;
				change_made = true;
			}
		}

		ImGui::Separator();

		// Physics
		const char* body_options[] = {"None", "Static", "Dynamic", "Kinematic"};
		int current_body_option = m_buffers.body_type;
		ImGui::ItemLabel("Physics Body", ItemLabelFlag::Left);
		if (ImGui::Combo("##physics_body", &current_body_option, body_options, 4))
		// if (ImGui::IsItemDeactivatedAfterEdit())
		{
			m_buffers.body_type = BodyType(current_body_option);

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		if (change_made)
		{
			update_from_buffers();

			ObjectEvent* event = new ObjectEvent(&copy_entity, this);
			event->object = this;
			push_event(event);
		}
	}

	void Entity::save_to_json(nlohmann::json& data) const
	{
		GameObject::save_to_json(data);

		data["body_type"] = body_type;
		data["texture_path"] = m_texture_path;
	}

	void Entity::load_from_json(nlohmann::json& data)
	{
		GameObject::load_from_json(data);

		for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++)
		{
			std::string key = it.key();
			auto value = *it;

			if (key == "body_type")
			{
				body_type = BodyType(value);
			}
			else if (key == "texture_path")
			{
				set_texture(value);
			}
		}

		this->update_buffers();
	}

	size_t Entity::calculate_size() const
	{
		debug_error("This function has not been implemented yet.");
		return 0;
	}

	uint8_t* Entity::serialize() const
	{
		debug_error("This function has not been implemented yet.");
		return nullptr;
	}

	void Entity::deserialize(uint8_t* bytes)
	{
		debug_error("This function has not been implemented yet.");
	}
} // namespace bacon
