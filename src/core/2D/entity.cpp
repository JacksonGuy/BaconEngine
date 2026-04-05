#include "entity_2d.h"

#include "box2d/box2d.h"
#include "box2d/collision.h"
#include "box2d/id.h"
#include "box2d/types.h"
#include "core/2D/object_2d.h"
#include "core/game_object.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include "nlohmann/detail/value_t.hpp"
#include "raylib.h"
#include "raymath.h"

#include "core/game_state.h"
#include "core/globals.h"
#include "core/util.h"
#include "file/file.h"
#include "editor/editor_event.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"

namespace bacon
{
	PoolAllocator<Entity2D> Entity2D::_allocator(globals::allocator_block_size);

	void* Entity2D::operator new(size_t size)
	{
		return Entity2D::_allocator.allocate();
	}

	void* Entity2D::operator new(size_t size, void* ptr)
	{
		return ptr;
	}

	void Entity2D::operator delete(void* ptr)
	{
		Entity2D::_allocator.deallocate((Entity2D*)ptr);
	}

	void Entity2D::operator delete(void* ptr, size_t size)
	{
		Entity2D::_allocator.deallocate((Entity2D*)ptr);
	}

	Entity2D::Entity2D() : Object2D()
	{
		m_type_id = TypeID::ENTITY_2D;
		set_name("Entity");

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

	Entity2D::Entity2D(ByteStream& bytes) : Object2D()
	{
		m_type_id = TypeID::ENTITY_2D;
		deserialize(bytes);
	}

	Entity2D::Entity2D(const Entity2D& entity) : Object2D()
	{
		m_type_id = TypeID::ENTITY_2D;
		copy(entity);
	}

	void Entity2D::destroy()
	{
		Object2D::destroy();

		// Destroy physics body if it exists
		if (b2Body_IsValid(m_physics_body))
		{
			b2DestroyBody(m_physics_body);
		}
	}

	void Entity2D::copy(const GameObject& object)
	{
		Object2D::copy(object);

		const Entity2D& entity = static_cast<const Entity2D&>(object);

		this->m_physics_properties = entity.m_physics_properties;
		this->set_texture(entity.m_texture_path);

		m_lua_variables = entity.m_lua_variables;
		for (const std::string& path : entity.m_lua_script_paths)
		{
			// Adds path to list and creates script object
			load_lua_script(path);
		}
	}

	Entity2D* Entity2D::clone() const
	{
		Entity2D* new_entity = new Entity2D(*this);
		return new_entity;
	}

	Entity2D* Entity2D::clone_unique() const
	{
		Entity2D* new_entity = new Entity2D(*this);
		new_entity->set_uuid(UUID());
		return new_entity;
	}

	void Entity2D::add_to_scene()
	{
		if (get_in_scene()) return;

		GameState::state_2d->scene->add_entity(this);
		GameState::state_2d->renderer->add_to_layer(this, get_layer());
		set_in_scene(true);

		add_children_to_scene();
	}

	void Entity2D::remove_from_scene()
	{
		if (!get_in_scene()) return;

		GameState::state_2d->scene->remove_entity(this);

		remove_children_from_scene();
	}

	void Entity2D::set_texture(const std::string& path)
	{
		if (path.length() <= 0)
		{
			m_texture = nullptr;
			m_texture_path = "";
			return;
		}

		m_texture_path = path;
		if (GameState::state_2d != nullptr && GameState::state_2d->assets != nullptr)
		{
			m_texture = GameState::state_2d->assets->load_texture(path);
		}
	}

	void Entity2D::create_body(b2WorldId world_id)
	{
		b2BodyDef body_def = b2DefaultBodyDef();
		body_def.rotation = b2MakeRot(get_rotation() * DEG2RAD);
		body_def.position = (b2Vec2){get_position().x, get_position().y};
		b2Polygon box = b2MakeBox(get_size().x / 2, get_size().y / 2);

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

	void Entity2D::destroy_body()
	{
		b2DestroyBody(m_physics_body);
		m_physics_body = b2_nullBodyId;
	}

	void Entity2D::load_lua_script(const std::string& path)
	{
		sol::load_result result = GameState::state_2d->scene->lua_state->load_file(path);
		if (!result.valid())
		{
			debug_error("Failed to load script: %s", path.c_str());
			return;
		}

		m_lua_script_paths.push_back(path);
		m_lua_script_objects.push_back(result);
	}

	void Entity2D::create_lua_variable(const std::string& name, const LuaVar& var)
	{
		// m_lua_variables[name] = var;
		m_lua_variables.emplace(std::pair(name, var));
	}

	LuaVar* Entity2D::get_lua_variable(const std::string& name)
	{
		auto variable = m_lua_variables.find(name);
		if (variable == m_lua_variables.end())
		{
			return nullptr;
		}

		return &(variable->second);
	}

	void Entity2D::update_ui_buffer() const
	{
		Object2D::update_ui_buffer();

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

	void Entity2D::update_from_ui_buffer()
	{
		Object2D::update_from_ui_buffer();

		this->set_size({ui::obj_properties.size[0], ui::obj_properties.size[1]});
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

	void Entity2D::draw() const
	{
		if (!get_visible())
			return;

		Vector2 draw_pos = get_position();
		float draw_rot = get_rotation();
		Vector2 draw_size = get_size();

		Object2D* parent = dynamic_cast_to<Object2D>(get_parent());
		if (parent != nullptr)
		{
			draw_pos = rotate_about_point(draw_pos, parent->get_position(), parent->get_rotation());
			draw_rot += parent->get_rotation();
		}

		if (m_texture == nullptr)
		{
			DrawRectanglePro(
				{draw_pos.x, draw_pos.y, draw_size.x, draw_size.y},
				{draw_size.x * 0.5f, draw_size.y * 0.5f},
				draw_rot,
				RED);
		}
		else
		{
			DrawTexturePro(
				*m_texture,
				(Rectangle){0, 0, (float)m_texture->width, (float)m_texture->height},
				(Rectangle){draw_pos.x, draw_pos.y, draw_size.x, draw_size.y},
				{draw_size.x * 0.5f, draw_size.y * 0.5f},
				draw_rot,
				WHITE);
		}
	}

	void Entity2D::draw_properties_editor()
	{
		// WARNING!!
		using namespace event;

		if (ui::inspect_object_copy == nullptr ||
			ui::inspect_object_copy->get_uuid() != get_uuid())
		{
			delete ui::inspect_object_copy;
			ui::inspect_object_copy = this->clone();
		}

		Object2D::draw_properties_editor();

		// Texture
		ImGui::ItemLabel("Texture", ItemLabelFlag::Left);
		ImGui::InputText("##texture", &ui::obj_properties.texture_path);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}
		if (ImGui::Button("Select"))
		{
			file::asset_t result = file::load_asset_nfd(file::AssetType::TEXTURE);
			if (result.type != file::AssetType::NONE)
			{
				ui::obj_properties.texture_path = result.path;

				globals::has_unsaved_changes = true;
				ui::properties_changes_made = true;
			}
		}

		ImGui::Separator();

		static bool show_variable_create = false;
		if (ImGui::BeginTabBar("Properties"))
		{
			if (ImGui::BeginTabItem("Physics"))
			{
				const char* body_options[] = {"None", "Static", "Dynamic", "Kinematic"};
				int current_body_option = ui::obj_properties.body_type;
				ImGui::ItemLabel("Physics Body", ItemLabelFlag::Left);
				if (ImGui::Combo("##physics_body", &current_body_option, body_options, 4))
				{
					ui::obj_properties.body_type = BodyType(current_body_option);

					globals::has_unsaved_changes = true;
					ui::properties_changes_made = true;
				}

				ImGui::ItemLabel("Mass", ItemLabelFlag::Left);
				ImGui::InputFloat("##body_mass", &ui::obj_properties.mass);
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					globals::has_unsaved_changes = true;
					ui::properties_changes_made = true;
				}

				ImGui::ItemLabel("Density", ItemLabelFlag::Left);
				ImGui::InputFloat("##density", &ui::obj_properties.density);
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					globals::has_unsaved_changes = true;
					ui::properties_changes_made = true;
				}

				ImGui::ItemLabel("Friction", ItemLabelFlag::Left);
				ImGui::SliderFloat("##shape_friction", &ui::obj_properties.friction, 0.f, 1.f);
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					globals::has_unsaved_changes = true;
					ui::properties_changes_made = true;
				}

				ImGui::ItemLabel("Restitution", ItemLabelFlag::Left);
				ImGui::SliderFloat("##restitution", &ui::obj_properties.restitution, 0.f, 1.f);
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					globals::has_unsaved_changes = true;
					ui::properties_changes_made = true;
				}

				ImGui::ItemLabel("Body Center", ItemLabelFlag::Left);
				ImGui::InputFloat2("##body_center", ui::obj_properties.body_center);
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					globals::has_unsaved_changes = true;
					ui::properties_changes_made = true;
				}

				ImGui::ItemLabel("Rotational Inertia", ItemLabelFlag::Left);
				ImGui::InputFloat("##rotational_inertia", &ui::obj_properties.rotational_inertia);
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					globals::has_unsaved_changes = true;
					ui::properties_changes_made = true;
				}

				ImGui::ItemLabel("Linear Damping", ItemLabelFlag::Left);
				ImGui::SliderFloat("##linear_damping", &ui::obj_properties.linear_damping, 0.0f, 1.0f);
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					globals::has_unsaved_changes = true;
					ui::properties_changes_made = true;
				}

				ImGui::ItemLabel("Angular Damping", ItemLabelFlag::Left);
				ImGui::SliderFloat("##angular_damping", &ui::obj_properties.angular_damping, 0.0f, 1.0f);
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					globals::has_unsaved_changes = true;
					ui::properties_changes_made = true;
				}

				ImGui::ItemLabel("Gravity Scale", ItemLabelFlag::Left);
				ImGui::InputFloat("##gravity_scale", &ui::obj_properties.gravity_scale);
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					globals::has_unsaved_changes = true;
					ui::properties_changes_made = true;
				}

				ImGui::ItemLabel("Sleeping?", ItemLabelFlag::Left);
				ImGui::Checkbox("##is_sleeping", &ui::obj_properties.is_sleeping);
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					globals::has_unsaved_changes = true;
					ui::properties_changes_made = true;
				}

				ImGui::ItemLabel("Disabled?", ItemLabelFlag::Left);
				ImGui::Checkbox("##disabled", &ui::obj_properties.disabled);
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					globals::has_unsaved_changes = true;
					ui::properties_changes_made = true;
				}

				ImGui::ItemLabel("Is Bullet?", ItemLabelFlag::Left);
				ImGui::Checkbox("##is_bullet", &ui::obj_properties.is_bullet);
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					globals::has_unsaved_changes = true;
					ui::properties_changes_made = true;
				}

				ImGui::ItemLabel("Fixed Rotation", ItemLabelFlag::Left);
				ImGui::Checkbox("##fixed_rotation", &ui::obj_properties.fixed_rotation);
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					globals::has_unsaved_changes = true;
					ui::properties_changes_made = true;
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Scripting"))
			{
				for (auto it = m_lua_variables.begin(); it != m_lua_variables.end(); ++it)
				{
					const std::string& var_name = it->first;
					LuaVar& variable = it->second;

					ImGui::ItemLabel(var_name, ItemLabelFlag::Left);
					switch (variable.type)
					{
						case LuaVar_t::BOOL:
						{
							ImGui::Checkbox("##bool_val", &variable.bool_val);
							break;
						}

						case LuaVar_t::INT:
						{
							ImGui::InputInt("##int_val", &variable.int_val);
							break;
						}

						case LuaVar_t::FLOAT:
						{
							ImGui::InputFloat("##float_val", &variable.float_val);
							break;
						}

						case LuaVar_t::STRING:
						{
							ImGui::InputText("##string_val", &variable.str_val);
							break;
						}

						default:
						{
							break;
						}
					}
					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						globals::has_unsaved_changes = true;
						ui::properties_changes_made = true;
					}
				}

				if (ImGui::Button("Create Variable"))
				{
					show_variable_create = true;
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		if (show_variable_create)
		{
			static std::string variable_name;
			static int variable_type_select;
			const char* variable_types[] = {
				"String", "Float", "Integer", "Boolean",
			};

			ImGui::Begin("Create Variable", &show_variable_create);
				ImGui::ItemLabel("Variable Name", ItemLabelFlag::Left);
				ImGui::InputText("##variable_name", &variable_name);

				ImGui::ItemLabel("Variable Type", ItemLabelFlag::Left);
				ImGui::Combo("##variable_type", &variable_type_select, variable_types, 4);

				if (ImGui::Button("Create"))
				{
					LuaVar_t var_type = LuaVar_t(variable_type_select + 1);
					LuaVar new_variable;
					new_variable.type = var_type;

					m_lua_variables.emplace(std::pair(variable_name, new_variable));
					variable_name = "";

					globals::has_unsaved_changes = true;
					ui::properties_changes_made = true;
				}
			ImGui::End();
		}

		if (ui::properties_changes_made)
		{
			update_from_ui_buffer();

			ObjectEvent* event = new ObjectEvent(ui::inspect_object_copy, this);
			push_event(event);

			ui::inspect_object_copy->copy(*this);
			ui::properties_changes_made = false;
		}
	}

	void Entity2D::save_to_json(nlohmann::json& data) const
	{
		Object2D::save_to_json(data);

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

		for (auto it = m_lua_variables.begin(); it != m_lua_variables.end(); ++it)
		{
			const std::string& var_name = it->first;
			const LuaVar& variable = it->second;

			switch (variable.type)
			{
				case LuaVar_t::BOOL:
					data["variables"][var_name] = {
						{"type", LuaVar_t::BOOL},
						{"value", variable.bool_val},
					};
					break;

				case LuaVar_t::INT:
					data["variables"][var_name] = {
						{"type", LuaVar_t::INT},
						{"value", variable.int_val},
					};
					break;

				case LuaVar_t::FLOAT:
					data["variables"][var_name] = {
						{"type", LuaVar_t::FLOAT},
						{"value", variable.float_val},
					};
					break;

				case LuaVar_t::STRING:
					data["variables"][var_name] = {
						{"type", LuaVar_t::STRING},
						{"value", variable.str_val},
					};
					break;

				default:
					data["variables"][var_name] = {
						{"type", LuaVar_t::NONE},
						{"value", NULL}
					};
					break;
			}
		}
	}

	void Entity2D::load_from_json(const nlohmann::json& data)
	{
		Object2D::load_from_json(data);

		m_texture_path = json_read_string(data, "texture_path");
		set_texture(m_texture_path);

		m_physics_properties.type = BodyType(json_read_uint8(data, "body_type"));
		Vector2 body_center = json_read_vector2(data, "body_center");
		m_physics_properties.body_center[0] = body_center.x;
		m_physics_properties.body_center[1] = body_center.y;
		m_physics_properties.mass = json_read_float(data, "mass");
		m_physics_properties.density = json_read_float(data, "density");
		m_physics_properties.friction = json_read_float(data, "friction");
		m_physics_properties.restitution = json_read_float(data, "restitution");
		m_physics_properties.rotational_inertia = json_read_float(data, "rotational_inertia");
		m_physics_properties.linear_damping = json_read_float(data, "linear_damping");
		m_physics_properties.angular_damping = json_read_float(data, "angular_damping");
		m_physics_properties.gravity_scale = json_read_float(data, "gravity_scale");
		m_physics_properties.is_sleeping = json_read_bool(data, "is_asleep");
		m_physics_properties.fixed_rotation = json_read_bool(data, "fixed_rotation");
		m_physics_properties.is_bullet = json_read_bool(data, "is_bullet");

		if (data.contains("variables"))
		{
			for (auto it = data["variables"].begin(); it != data["variables"].end(); ++it)
			{
				const std::string& var_name = it.key();
				auto& parameters = *it;
				LuaVar_t var_type = parameters["type"];
				auto value = parameters["value"];

				if (var_type == LuaVar_t::NONE)
					continue;

				LuaVar new_variable;
				new_variable.type = var_type;
				switch (var_type)
				{
					case LuaVar_t::BOOL:
						new_variable.bool_val = value;
						break;

					case LuaVar_t::INT:
						new_variable.int_val = value;
						break;

					case LuaVar_t::FLOAT:
						new_variable.float_val = value;
						break;

					case LuaVar_t::STRING:
						new_variable.str_val = value;
						break;

					default:
						break;
				}
				m_lua_variables[var_name] = new_variable;
			}
		}
	}

	ByteStream Entity2D::serialize() const
	{
		ByteStream bytes = Object2D::serialize();

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

	void Entity2D::deserialize(ByteStream& bytes)
	{
		Object2D::deserialize(bytes);

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
