#include "game_object.h"

#include "imgui.h"
#include "imgui_stdlib.h"

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
		this->object_type = ObjectType::OBJECT;
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
        this->object_type = ObjectType::OBJECT;

        this->uuid = obj.uuid;
    	this->name = obj.name;
    	this->tag = obj.tag;
    	this->position = obj.position;
    	this->size = obj.size;
    	this->rotation = obj.rotation;
    	this->is_visible = obj.is_visible;
    	this->layer = obj.layer;

    	this->parent = obj.parent;
    	if (obj.parent != nullptr)
    	{
    		obj.parent->children.push_back(this);
    	}

        // TODO
        // const std::vector<GameObject*>& children = obj.get_children();
		// if (children.size() > 0)
		// {
		//     for (GameObject* child : children)
		// 	{
		// 	    GameObject* new_child = child->clone();
		// 		new_child->add_to_state();
		// 		this->children.push_back(new_child);
		// 	}
		// }
	}

	GameObject* GameObject::get_parent() const
	{
		return this->parent;
	}

	void GameObject::set_parent(GameObject* parent)
	{
	    this->parent = parent;
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

	const std::vector<GameObject*>& GameObject::get_children() const
	{
		return this->children;
	}

	const size_t GameObject::get_layer() const
	{
		return this->layer;
	}

	void GameObject::set_layer(size_t layer)
	{
		GameState::renderer->remove_from_layer(this);
		GameState::renderer->add_to_layer(this, layer);
	}

	void GameObject::update_base_buffers()
	{
	    m_buffers.name = name;
		m_buffers.tag = tag;
		m_buffers.position[0] = position.x;
		m_buffers.position[1] = position.y;
		m_buffers.size[0] = size.x;
		m_buffers.size[1] = size.y;
		m_buffers.rotation = rotation;
		m_buffers.is_visible = is_visible;
		m_buffers.layer = layer;
	}

	void GameObject::update_from_base_buffers()
	{
	    this->name = m_buffers.name;
		this->tag = m_buffers.tag;
		this->position = (Vector2){m_buffers.position[0], m_buffers.position[1]};
		this->size = (Vector2){m_buffers.size[0], m_buffers.size[1]};
		this->rotation = m_buffers.rotation;
		this->is_visible = m_buffers.is_visible;
		this->set_layer(m_buffers.layer);
	}

	bool GameObject::draw_base_properties()
	{
	    bool change_made = false;

	    // Name
		ImGui::ItemLabel("Name", ItemLabelFlag::Left);
		ImGui::InputText("##name", &m_buffers.name);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}
		// ImGui::SameLine();
		ImGui::HelpMarker("ID: " + this->uuid.get_uuid());

		// Tag
		ImGui::ItemLabel("Tag", ItemLabelFlag::Left);
		ImGui::InputText("##tag", &m_buffers.tag);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Rendering layer
		ImGui::ItemLabel("Layer", ItemLabelFlag::Left);
		ImGui::InputScalar("##layer", ImGuiDataType_U64, &m_buffers.layer);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Position
		ImGui::ItemLabel("Position", ItemLabelFlag::Left);
		ImGui::InputFloat2("##position", m_buffers.position);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Size
		ImGui::ItemLabel("Size", ItemLabelFlag::Left);
		ImGui::InputFloat2("##size", m_buffers.size);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Rotation
		ImGui::ItemLabel("Rotation", ItemLabelFlag::Left);
		ImGui::InputFloat("##rotation", &m_buffers.rotation);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			m_buffers.rotation = b_fmod(m_buffers.rotation, 360);

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Visibility
		ImGui::ItemLabel("Visible", ItemLabelFlag::Left);
		if (ImGui::Checkbox("##visible", &m_buffers.is_visible))
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
		data["type"] = object_type;
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
			else if (key == "type")
			{
				this->object_type = ObjectType(value);
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
				// this->manager->set_object_layer(this, this->layer);
			}
			else if (key == "parent")
			{
				if (value == "null")
				{
					this->parent = nullptr;
				}
				else
				{
					this->parent = GameState::scene.find_object_by_uuid(value);
				}
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
