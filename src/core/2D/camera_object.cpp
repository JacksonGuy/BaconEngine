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

	void* CameraObject::operator new(size_t size, void* ptr)
	{
		return ptr;
	}

	void CameraObject::operator delete(void* ptr)
	{
		CameraObject::_allocator.deallocate((CameraObject*)ptr);
	}

	void CameraObject::operator delete(void* ptr, size_t size)
	{
		CameraObject::_allocator.deallocate((CameraObject*)ptr);
	}

	CameraObject::CameraObject() : GameObject()
	{
		this->name = "Camera";
		this->camera = {0};
		this->is_active = false;
		this->zoom = 1.0f;
	}

	CameraObject::CameraObject(uint8_t* bytes)
	{
		this->deserialize(bytes);
	}

	CameraObject::CameraObject(const CameraObject& camera) : GameObject()
	{
		this->copy(camera);
	}

	CameraObject& CameraObject::operator=(const CameraObject& camera)
	{
		this->copy(camera);

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
		CameraObject* new_camera = new CameraObject(*this);
		return new_camera;
	}

	CameraObject* CameraObject::clone_unique() const
	{
		CameraObject* new_camera = new CameraObject(*this);
		new_camera->uuid = UUID();
		return new_camera;
	}

	void CameraObject::add_to_scene()
	{
		GameState::scene.add_camera(this);
		GameState::renderer->add_to_layer(this, layer);
	}

	void CameraObject::remove_from_scene()
	{
		GameState::scene.remove_camera(this);
		GameState::renderer->remove_from_layer(this);
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

	void CameraObject::adjust_frame_size(Vector2 window_size)
	{
		this->size = {window_size.x * this->camera.zoom,
					  window_size.y * this->camera.zoom};
	}

	void CameraObject::update_ui_buffer()
	{
		base_update_ui_buffer();

		ui::obj_properties.is_active = is_active;
		ui::obj_properties.zoom = zoom;
	}

	void CameraObject::update_from_ui_buffer()
	{
		base_update_from_ui_buffer();

		is_active = ui::obj_properties.is_active;
		zoom = ui::obj_properties.zoom;

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
		if (ImGui::Checkbox("##is_active", &ui::obj_properties.is_active))
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Zoom", ItemLabelFlag::Left);
		ImGui::InputFloat("##zoom", &ui::obj_properties.zoom);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			change_made = true;
		}

		if (change_made)
		{
			update_from_ui_buffer();

			ObjectEvent* event = new ObjectEvent(&copy_camera, this);
			event->object_uuid = this->uuid;
			event::push_event(event);
		}
	}

	void CameraObject::save_to_json(nlohmann::json& data) const
	{
		GameObject::save_to_json(data);

		data["type"] = "camera";
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
