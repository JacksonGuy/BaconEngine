#include "camera_object.h"

#include "imgui.h"

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

	CameraObject::CameraObject() : GameObject()
	{
		this->object_type = ObjectType::CAMERA;
		this->name = "Camera";
		this->camera = {0};
		this->is_active = false;
		this->zoom = 1.0f;
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

	void CameraObject::draw() const {}

	void CameraObject::draw_properties_editor()
	{
		GameObject::draw_properties_editor();

		bool change_made = false;

		// Position
		float position[] = {this->position.x, this->position.y};
		ImGui::ItemLabel("Position", ItemLabelFlag::Left);
		if (ImGui::InputFloat2("##position", position))
		{
			this->position = (Vector2){position[0], position[1]};
			this->camera.target = this->position;

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		// Size
		float size[] = {this->size.x, this->size.y};
		ImGui::ItemLabel("Size", ItemLabelFlag::Left);
		if (ImGui::InputFloat2("##size", size))
		{
			this->size = (Vector2){size[0], size[1]};

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

		ImGui::Separator();

		ImGui::ItemLabel("Active", ItemLabelFlag::Left);
		if (ImGui::Checkbox("##is_active", &this->is_active))
		{
			if (this->is_active)
			{
				GameState::scene.set_active_camera(this);
			}

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		ImGui::ItemLabel("Zoom", ItemLabelFlag::Left);
		if (ImGui::InputFloat("##zoom", &this->zoom))
		{
			camera.zoom = this->zoom;

			globals::has_unsaved_changes = true;
			change_made = true;
		}

		if (change_made)
		{
			event::push_event(event::EventType::OBJECT_EDIT, this);
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
