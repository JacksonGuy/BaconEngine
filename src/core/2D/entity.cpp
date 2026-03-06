#include "entity.h"

#include "box2d/box2d.h"
#include "box2d/collision.h"
#include "box2d/id.h"
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
		this->object_type = ObjectType::ENTITY;
		this->name = "Entity";
		m_texture = {0};
		m_texture_path = "";

		m_physics_body = {0};
		m_physics_shape = {0};

		m_physics_properties = {
			.type = BodyType::NONE,
			.body_center = {0.f, 0.f},
			.mass = 1.f,
			.density = 1.f,
			.friction = 0.f,
			.restitution = 0.f,
			.rotational_inertia = 0.f,
			.linear_damping = 0.f,
			.angular_damping = 0.f,
			.gravity_scale = 1.f,
			.is_sleeping = false,
			.disabled = false,
			.fixed_rotation = false,
			.is_bullet = false,
		};
	}

	Entity::Entity(ByteStream& bytes) : GameObject()
	{
		this->object_type = ObjectType::ENTITY;
		this->deserialize(bytes);
	}

	Entity::Entity(const Entity& entity) : GameObject()
	{
		this->object_type = ObjectType::ENTITY;
		this->copy(entity);
	}

	Entity& Entity::operator=(const Entity& entity)
	{
		this->object_type = ObjectType::ENTITY;
		this->copy(entity);

		return *this;
	}

	void Entity::destroy()
	{
		// Remove from scene (removes from render layer too)
		if (in_scene)
		{
			this->remove_from_scene();
		}

		// Destroy physics body if it exists
		if (b2Body_IsValid(m_physics_body))
		{
			b2DestroyBody(m_physics_body);
		}

		this->delete_children();
	}

	void Entity::copy(const GameObject& object)
	{
		GameObject::copy(object);

		const Entity& entity = static_cast<const Entity&>(object);

		this->m_physics_properties = entity.m_physics_properties;
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
		if (in_scene) return;

		GameState::scene.add_entity(this);
		GameState::renderer->add_to_layer(this, layer);
		in_scene = true;

		add_children_to_scene();
	}

	void Entity::remove_from_scene()
	{
		if (!in_scene) return;

		GameState::scene.remove_entity(this);
		GameState::renderer->remove_from_layer(this);
		in_scene = false;

		remove_children_from_scene();
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

		switch (m_physics_properties.type)
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

		body_def.linearDamping = m_physics_properties.linear_damping;
		body_def.angularDamping = m_physics_properties.angular_damping;
		body_def.gravityScale = m_physics_properties.gravity_scale;
		body_def.enableSleep = true;
		body_def.isAwake = !m_physics_properties.is_sleeping;
		body_def.motionLocks = {
			.linearX = false,
			.linearY = false,
			.angularZ = m_physics_properties.fixed_rotation,
		};
		body_def.isBullet = m_physics_properties.is_bullet;
		body_def.isEnabled = !m_physics_properties.disabled;

		b2MassData mass_data;
		mass_data.mass = m_physics_properties.mass;
		mass_data.center = (b2Vec2){
			m_physics_properties.body_center[0],
			m_physics_properties.body_center[1],
		};
		mass_data.rotationalInertia = m_physics_properties.rotational_inertia;

		m_physics_body = b2CreateBody(world_id, &body_def);
		b2Body_SetMassData(m_physics_body, mass_data);

		b2ShapeDef shape = b2DefaultShapeDef();
		shape.density = m_physics_properties.density;
		shape.material.friction = m_physics_properties.friction;
		shape.material.restitution = m_physics_properties.restitution;

		b2CreatePolygonShape(this->m_physics_body, &shape, &box);
	}

	void Entity::destroy_body()
	{
		b2DestroyBody(m_physics_body);
		m_physics_body = b2_nullBodyId;
	}

	void Entity::update_ui_buffer()
	{
		base_update_ui_buffer();

		ui::obj_properties.texture_path = m_texture_path;

		ui::obj_properties.body_type = m_physics_properties.type;
		ui::obj_properties.body_center[0] = m_physics_properties.body_center[0];
		ui::obj_properties.body_center[1] = m_physics_properties.body_center[1];
		ui::obj_properties.mass = m_physics_properties.mass;
		ui::obj_properties.density = m_physics_properties.density;
		ui::obj_properties.friction = m_physics_properties.friction;
		ui::obj_properties.restitution = m_physics_properties.restitution;
		ui::obj_properties.rotational_inertia = m_physics_properties.rotational_inertia;
		ui::obj_properties.linear_damping = m_physics_properties.linear_damping;
		ui::obj_properties.angular_damping = m_physics_properties.angular_damping;
		ui::obj_properties.gravity_scale = m_physics_properties.gravity_scale;
		ui::obj_properties.is_sleeping = m_physics_properties.is_sleeping;
		ui::obj_properties.fixed_rotation = m_physics_properties.fixed_rotation;
		ui::obj_properties.is_bullet = m_physics_properties.is_bullet;
	}

	void Entity::update_from_ui_buffer()
	{
		base_update_from_ui_buffer();

		this->set_size(size.x, size.y);
		this->set_texture(ui::obj_properties.texture_path);

		m_physics_properties = (PhysicsProperties){
			.type = BodyType(ui::obj_properties.body_type),
			.body_center = {
				ui::obj_properties.body_center[0],
				ui::obj_properties.body_center[1],
			},
			.mass = ui::obj_properties.mass,
			.density = ui::obj_properties.density,
			.friction = ui::obj_properties.friction,
			.restitution = ui::obj_properties.restitution,
			.rotational_inertia = ui::obj_properties.rotational_inertia,
			.linear_damping = ui::obj_properties.linear_damping,
			.angular_damping = ui::obj_properties.angular_damping,
			.gravity_scale = ui::obj_properties.gravity_scale,
			.is_sleeping = ui::obj_properties.is_sleeping,
			.fixed_rotation = ui::obj_properties.fixed_rotation,
			.is_bullet = ui::obj_properties.is_bullet,
		};
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

		if (ui::inspect_object_copy == nullptr ||
			ui::inspect_object_copy->uuid != this->uuid)
		{
			delete ui::inspect_object_copy;
			ui::inspect_object_copy = this->clone();
		}

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

		ImGui::BeginTabBar("Properties");
		// -----------------
		// Physics variables
		// -----------------
		ImGui::BeginTabItem("Physics");
		const char* body_options[] = {"None", "Static", "Dynamic", "Kinematic"};
		int current_body_option = ui::obj_properties.body_type;
		ImGui::ItemLabel("Physics Body", ItemLabelFlag::Left);
		if (ImGui::Combo("##physics_body", &current_body_option, body_options, 4))
		{
			ui::obj_properties.body_type = BodyType(current_body_option);

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Mass", ItemLabelFlag::Left);
		ImGui::InputFloat("##body_mass", &ui::obj_properties.mass);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Density", ItemLabelFlag::Left);
		ImGui::InputFloat("##density", &ui::obj_properties.density);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Friction", ItemLabelFlag::Left);
		ImGui::SliderFloat("##shape_friction", &ui::obj_properties.friction, 0.f, 1.f);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Restitution", ItemLabelFlag::Left);
		ImGui::SliderFloat("##restitution", &ui::obj_properties.restitution, 0.f, 1.f);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Body Center", ItemLabelFlag::Left);
		ImGui::InputFloat2("##body_center", ui::obj_properties.body_center);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Rotational Inertia", ItemLabelFlag::Left);
		ImGui::InputFloat("##rotational_inertia", &ui::obj_properties.rotational_inertia);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Linear Damping", ItemLabelFlag::Left);
		ImGui::SliderFloat("##linear_damping", &ui::obj_properties.linear_damping, 0.0f, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Angular Damping", ItemLabelFlag::Left);
		ImGui::SliderFloat("##angular_damping", &ui::obj_properties.angular_damping, 0.0f, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Gravity Scale", ItemLabelFlag::Left);
		ImGui::InputFloat("##gravity_scale", &ui::obj_properties.gravity_scale);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Sleeping?", ItemLabelFlag::Left);
		ImGui::Checkbox("##is_sleeping", &ui::obj_properties.is_sleeping);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Disabled?", ItemLabelFlag::Left);
		ImGui::Checkbox("##disabled", &ui::obj_properties.disabled);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Is Bullet?", ItemLabelFlag::Left);
		ImGui::Checkbox("##is_bullet", &ui::obj_properties.is_bullet);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Fixed Rotation", ItemLabelFlag::Left);
		ImGui::Checkbox("##fixed_rotation", &ui::obj_properties.fixed_rotation);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::EndTabItem();
		// -----------------

		ImGui::EndTabBar();

		if (change_made)
		{
			update_from_ui_buffer();

			ObjectEvent* event = new ObjectEvent(ui::inspect_object_copy, this);
			push_event(event);

			ui::inspect_object_copy->copy(*this);
		}
	}

	void Entity::save_to_json(nlohmann::json& data) const
	{
		GameObject::save_to_json(data);

		data["type"] = "entity";
		data["texture_path"] = m_texture_path;

		data["body_type"] = m_physics_properties.type;
		data["body_center"] = m_physics_properties.body_center;
		data["mass"] = m_physics_properties.mass;
		data["density"] = m_physics_properties.density;
		data["friction"] = m_physics_properties.friction;
		data["restituion"] = m_physics_properties.rotational_inertia;
		data["linear_damping"] = m_physics_properties.linear_damping;
		data["angular_damping"] = m_physics_properties.angular_damping;
		data["gravity_scale"] = m_physics_properties.gravity_scale;
		data["is_asleep"] = m_physics_properties.is_sleeping;
		data["fixed_rotation"] = m_physics_properties.fixed_rotation;
		data["is_bullet"] = m_physics_properties.is_bullet;
	}

	void Entity::load_from_json(nlohmann::json& data)
	{
		GameObject::load_from_json(data);

		for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++)
		{
			std::string key = it.key();
			auto value = *it;

			if (key == "texture_path")
			{
				set_texture(value);
			}
			else if (key == "body_type")
			{
				m_physics_properties.type = BodyType(value);
			}
			else if (key == "body_center")
			{
				m_physics_properties.body_center[0] = value[0];
				m_physics_properties.body_center[1] = value[1];
			}
			else if (key == "mass")
			{
				m_physics_properties.mass = value;
			}
			else if (key == "density")
			{
				m_physics_properties.density = value;
			}
			else if (key == "friction")
			{
				m_physics_properties.friction = value;
			}
			else if (key == "restitution")
			{
				m_physics_properties.restitution = value;
			}
			else if (key == "rotational_inertia")
			{
				m_physics_properties.rotational_inertia = value;
			}
			else if (key == "linear_damping")
			{
				m_physics_properties.linear_damping = value;
			}
			else if (key == "angular_damping")
			{
				m_physics_properties.angular_damping = value;
			}
			else if (key == "gravity_scale")
			{
				m_physics_properties.gravity_scale = value;
			}
			else if (key == "is_asleep")
			{
				m_physics_properties.is_sleeping = value;
			}
			else if (key == "fixed_rotation")
			{
				m_physics_properties.fixed_rotation = value;
			}
			else if (key == "is_bullet")
			{
				m_physics_properties.is_bullet = value;
			}
		}
	}

	size_t Entity::calculate_size() const
	{
		debug_error("This function has not been implemented yet.");
		return 0;
	}

	ByteStream Entity::serialize() const
	{
		ByteStream bytes = GameObject::serialize();

		bytes << m_texture_path;
		bytes << static_cast<uint8_t>(m_physics_properties.type);
		bytes << m_physics_properties.body_center[0];
		bytes << m_physics_properties.body_center[1];
		bytes << m_physics_properties.mass;
		bytes << m_physics_properties.density;
		bytes << m_physics_properties.friction;
		bytes << m_physics_properties.restitution;
		bytes << m_physics_properties.rotational_inertia;
		bytes << m_physics_properties.linear_damping;
		bytes << m_physics_properties.angular_damping;
		bytes << m_physics_properties.gravity_scale;
		bytes << m_physics_properties.is_sleeping;
		bytes << m_physics_properties.disabled;
		bytes << m_physics_properties.fixed_rotation;
		bytes << m_physics_properties.is_bullet;

		return bytes;
	}

	void Entity::deserialize(ByteStream& bytes)
	{
		GameObject::deserialize(bytes);

		std::string texture_path;
		bytes >> texture_path;
		this->set_texture(texture_path);

		uint8_t body_type;
		bytes >> body_type;
		m_physics_properties.type = static_cast<BodyType>(body_type);

		bytes >> m_physics_properties.body_center[0];
		bytes >> m_physics_properties.body_center[1];
		bytes >> m_physics_properties.mass;
		bytes >> m_physics_properties.density;
		bytes >> m_physics_properties.friction;
		bytes >> m_physics_properties.restitution;
		bytes >> m_physics_properties.rotational_inertia;
		bytes >> m_physics_properties.linear_damping;
		bytes >> m_physics_properties.angular_damping;
		bytes >> m_physics_properties.gravity_scale;
		bytes >> m_physics_properties.is_sleeping;
		bytes >> m_physics_properties.disabled;
		bytes >> m_physics_properties.fixed_rotation;
		bytes >> m_physics_properties.is_bullet;
	}
} // namespace bacon
