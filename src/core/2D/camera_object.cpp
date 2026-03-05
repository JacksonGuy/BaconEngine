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
#include "raymath.h"

namespace bacon
{
	PoolAllocator<CameraObject> CameraObject::_allocator(globals::allocator_block_size);

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
		this->camera = {};
		this->is_active = false;
		this->zoom = 1.0f;

		// Camera specific overrides
		this->size = {0, 0};
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

	CameraObject::~CameraObject()
	{
		if (in_scene)
		{
			remove_from_scene();
		}

		delete_children();
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
		if (in_scene) return;

		GameState::scene.add_camera(this);
		GameState::renderer->add_to_layer(this, layer);
		in_scene = true;
	}

	void CameraObject::remove_from_scene()
	{
		if (!in_scene) return;

		GameState::scene.remove_camera(this);
		GameState::renderer->remove_from_layer(this);
		in_scene = false;
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

	void CameraObject::draw_outline() const
	{
		// Get size from zoom
		Vector2 frame_size = {
			ui::window_size.x / this->camera.zoom,
			ui::window_size.y / this->camera.zoom,
		};

		DrawRectangleLinesPro(
			{
				position.x + (frame_size.x / 2.f),
				position.y + (frame_size.y / 2.f),
				frame_size.x,
				frame_size.y,
			},
			this->rotation,
			3.f,
			Color{0, 255, 0, 255});
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

		// Update zoom from zoom field
		this->zoom = ui::obj_properties.zoom;

		// Set active
		is_active = ui::obj_properties.is_active;
		if (this->is_active)
		{
			GameState::scene.set_active_camera(this);
		}

		// Adjust camera
		this->camera.target = this->position;
		this->camera.rotation = this->rotation;
		this->camera.zoom = this->zoom;
	}

	void CameraObject::draw() const {}

	void CameraObject::draw_properties_editor()
	{
		// WARNING!!
		using namespace event;

		if (ui::inspect_object_copy == nullptr ||
			ui::inspect_object_copy->uuid != this->uuid)
		{
			delete ui::inspect_object_copy;
			ui::inspect_object_copy = this->clone();
		}

		// We don't use draw_base_properties() here
		// since certain fields do not apply to camera.

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

		// Position
		ImGui::ItemLabel("Position", ItemLabelFlag::Left);
		ImGui::InputFloat2("##position", ui::obj_properties.position);
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

		ImGui::Separator();

		// ----------------------
		// Camera Fields
		// ----------------------

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

			ObjectEvent* event = new ObjectEvent(ui::inspect_object_copy, this);
			event::push_event(event);

			ui::inspect_object_copy->copy(*this);
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
