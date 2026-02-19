#include "camera_object.h"

#include "core/2D/game_object.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#include "core/game_state.h"
#include "core/globals.h"
#include "core/util.h"
#include "editor/editor_event.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"
#include "lib/pool_allocator.h"

namespace bacon
{
	Arena<CameraObject> CameraObject::_allocator(globals::allocator_block_size);

	void* CameraObject::operator new(size_t size)
	{
	    return CameraObject::_allocator.allocate();
	}

	void CameraObject::operator delete(void* ptr, size_t size)
	{
	    CameraObject::_allocator.deallocate((CameraObject*)ptr);
	}

	void* CameraObject::operator new(size_t size, void* ptr)
	{
	    return ptr;
	}

	CameraObject::CameraObject() : GameObject()
	{
		this->object_type = ObjectType::CAMERA;
		this->name = "Camera";
		this->camera = {0};
		this->is_active = false;
		this->zoom = 1.0f;

		this->update_buffers();
	}

	CameraObject::CameraObject(uint8_t* bytes)
	{
		this->deserialize(bytes);
	}

	CameraObject::CameraObject(const CameraObject& camera) : GameObject(camera)
	{
		this->camera = camera.camera;
		this->is_active = false;
		this->zoom = camera.zoom;
	}

	CameraObject& CameraObject::operator=(const CameraObject& camera)
	{
	    GameObject::operator=(camera);

		this->camera = camera.camera;
		this->is_active = false;
		this->zoom = camera.zoom;

	    return *this;
	}

	void CameraObject::copy(const GameObject& object)
	{
	    GameObject::copy(object);

		const CameraObject& camera = static_cast<const CameraObject&>(object);

		this->camera = camera.camera;
		this->is_active = false;
		this->zoom = camera.zoom;
	}

	CameraObject* CameraObject::clone() const
	{
	    return new CameraObject(*this);
	}

	void CameraObject::add_to_state()
	{
	    GameState::scene.add_camera(this);
	}

	void CameraObject::move_camera(Vector2 delta)
	{
		this->position.x += delta.x;
		this->position.y += delta.y;
		this->camera.target = this->position;
	}

	void CameraObject::set_position(Vector2 position)
	{
		this->position = position;
		this->camera.target = position;
	}

	void CameraObject::calculate_size(Vector2 window_size)
	{
		this->size = {window_size.x * this->camera.zoom,
					  window_size.y * this->camera.zoom};
	}

	void CameraObject::update_buffers()
	{
	    update_base_buffers();

		m_buffers.is_active = is_active;
		m_buffers.zoom = zoom;
	}

	void CameraObject::update_from_buffers()
	{
	    update_from_base_buffers();

		is_active = m_buffers.is_active;
		zoom = m_buffers.zoom;

		if (this->is_active)
		{
		    GameState::scene.set_active_camera(this);
		}
		this->camera.target = this->position;
		this->camera.rotation = this->rotation;
		this->camera.zoom = zoom;
	}

	void CameraObject::draw() const {}

	void CameraObject::draw_properties_editor()
	{
	    // WARNING!!
		using namespace event;

	    CameraObject copy_camera(*this);

		bool change_made = draw_base_properties();

		ImGui::ItemLabel("Active", ItemLabelFlag::Left);
		if (ImGui::Checkbox("##is_active", &m_buffers.is_active))
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Zoom", ItemLabelFlag::Left);
		ImGui::InputFloat("##zoom", &m_buffers.zoom);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		if (change_made)
		{
		    update_from_buffers();

			ObjectEvent* event = new ObjectEvent(&copy_camera, this);
			event->object = this;
			event::push_event(event);
		}
	}

	void CameraObject::save_to_json(nlohmann::json& data) const
	{
		GameObject::save_to_json(data);

		data["is_active"] = this->is_active;
		data["zoom"] = this->zoom;
	}

	void CameraObject::load_from_json(nlohmann::json& data)
	{
		GameObject::load_from_json(data);

		for (nlohmann::json::iterator it = data.begin(); it != data.end(); it++)
		{
			std::string key = it.key();
			auto value = *it;

			if (key == "is_active")
			{
				this->is_active = value;
			}
			else if (key == "zoom")
			{
				this->zoom = value;
				this->camera.zoom = value;
			}
		}

		if (this->is_active)
		{
			GameState::scene.set_active_camera(this);
		}
		this->camera.target = this->position;
		this->camera.rotation = this->rotation;

		this->update_buffers();
	}

	size_t CameraObject::calculate_size() const
	{
		debug_error("This function has not been implemented yet.");
		return 0;
	}

	uint8_t* CameraObject::serialize() const
	{
		debug_error("This function has not been implemented yet.");
		return nullptr;
	}

	void CameraObject::deserialize(uint8_t* bytes)
	{
		debug_error("This function has not been implemented yet.");
	}
} // namespace bacon
