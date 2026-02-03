#include "camera_object.h"

#include "imgui.h"

#include "core/util.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"

namespace bacon {
    CameraObject::CameraObject(uid_t uid) : GameObject(uid) {
        this->class_type = object_t::CAMERA;
        this->name = "Camera (" + std::to_string(uid) + ")";
        this->camera = {0};
        this->is_active = false;
        this->zoom = 1.0f;
    }

    void CameraObject::move_camera(Vector2 delta) {
        this->position.x += delta.x;
        this->position.y += delta.y;
        this->camera.target = this->position;
    }

    void CameraObject::set_position(Vector2 position) {
        this->position = position;
        this->camera.target = position;
    }

    void CameraObject::calculate_size(Vector2 window_size) {
        this->size = {
            window_size.x * this->camera.zoom,
            window_size.y * this->camera.zoom
        };
    }

    void CameraObject::draw() const {

    }

    void CameraObject::draw_properties_editor() {
        GameObject::draw_properties_editor();
    }

    void CameraObject::save_to_json(nlohmann::json& data) const {
        debug_error("This function has not been implemented yet.");
    }

    void CameraObject::load_from_json(nlohmann::json& data) {
        debug_error("This function has not been implemented yet.");
    }

    size_t CameraObject::calculate_size() const {
        debug_error("This function has not been implemented yet.");
        return 0;
    }

    uint8_t* CameraObject::serialize() const {
        debug_error("This function has not been implemented yet.");
        return nullptr;
    }

    void CameraObject::deserialize(uint8_t* bytes) {
        debug_error("This function has not been implemented yet.");
    }
}
