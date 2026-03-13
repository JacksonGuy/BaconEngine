#include "camera_object.h"

#include "imgui.h"
#include "imgui_stdlib.h"
#include "raymath.h"

#include "core/game_state.h"
#include "core/globals.h"
#include "core/util.h"
#include "editor/editor_event.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"
#include "lib/pool_allocator.h"

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

	CameraObject::CameraObject() : Object2D()
	{
		m_type_id = TypeID::CAMERA_2D;
		set_name("Camera");

		this->camera = {};
		this->is_active = false;
		this->zoom = 1.0f;

		// Camera specific overrides
		set_size({0, 0});
	}

	CameraObject::CameraObject(ByteStream& bytes) : Object2D()
	{
		m_type_id = TypeID::CAMERA_2D;
		deserialize(bytes);
	}

	CameraObject::CameraObject(const CameraObject& camera) : Object2D()
	{
		m_type_id = TypeID::CAMERA_2D;
		copy(camera);
	}

	void CameraObject::copy(const GameObject& object)
	{
		Object2D::copy(object);

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
		new_camera->set_uuid(UUID());
		return new_camera;
	}

	void CameraObject::add_to_scene()
	{
		if (get_in_scene()) return;

		GameState::state_2d->scene->add_camera(this);
		GameState::state_2d->renderer->add_to_layer(this, get_layer());
		set_in_scene(true);

		add_children_to_scene();
	}

	void CameraObject::remove_from_scene()
	{
		if (!get_in_scene()) return;

		GameState::state_2d->scene->remove_camera(this);
		GameState::state_2d->renderer->remove_from_layer(this);
		set_in_scene(false);

		remove_children_from_scene();
	}

	void CameraObject::move_camera(Vector2 delta)
	{
		Vector2 new_pos = Vector2Add(get_position(), delta);
		set_position(new_pos);
		this->camera.target = new_pos;
	}

	void CameraObject::set_camera_position(Vector2 position)
	{
		set_position(position);
		this->camera.target = position;
	}

	void CameraObject::draw_outline() const
	{
		// Don't draw outline while playing game
		if (globals::editor_ref->is_playing) return;

		// Get size from zoom
		Vector2 frame_size = {
			ui::window_size.x / this->camera.zoom,
			ui::window_size.y / this->camera.zoom,
		};

		DrawRectangleLinesPro(
			{
				get_position().x + (frame_size.x / 2.f),
				get_position().y + (frame_size.y / 2.f),
				frame_size.x,
				frame_size.y,
			},
			get_rotation(),
			3.f,
			Color{0, 255, 0, 255});
	}

	void CameraObject::update_ui_buffer() const
	{
		Object2D::update_ui_buffer();

		ui::obj_properties.is_active = is_active;
		ui::obj_properties.zoom = zoom;
	}

	void CameraObject::update_from_ui_buffer()
	{
		Object2D::update_from_ui_buffer();

		// Update zoom from zoom field
		this->zoom = ui::obj_properties.zoom;

		// Set active
		is_active = ui::obj_properties.is_active;
		if (this->is_active)
		{
			GameState::state_2d->scene->set_active_camera(this);
		}

		// Adjust camera
		this->camera.target = get_position();
		this->camera.rotation = get_rotation();
		this->camera.zoom = this->zoom;
	}

	void CameraObject::draw() const {}

	void CameraObject::draw_properties_editor()
	{
		// WARNING!!
		using namespace event;

		if (ui::inspect_object_copy == nullptr ||
			ui::inspect_object_copy->get_uuid() != get_uuid())
		{
			delete ui::inspect_object_copy;
			ui::inspect_object_copy = this->clone();
		}

		// We don't use draw_base_properties() here
		// since certain fields do not apply to camera.

		// Name
		ImGui::ItemLabel("Name", ItemLabelFlag::Left);
		ImGui::InputText("##name", &ui::obj_properties.name);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}
		// ImGui::SameLine();
		ImGui::HelpMarker("ID: " + get_uuid().as_string());

		// Tag
		ImGui::ItemLabel("Tag", ItemLabelFlag::Left);
		ImGui::InputText("##tag", &ui::obj_properties.tag);
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

		// Rotation
		ImGui::ItemLabel("Rotation", ItemLabelFlag::Left);
		ImGui::InputFloat("##rotation", &ui::obj_properties.rotation);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			ui::obj_properties.rotation = b_fmod(ui::obj_properties.rotation, 360);

			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}

		ImGui::Separator();

		// ----------------------
		// Camera Fields
		// ----------------------

		ImGui::ItemLabel("Active", ItemLabelFlag::Left);
		if (ImGui::Checkbox("##is_active", &ui::obj_properties.is_active))
		{
			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}

		ImGui::ItemLabel("Zoom", ItemLabelFlag::Left);
		ImGui::InputFloat("##zoom", &ui::obj_properties.zoom);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			globals::has_unsaved_changes = true;
			ui::properties_changes_made = true;
		}

		if (ui::properties_changes_made)
		{
			update_from_ui_buffer();

			ObjectEvent* event = new ObjectEvent(ui::inspect_object_copy, this);
			event::push_event(event);

			ui::inspect_object_copy->copy(*this);
		}
	}

	void CameraObject::save_to_json(nlohmann::json& data) const
	{
		Object2D::save_to_json(data);

		data["type"] = "camera";
		data["is_active"] = this->is_active;
		data["zoom"] = this->zoom;
	}

	void CameraObject::load_from_json(const nlohmann::json& data)
	{
		Object2D::load_from_json(data);

		this->is_active = json_read_bool(data, "is_active");
		this->zoom = json_read_float(data, "zoom");

		if (this->is_active)
		{
			GameState::state_2d->scene->set_active_camera(this);
		}

		camera.target = get_position();
		camera.rotation = get_rotation();
		camera.zoom = this->zoom;
	}

	ByteStream CameraObject::serialize() const
	{
		ByteStream bytes = Object2D::serialize();

		bytes << this->is_active;
		bytes << this->zoom;

		return bytes;
	}

	void CameraObject::deserialize(ByteStream& bytes)
	{
		Object2D::deserialize(bytes);

		bytes >> this->is_active;
		bytes >> this->zoom;

		camera.zoom = this->zoom;
	}
} // namespace bacon
