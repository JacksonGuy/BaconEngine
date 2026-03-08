#include "core/game_object.h"

#include "editor/ui/editor_ui.h"
#include "util.h"

namespace bacon
{
	GameObject::GameObject()
	{
		m_uuid = UUID();
		m_name = "GameObject";
		m_tag = "Default";
		m_in_scene = false;
	}

	GameObject::GameObject(const GameObject& object)
	{
		copy(object);

		m_in_scene = false;
	}

	GameObject& GameObject::operator=(const GameObject& object)
	{
		copy(object);

		m_in_scene = false;

		return *this;
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
		json["uuid"] = m_uuid.as_string();
		json["name"] = m_name;
		json["tag"] = m_tag;
	}

	/**
	 * Loads the GameObject from a JSON object.
	 * WARNING: This does not load the children!
	 */
	void GameObject::load_from_json(const nlohmann::json& json)
	{
		m_uuid = UUID(json_read_string(json, "uuid"));
		m_name = json_read_string(json, "name");
		m_tag = json_read_string(json, "tag");
	}

	ByteStream GameObject::serialize() const
	{
		ByteStream bytes;

		bytes << m_uuid.as_string();
		bytes << m_name;
		bytes << m_tag;

		return bytes;
	}

	void GameObject::deserialize(ByteStream& bytes)
	{
		std::string uuid_str;
		bytes >> uuid_str;
		m_uuid = UUID(uuid_str);

		bytes >> m_name;
		bytes >> m_tag;
	}
}
