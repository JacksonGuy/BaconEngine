#include "game_object.h"

#include "imgui.h"
#include "imgui_stdlib.h"
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
	GameObject::GameObject()
	{
		this->name = "Object";
		this->tag = "Default";
		this->position = {0.f, 0.f};
		this->size = {1.f, 1.f};
		this->rotation = 0.f;
		this->is_visible = true;
		this->layer = 0;

		this->parent = nullptr;
	}

	GameObject::GameObject(uint8_t* bytes)
	{
		this->deserialize(bytes);
	}

	GameObject::GameObject(const GameObject& obj)
	{
		this->copy(obj);
	}

	GameObject& GameObject::operator=(const GameObject& obj)
	{
		this->copy(obj);

		return *this;
	}

	void GameObject::copy(const GameObject& obj)
	{
		this->uuid = obj.uuid;
		this->name = obj.name;
		this->tag = obj.tag;
		set_position(obj.position);
		this->size = obj.size;
		this->rotation = obj.rotation;
		this->is_visible = obj.is_visible;

		// TODO this is a problem !!!
		// Just setting layer doesn't do anything when
		// we need a unique copy. But using set_layer
		// breaks functionality with inspecting object...
		// set_layer(obj.layer);
		this->layer = obj.layer;
	}

	GameObject* GameObject::get_parent() const
	{
		return this->parent;
	}

	void GameObject::set_parent(GameObject* new_parent)
	{
		if (new_parent == nullptr)
		{
			if (this->parent != nullptr)
			{
				this->parent->remove_child(this);
			}
			this->parent = nullptr;

			return;
		}

		this->parent = new_parent;
	}

	void GameObject::add_child(GameObject* object)
	{
		this->children.push_back(object);
	}

	void GameObject::remove_child(GameObject* object)
	{
		for (auto it = children.begin(); it != children.end(); it++)
		{
			GameObject* child = *it;
			if (child->uuid == object->uuid)
			{
				children.erase(it);
				globals::has_unsaved_changes = true;
				return;
			}
		}
	}

	void GameObject::delete_children()
	{
		auto it = children.begin();
		while (it != children.end())
		{
			GameObject* child = *it;

			child->remove_from_scene();
			child->delete_children();
			delete child;

			it = children.erase(it);
		}

		children.clear();
	}

	void GameObject::clone_children(const GameObject& object, bool add_to_scene)
	{
		for (GameObject* child : object.get_children())
		{
			// Clone child
			GameObject* new_child = nullptr;
			if (add_to_scene)
			{
				new_child = child->clone_unique();
			}
			else
			{
				new_child = child->clone();
			}

			// Set correct position
			Vector2 delta = Vector2Subtract(child->position, child->parent->position);
			Vector2 pos = Vector2Add(this->position, delta);
			new_child->set_position(pos);

			// Add to object
			this->children.push_back(new_child);
			new_child->set_parent(this);

			// Add to scene
			if (add_to_scene)
				new_child->add_to_scene();

			// Recurse down to copy all children below the
			// root object being copied.
			new_child->clone_children(*child, add_to_scene);
		}
	}

	const std::vector<GameObject*>& GameObject::get_children() const
	{
		return this->children;
	}

	Vector2 GameObject::get_position_abs() const
	{
		if (parent == nullptr)
		{
			return position;
		}

		return Vector2Add(position, parent->get_position_abs());
	}

	void GameObject::set_position(Vector2 position)
	{
		Vector2 delta = Vector2Subtract(position, this->position);
		this->position = position;
		update_child_positions(delta);
	}

	void GameObject::update_child_positions(Vector2 delta)
	{
		for (GameObject* child : children)
		{
			child->position = Vector2Add(child->position, delta);
		}
	}

	void GameObject::set_rotation(float rotation)
	{
		this->rotation = rotation;
	}

	void GameObject::set_visibility(bool visibility)
	{
		this->is_visible = visibility;

		for (GameObject* child : children)
		{
			child->set_visibility(visibility);
		}
	}

	size_t GameObject::get_layer() const
	{
		return this->layer;
	}

	void GameObject::set_layer(size_t layer)
	{
		GameState::renderer->remove_from_layer(this);
		GameState::renderer->add_to_layer(this, layer);
	}

	void GameObject::draw_outline() const
	{
		Vector2 draw_pos = this->position;
		float draw_rot = this->rotation;
		if (this->parent != nullptr)
		{
			draw_pos = rotate_about_point(draw_pos, parent->position, parent->rotation);
			draw_rot += parent->rotation;
		}

		DrawRectangleLinesPro(
			{
				draw_pos.x, //- (size.x / 2.f),
				draw_pos.y, //- (size.y / 2.f),
				size.x,
				size.y,
			},
			draw_rot,
			3.f,
			Color{0, 255, 0, 255});
	}

	bool GameObject::contains_point(Vector2 point)
	{
		Vector2 point_relative = Vector2Subtract(point, position);
		Vector2 p = Vector2Rotate(point_relative, -this->rotation * DEG2RAD);
		Rectangle rect = {
			-size.x / 2.f,
			-size.y / 2.f,
			size.x,
			size.y,
		};

		return CheckCollisionPointRec(p, rect);
	}

	void GameObject::base_update_ui_buffer()
	{
		ui::obj_properties.name = name;
		ui::obj_properties.tag = tag;
		ui::obj_properties.position[0] = position.x;
		ui::obj_properties.position[1] = position.y;
		ui::obj_properties.size[0] = size.x;
		ui::obj_properties.size[1] = size.y;
		ui::obj_properties.rotation = rotation;
		ui::obj_properties.is_visible = is_visible;
		ui::obj_properties.layer = layer;
	}

	void GameObject::base_update_from_ui_buffer()
	{
		this->name = ui::obj_properties.name;
		this->tag = ui::obj_properties.tag;

		Vector2 pos = {ui::obj_properties.position[0], ui::obj_properties.position[1]};
		this->set_position(pos);
		this->size = (Vector2){ui::obj_properties.size[0], ui::obj_properties.size[1]};
		this->set_rotation(ui::obj_properties.rotation);
		this->set_visibility(ui::obj_properties.is_visible);

		this->set_layer(ui::obj_properties.layer);
	}

	bool GameObject::draw_base_properties()
	{
		bool change_made = false;

		// Name
		ImGui::ItemLabel("Name", ItemLabelFlag::Left);
		ImGui::InputText("##name", &ui::obj_properties.name);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}
		// ImGui::SameLine();
		ImGui::HelpMarker("ID: " + this->uuid.get_uuid());

		// Tag
		ImGui::ItemLabel("Tag", ItemLabelFlag::Left);
		ImGui::InputText("##tag", &ui::obj_properties.tag);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Rendering layer
		ImGui::ItemLabel("Layer", ItemLabelFlag::Left);
		ImGui::InputScalar("##layer", ImGuiDataType_U64, &ui::obj_properties.layer);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Position
		ImGui::ItemLabel("Position", ItemLabelFlag::Left);
		ImGui::InputFloat2("##position", ui::obj_properties.position);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Size
		ImGui::ItemLabel("Size", ItemLabelFlag::Left);
		ImGui::InputFloat2("##size", ui::obj_properties.size);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Rotation
		ImGui::ItemLabel("Rotation", ItemLabelFlag::Left);
		ImGui::InputFloat("##rotation", &ui::obj_properties.rotation);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			ui::obj_properties.rotation = b_fmod(ui::obj_properties.rotation, 360);

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Visibility
		ImGui::ItemLabel("Visible", ItemLabelFlag::Left);
		if (ImGui::Checkbox("##visible", &ui::obj_properties.is_visible))
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::Separator();

		return change_made;
	}

	void GameObject::save_to_json(nlohmann::json& data) const
	{
		data["uuid"] = uuid.get_uuid();
		data["name"] = name;
		data["tag"] = tag;
		data["position"] = {position.x, position.y};
		data["size"] = {size.x, size.y};
		data["rotation"] = rotation;
		data["is_visible"] = is_visible;
		data["layer"] = layer;

		if (parent != nullptr)
		{
			data["parent"] = parent->uuid.get_uuid();
		}
		else
		{
			data["parent"] = "null";
		}
	}

	void GameObject::load_from_json(nlohmann::json& data)
	{
		for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++)
		{
			std::string key = it.key();
			auto value = *it;

			if (key == "uuid")
			{
				this->uuid = UUID(value);
			}
			else if (key == "name")
			{
				this->name = value;
			}
			else if (key == "tag")
			{
				this->tag = value;
			}
			else if (key == "position")
			{
				this->position = (Vector2){value[0], value[1]};
			}
			else if (key == "size")
			{
				this->size = (Vector2){value[0], value[1]};
			}
			else if (key == "rotation")
			{
				this->rotation = value;
			}
			else if (key == "is_visible")
			{
				this->is_visible = value;
			}
			else if (key == "layer")
			{
				this->layer = value;
			}
		}
	}

	size_t GameObject::calculate_size() const
	{
		debug_error("default calculate_size has not been implemented yet.");
		return 0;
	}

	uint8_t* GameObject::serialize() const
	{
		debug_error("default serialize has not been implemented yet.");
		return nullptr;
	}

	void GameObject::deserialize(uint8_t* bytes)
	{
		debug_error("default deserialize has not been implemented yet.");
	}
} // namespace bacon
