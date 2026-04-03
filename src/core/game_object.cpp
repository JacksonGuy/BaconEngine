#include "core/game_object.h"

#include "editor/ui/editor_ui.h"
#include "util.h"

namespace bacon
{
	GameObject* GameObject::create_game_object(ByteStream& bytes)
	{
		TypeID type_id;
		bytes >> type_id;

		if (type_id > TypeID::OBJECT_2D_START && type_id < TypeID::OBJECT_2D_END)
		{
			return Object2D::create_object_2d(bytes, type_id);
		}

		return nullptr;
	}

	GameObject::GameObject()
	{
		m_type_id = static_type_id;
		m_uuid = UUID();
		m_name = "GameObject";
		m_tag = "Default";
		m_in_scene = false;
		m_parent = nullptr;
	}

	GameObject::GameObject(const GameObject& object)
	{
		m_type_id = static_type_id;
		copy(object);

		m_in_scene = false;
	}

	void GameObject::destroy()
	{
		if (m_in_scene)
		{
			remove_from_scene();
			m_in_scene = false;
		}

		delete_children();
		if (m_parent != nullptr)
		{
			m_parent->remove_child(this);
			m_parent = nullptr;
		}
	}

	/**
	 * Copies the elements of the GameObject.
	 * This includes UUID!
 	 */
	void GameObject::copy(const GameObject& object)
	{
		m_uuid = object.get_uuid();
		m_name = object.get_name();
		m_tag = object.get_tag();
	}

	// Default implementations to avoid virtual function call
	// in destroy()
	void GameObject::add_to_scene() {};
	void GameObject::remove_from_scene() {};

	void GameObject::add_children_to_scene()
	{
		for (GameObject* child : m_children)
		{
			child->add_to_scene();
		}
	}

	void GameObject::remove_children_from_scene()
	{
		for (GameObject* child : m_children)
		{
			child->remove_from_scene();
		}
	}

	/**
	 * Sets the parent of the GameObject.
	 * This will remove the object from its current
	 * parent's child list.
	 */
	void GameObject::set_parent(GameObject* object)
	{
		if (object->get_uuid() == get_uuid())
		{
			return;
		}

		// Remove from current parent's child list.
		if (m_parent != nullptr)
		{
			m_parent->remove_child(this);
		}

		// Set new parent.
		m_parent = object;

		// Add to new parent's child list.
		if (object != nullptr)
		{
			object->add_child(this);
		}
	}

	/**
	 * Adds a new GameObject to children.
	 * Sets the passed GameObject's parent variable.
	 */
	void GameObject::add_child(GameObject* child)
	{
		m_children.push_back(child);
		child->m_parent = this;
	}

	/**
	 * Removes a GameObject from children.
	 * Sets the passed GameObject's parent variable.
	 */
	void GameObject::remove_child(GameObject* child)
	{
		for (auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			GameObject* object = *it;
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
	void GameObject::delete_children()
	{
		for (GameObject* child : m_children)
		{
			child->destroy();
			delete child;
		}

		m_children.clear();
	}

	void GameObject::clone_children(const GameObject& object, bool add_to_scene)
	{
		for (GameObject* child : object.get_children())
		{
			GameObject* clone = nullptr;
			if (add_to_scene)
			{
				clone = child->clone_unique();
			}
			else
			{
				clone = child->clone();
			}

			if (add_to_scene)
			{
				clone->add_to_scene();
			}
		}
	}

	void GameObject::update_ui_buffer() const
	{
		ui::obj_properties.name = m_name;
		ui::obj_properties.tag = m_tag;
	}

	void GameObject::update_from_ui_buffer()
	{
		m_name = ui::obj_properties.name;
		m_tag = ui::obj_properties.tag;
	}

	/**
	 * Saves the GameObject and its children to a JSON object.
	 */
	void GameObject::save_to_json(nlohmann::json& json) const
	{
		json["type_id"] = static_cast<uint32_t>(m_type_id);
		json["uuid"] = m_uuid.as_string();
		json["name"] = m_name;
		json["tag"] = m_tag;

		for (GameObject* child : m_children)
		{
			nlohmann::json child_data;
			child->save_to_json(child_data);
			json["children"].push_back(child_data);
		}
	}

	/**
	 * Loads the GameObject from a JSON object.
	 * WARNING: This does not load the children!
	 */
	void GameObject::load_from_json(const nlohmann::json& json)
	{
		m_type_id = TypeID(json_read_uint32(json, "type_id"));
		m_uuid = UUID(json_read_string(json, "uuid"));
		m_name = json_read_string(json, "name");
		m_tag = json_read_string(json, "tag");

		try {
			auto children = json.at("children");

			for (auto child : children)
			{
				TypeID type_id = TypeID(json_read_uint32(child, "type_id"));

				if (type_id == TypeID::ENTITY_2D)
				{
					Entity2D* entity = new Entity2D();
					entity->load_from_json(child);
					entity->add_to_scene();

					entity->set_parent(this);
				}
				else if (type_id == TypeID::TEXT_2D)
				{
					TextObject* text = new TextObject();
					text->load_from_json(child);
					text->add_to_scene();

					text->set_parent(this);
				}
				else if (type_id == TypeID::CAMERA_2D)
				{
					CameraObject* camera = new CameraObject();
					camera->load_from_json(child);
					camera->add_to_scene();

					camera->set_parent(this);
				}
				else
				{
					debug_error("Unknown child type: %lu", (uint32_t)type_id);
					continue;
				}
			}
		}
		catch (const nlohmann::json::out_of_range&)
		{
			// No children, do nothing
			return;
		}
		catch (const std::exception& e)
		{
			debug_log("Failed to load children: %s", e.what());
			return;
		}
	}

	ByteStream GameObject::serialize() const
	{
		ByteStream bytes;

		bytes << static_cast<uint32_t>(m_type_id);
		bytes << m_uuid.as_string();
		bytes << m_name;
		bytes << m_tag;

		bytes << m_children.size();
		for (GameObject* child : m_children)
		{
			bytes << child->serialize().raw();
		}

		return bytes;
	}

	void GameObject::deserialize(ByteStream& bytes)
	{
		std::string uuid_str;
		bytes >> uuid_str;
		m_uuid = UUID(uuid_str);

		bytes >> m_name;
		bytes >> m_tag;

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

			GameObject* child = GameObject::create_game_object(bytes);
			if (!child)
			{
				debug_error("Failed to deserialize child object!");
				return;
			}
			child->set_parent(this);
			this->add_child(child);
		}
	}
}
