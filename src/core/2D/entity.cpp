#include "entity.h"

#include "box2d/box2d.h"
#include "box2d/collision.h"
#include "box2d/types.h"
#include "core/game_state.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "raylib.h"

#include "core/globals.h"
#include "core/util.h"
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
		    return;
		}

		this->m_texture = GameState::assets.load_texture(path);
		m_texture_path = path;
	}

	void Entity::set_size(float width, float height)
	{
		this->size = (Vector2){width, height};

		// New size means we have to update texture?
		// this->set_texture(this->m_texture_path.c_str());
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

	void Entity::draw() const
	{
		if (!this->is_visible)
			return;

		// Vector2 pos = {b_pos.x, b_pos.y};
		// DrawTextureEx(
		//     this->m_texture,
		//     pos,
		//     RAD2DEG * b_radians,
		//     1.f,
		//     WHITE
		// );

		DrawRectanglePro(
			{this->position.x, this->position.y, this->size.x, this->size.y},
			{this->size.x * 0.5f, this->size.y * 0.5f}, this->rotation, RED);
	}

	void Entity::draw_properties_editor()
	{
	    Entity copy_entity(*this);

		bool change_made = false;

		// Name
		std::string name_buf = this->name;
		ImGui::ItemLabel("Name", ItemLabelFlag::Left);
		if (ImGui::InputText("##name", &name_buf,
							 ImGuiInputTextFlags_EnterReturnsTrue))
		{
			this->name = std::string(name_buf);

			globals::has_unsaved_changes = true;
			change_made = true;
		}
		ImGui::SameLine();
		ImGui::HelpMarker("ID: " + this->uuid.get_uuid());

		// Tag
		std::string tag_buf = this->tag;
		ImGui::ItemLabel("Tag", ItemLabelFlag::Left);
		ImGui::InputText("##tag", &tag_buf);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			this->tag = std::string(tag_buf);

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Rendering layer
		int render_layer = this->layer;
		ImGui::ItemLabel("Layer", ItemLabelFlag::Left);
		// TODO this might be a better choice?
		// ImGui::InputScalar("##layer", ImGuiDataType_U64, &render_layer);
		if (ImGui::InputInt("##layer", &render_layer))
		{
			if (render_layer < 0)
			{
				render_layer = 0;
			}

			this->set_layer(render_layer);

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Position
		float position[] = {this->position.x, this->position.y};
		ImGui::ItemLabel("Position", ItemLabelFlag::Left);
		if (ImGui::InputFloat2("##position", position))
		{
			this->position = (Vector2){position[0], position[1]};

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Size
		float size[] = {this->size.x, this->size.y};
		ImGui::ItemLabel("Size", ItemLabelFlag::Left);
		if (ImGui::InputFloat2("##size", size))
		{
			this->set_size(size[0], size[1]);

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Rotation
		ImGui::ItemLabel("Rotation", ItemLabelFlag::Left);
		if (ImGui::InputFloat("##rotation", &this->rotation))
		{
			this->rotation = b_fmod(this->rotation, 360);

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Visibility
		ImGui::ItemLabel("Visible", ItemLabelFlag::Left);
		if (ImGui::Checkbox("##visible", &this->is_visible))
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::Separator();

		// Texture
		ImGui::ItemLabel("Texture", ItemLabelFlag::Left);
		std::string texture_path_buf = this->m_texture_path;
		if (ImGui::InputText("##texture", &texture_path_buf))
		{
			this->m_texture_path = texture_path_buf;
			this->set_texture(m_texture_path.c_str());

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::Separator();

		// Physics
		const char* body_options[] = {"None", "Static", "Dynamic", "Kinematic"};
		int current_body_option = this->body_type;
		ImGui::Combo("Physics Body", &current_body_option, body_options, 4);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			this->body_type = BodyType(current_body_option);

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		if (change_made)
		{
			event::ObjectEvent* event = new event::ObjectEvent(&copy_entity, this);
			event::push_event(event);
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
				this->body_type = BodyType(value);
			}
			else if (key == "texture_path")
			{
				this->m_texture_path = value;

				// This calls set_texture() with m_texture_path
				if (this->m_texture_path.length() > 0)
				{
					this->set_size(this->size.x, this->size.y);
				}
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
