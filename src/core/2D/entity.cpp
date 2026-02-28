#include "entity.h"

#include "box2d/box2d.h"
#include "box2d/collision.h"
#include "box2d/types.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "raylib.h"
#include "raymath.h"

#include "core/game_state.h"
#include "core/globals.h"
#include "core/util.h"
#include "file/file.h"
#include "editor/editor_event.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"
#include "core/2D/game_object.h"

namespace bacon
{
	PoolAllocator<Entity> Entity::_allocator(globals::allocator_block_size);

	void* Entity::operator new(size_t size)
	{
		return Entity::_allocator.allocate();
	}

	void* Entity::operator new(size_t size, void* ptr)
	{
		return ptr;
	}

	void Entity::operator delete(void* ptr)
	{
		Entity::_allocator.deallocate((Entity*)ptr);
	}

	void Entity::operator delete(void* ptr, size_t size)
	{
		Entity::_allocator.deallocate((Entity*)ptr);
	}

	Entity::Entity() : GameObject()
	{
		this->name = "Entity";
		this->m_texture = {0};
		this->m_texture_path = "";
		this->physics_body = {0};
		this->body_type = BodyType::NONE;
	}

	Entity::Entity(uint8_t* bytes)
	{
		this->deserialize(bytes);
	}

	Entity::Entity(const Entity& entity) : GameObject()
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

		this->body_type = entity.body_type;
		this->set_texture(entity.m_texture_path);
	}

	Entity* Entity::clone() const
	{
		Entity* new_entity = new Entity(*this);
		return new_entity;
	}

	Entity* Entity::clone_unique() const
	{
		Entity* new_entity = new Entity(*this);
		new_entity->uuid = UUID();
		return new_entity;
	}

	void Entity::add_to_scene()
	{
		GameState::scene.add_entity(this);
		GameState::renderer->add_to_layer(this, layer);
	}

	void Entity::remove_from_scene()
	{
		GameState::scene.remove_entity(this);
		GameState::renderer->remove_from_layer(this);
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

	void Entity::update_ui_buffer()
	{
		base_update_ui_buffer();

		ui::obj_properties.texture_path = m_texture_path;
		ui::obj_properties.body_type = body_type;
	}

	void Entity::update_from_ui_buffer()
	{
		base_update_from_ui_buffer();

		this->set_size(size.x, size.y);
		this->set_texture(ui::obj_properties.texture_path);
		this->body_type = BodyType(ui::obj_properties.body_type);
	}

	void Entity::draw() const
	{
		if (!this->is_visible)
			return;

		Vector2 draw_pos = this->position;
		float draw_rot = this->rotation;
		if (parent != nullptr)
		{
			draw_pos = rotate_about_point(draw_pos, parent->position, parent->rotation);
			draw_rot += parent->rotation;
		}

		if (m_texture == nullptr)
		{
			DrawRectanglePro(
				{position.x, position.y, size.x, size.y},
				{size.x * 0.5f, size.y * 0.5f},
				this->rotation,
				RED);
		}
		else
		{
			DrawTexturePro(
				*m_texture,
				(Rectangle){0, 0, (float)m_texture->width, (float)m_texture->height},
				(Rectangle){draw_pos.x, draw_pos.y, size.x, size.y},
				{size.x * 0.5f, size.y * 0.5f},
				draw_rot,
				WHITE);
		}
	}

	void Entity::draw_properties_editor()
	{
		// WARNING!!
		using namespace event;

		Entity copy_entity(*this);
		// if (ui::inspect_object_copy->uuid != this->uuid)
		// {
		// 	delete ui::inspect_object_copy;

		// 	ui::inspect_object_copy = this->clone();
		// }

		bool change_made = draw_base_properties();

		// Texture
		ImGui::ItemLabel("Texture", ItemLabelFlag::Left);
		ImGui::InputText("##texture", &ui::obj_properties.texture_path);
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
				ui::obj_properties.texture_path = result.path;

				globals::has_unsaved_changes = true;
				change_made = true;
			}
		}

		ImGui::Separator();

		// Physics
		const char* body_options[] = {"None", "Static", "Dynamic", "Kinematic"};
		int current_body_option = ui::obj_properties.body_type;
		ImGui::ItemLabel("Physics Body", ItemLabelFlag::Left);
		if (ImGui::Combo("##physics_body", &current_body_option, body_options, 4))
		// if (ImGui::IsItemDeactivatedAfterEdit())
		{
			ui::obj_properties.body_type = BodyType(current_body_option);

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		if (change_made)
		{
			update_from_ui_buffer();

			ObjectEvent* event = new ObjectEvent(&copy_entity, this);
			event->object_uuid = this->uuid;
			push_event(event);
		}
	}

	void Entity::save_to_json(nlohmann::json& data) const
	{
		GameObject::save_to_json(data);

		data["type"] = "entity";
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
