#include "object_2d.h"

#include "imgui.h"
#include "imgui_stdlib.h"
#include "lib/byte_stream.h"
#include "nlohmann/detail/exceptions.hpp"
#include "raylib.h"
#include "raymath.h"

#include "core/game_state.h"
#include "core/globals.h"
#include "core/util.h"
#include "editor/editor_event.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"

namespace bacon
{
	Object2D* Object2D::create_object(ByteStream& bytes)
	{
		uint8_t type;
		bytes >> type;

		switch (ObjectType(type))
		{
			case ObjectType::ENTITY:
				return new Entity(bytes);
			case ObjectType::TEXT:
				return new TextObject(bytes);
			case ObjectType::CAMERA:
				return new CameraObject(bytes);
			default:
				debug_error("Unknown object type!");
				return nullptr;
		}
	}

	Object2D::Object2D() : GameObject()
	{
		set_name("Object2D");

		m_object_type = ObjectType::OBJECT;
		m_position = {0.f, 0.f};
		m_size = {1.f, 1.f};
		m_rotation = 0.f;
		m_is_visible = true;
		m_layer = 0;
	}

	Object2D::Object2D(const Object2D& obj) : GameObject()
	{
		this->copy(obj);
	}

	Object2D& Object2D::operator=(const Object2D& obj)
	{
		this->copy(obj);

		return *this;
	}

	void Object2D::destroy()
	{
		if (get_in_scene())
		{
			remove_from_scene();
		}

		delete_children();
	}

	void Object2D::copy(const GameObject& obj)
	{
		GameObject::copy(obj);

		const Object2D& object = static_cast<const Object2D&>(obj);
		m_object_type = object.m_object_type;
		m_position = object.m_position;
		m_size = object.m_size;
		m_rotation = object.m_rotation;
		m_is_visible = object.m_is_visible;
		m_layer = object.m_layer;
	}

	void Object2D::add_children_to_scene()
	{
		for (Object2D* child : m_children)
		{
			child->add_to_scene();
		}
	}

	void Object2D::remove_children_from_scene()
	{
		for (Object2D* child : m_children)
		{
			child->remove_from_scene();
		}
	}

	/**
	 * Sets the parent of the GameObject.
	 * This will remove the object from its current
	 * parent's child list.
	 */
	void Object2D::set_parent(Object2D* object)
	{
		if (object == nullptr || object->get_uuid() == get_uuid())
		{
			return;
		}

		m_parent->remove_child(this);
		m_parent = object;
	}

	/**
	 * Adds a new GameObject to children.
	 * Sets the passed GameObject's parent variable.
	 */
	void Object2D::add_child(Object2D* child)
	{
		m_children.push_back(child);
		child->m_parent = this;
	}

	/**
	 * Removes a GameObject from children.
	 * Sets the passed GameObject's parent variable.
	 */
	void Object2D::remove_child(Object2D* child)
	{
		for (auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			Object2D* object = *it;
			if (object->get_uuid() == child->get_uuid())
			{
				object->m_parent = nullptr;
				m_children.erase(it);
				return;
			}
		}
	}

	/**
	 * Destroys and deletes all children.
	 * (Removes from scene and frees memory for each child).
	 */
	void Object2D::delete_children()
	{
		auto it = m_children.begin();
		while (it != m_children.end())
		{
			Object2D* child = *it;

			child->destroy();
			delete child;

			it = m_children.erase(it);
		}

		m_children.clear();
	}

	void Object2D::clone_children(const Object2D& object, bool add_to_scene)
	{
		for (Object2D* child : object.get_children())
		{
			// Clone child
			Object2D* new_child = nullptr;
			if (add_to_scene)
			{
				new_child = child->clone_unique();
			}
			else
			{
				new_child = child->clone();
			}

			// Set correct position
			Vector2 delta = Vector2Subtract(child->m_position, child->m_parent->m_position);
			Vector2 pos = Vector2Add(this->m_position, delta);
			new_child->set_position(pos);

			// Add to object
			m_children.push_back(new_child);
			new_child->set_parent(this);

			// Add to scene
			if (add_to_scene)
				new_child->add_to_scene();

			// Recurse down to copy all children below the
			// root object being copied.
			new_child->clone_children(*child, add_to_scene);
		}
	}

	void Object2D::draw_outline() const
	{
		Vector2 draw_pos = m_position;
		float draw_rot = m_rotation;
		if (m_parent != nullptr)
		{
			draw_pos = rotate_about_point(draw_pos, m_parent->m_position, m_parent->m_rotation);
			draw_rot += m_parent->m_rotation;
		}

		DrawRectangleLinesPro(
			{
				draw_pos.x, //- (size.x / 2.f),
				draw_pos.y, //- (size.y / 2.f),
				m_size.x,
				m_size.y,
			},
			draw_rot,
			3.f,
			Color{0, 255, 0, 255});
	}

	bool Object2D::contains_point(Vector2 point)
	{
		Vector2 point_relative = Vector2Subtract(point, m_position);
		Vector2 p = Vector2Rotate(point_relative, -m_rotation * DEG2RAD);
		Rectangle rect = {
			-m_size.x / 2.f,
			-m_size.y / 2.f,
			m_size.x,
			m_size.y,
		};

		return CheckCollisionPointRec(p, rect);
	}

	void Object2D::update_ui_buffer() const
	{
		ui::obj_properties.name = get_name();
		ui::obj_properties.tag = get_tag();
		ui::obj_properties.position[0] = m_position.x;
		ui::obj_properties.position[1] = m_position.y;
		ui::obj_properties.size[0] = m_size.x;
		ui::obj_properties.size[1] = m_size.y;
		ui::obj_properties.rotation = m_rotation;
		ui::obj_properties.is_visible = m_is_visible;
		ui::obj_properties.layer = m_layer;
	}

	void Object2D::update_from_ui_buffer()
	{
		set_name(ui::obj_properties.name);
		set_tag(ui::obj_properties.tag);

		set_position({ui::obj_properties.position[0], ui::obj_properties.position[1]});
		set_size({ui::obj_properties.size[0], ui::obj_properties.size[1]});
		set_rotation(ui::obj_properties.rotation);
		set_visibility(ui::obj_properties.is_visible);

		if (get_in_scene())
		{
			GameState::state_2d->renderer->remove_from_layer(this);
			GameState::state_2d->renderer->add_to_layer(this, m_layer);
		}
		else
		{
			GameState::state_2d->renderer->add_to_layer(this, m_layer);
		}
	}

	void Object2D::draw_properties_editor()
	{
		// Name
		ImGui::ItemLabel("Name", ItemLabelFlag::Left);
		ImGui::InputText("##name", &ui::obj_properties.name);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}
		// ImGui::SameLine();
		ImGui::HelpMarker("ID: " + this->get_uuid().as_string());

		// Tag
		ImGui::ItemLabel("Tag", ItemLabelFlag::Left);
		ImGui::InputText("##tag", &ui::obj_properties.tag);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}

		// Rendering layer
		ImGui::ItemLabel("Layer", ItemLabelFlag::Left);
		ImGui::InputScalar("##layer", ImGuiDataType_U64, &ui::obj_properties.layer);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}

		// Position
		ImGui::ItemLabel("Position", ItemLabelFlag::Left);
		ImGui::InputFloat2("##position", ui::obj_properties.position);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}

		// Size
		ImGui::ItemLabel("Size", ItemLabelFlag::Left);
		ImGui::InputFloat2("##size", ui::obj_properties.size);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}

		// Rotation
		ImGui::ItemLabel("Rotation", ItemLabelFlag::Left);
		ImGui::InputFloat("##rotation", &ui::obj_properties.rotation);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			ui::obj_properties.rotation = b_fmod(ui::obj_properties.rotation, 360);

			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}

		// Visibility
		ImGui::ItemLabel("Visible", ItemLabelFlag::Left);
		if (ImGui::Checkbox("##visible", &ui::obj_properties.is_visible))
		{
			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}

		ImGui::Separator();
	}

	void Object2D::save_to_json(nlohmann::json& data) const
	{
		GameObject::save_to_json(data);

		data["object_type"] = static_cast<uint8_t>(m_object_type);
		data["position"] = {m_position.x, m_position.y};
		data["size"] = {m_size.x, m_size.y};
		data["rotation"] = m_rotation;
		data["is_visible"] = m_is_visible;
		data["layer"] = m_layer;

		for (Object2D* child : m_children)
		{
			nlohmann::json child_data;
			child->save_to_json(child_data);
			data["children"].push_back(child_data);
		}
	}

	void Object2D::load_from_json(const nlohmann::json& data)
	{
		GameObject::load_from_json(data);

		m_object_type = static_cast<ObjectType>(json_read_uint8(data, "object_type"));
		m_position = json_read_vector2(data, "position");
		m_size = json_read_vector2(data, "size");
		m_rotation = json_read_float(data, "rotation");
		m_is_visible = json_read_bool(data, "is_visible");
		m_layer = json_read_size_t(data, "layer");

		auto children = data["children"];
		for (auto child : children)
		{
			if (child["type"] == "entity")
			{
				Entity* entity = new Entity();
				entity->load_from_json(child);
				entity->add_to_scene();

				entity->set_parent(this);
				this->add_child(entity);
			}
			else if (child["type"] == "text")
			{
				TextObject* text = new TextObject();
				text->load_from_json(child);
				text->add_to_scene();

				text->set_parent(this);
				this->add_child(text);
			}
			else if (child["type"] == "camera")
			{
				CameraObject* camera = new CameraObject();
				camera->load_from_json(child);
				camera->add_to_scene();

				camera->set_parent(this);
				this->add_child(camera);
			}
			else
			{
				debug_error("Unknown child type!");
				continue;
			}
		}
	}

	ByteStream Object2D::serialize() const
	{
		ByteStream stream;

		stream << static_cast<uint8_t>(m_object_type);

		ByteStream base_data = GameObject::serialize();
		stream << base_data.raw();

		stream << m_position.x << m_position.y;
		stream << m_size.x << m_size.y;
		stream << m_rotation;
		stream << m_is_visible;
		stream << m_layer;

		stream << m_children.size();
		for (GameObject* child : m_children)
		{
			stream << child->serialize().raw();
		}

		return stream;
	}

	void Object2D::deserialize(ByteStream& bytes)
	{
		GameObject::deserialize(bytes);

		bytes >> m_position.x >> m_position.y;
		bytes >> m_size.x >> m_size.y;
		bytes >> m_rotation;
		bytes >> m_is_visible;
		bytes >> m_layer;

		size_t child_count = 0;
		bytes >> child_count;
		m_children.reserve(child_count);
		for (size_t i = 0; i < child_count; ++i)
		{
			// When we write byte arrays to ByteStream,
			// we include the size of the array first.
			// That isn't needed here, so just ignore this.
			size_t child_size;
			bytes >> child_size;

			Object2D* child = Object2D::create_object(bytes);
			if (!child)
			{
				debug_error("Failed to deserialize child object!");
				return;
			}
			child->set_parent(this);
			this->add_child(child);
		}
	}

	void Object2D::set_position(Vector2 position)
	{
		Vector2 delta = Vector2Subtract(position, m_position);
		m_position = position;
		update_child_positions(delta);
	}

	void Object2D::set_size(Vector2 size)
	{
		m_size = size;
	}

	void Object2D::set_rotation(float rotation)
	{
		m_rotation = rotation;
	}

	void Object2D::set_visibility(bool visibility)
	{
		m_is_visible = visibility;

		for (Object2D* child : m_children)
		{
			child->set_visibility(visibility);
		}
	}

	void Object2D::set_layer(size_t layer)
	{
		m_layer = layer;
		if (get_in_scene())
		{
			GameState::state_2d->renderer->remove_from_layer(this);
			GameState::state_2d->renderer->add_to_layer(this, layer);
		}
		else
		{
			GameState::state_2d->renderer->add_to_layer(this, layer);
		}
	}

	void Object2D::update_child_positions(Vector2 delta)
	{
		for (Object2D* child : m_children)
		{
			child->m_position = Vector2Add(child->m_position, delta);
		}
	}
} // namespace bacon
