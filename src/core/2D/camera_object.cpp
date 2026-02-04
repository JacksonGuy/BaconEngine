#include "camera_object.h"

#include "imgui.h"

#include "core/util.h"
#include "editor/ui/editor_ui.h"
#include "editor/ui/imgui_extras.h"

namespace bacon {
    CameraObject::CameraObject() : GameObject() {
        this->class_type = ObjectType::CAMERA;
        this->name = "Camera";
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
        GameObject::save_to_json(data);

        data["is_active"] = this->is_active;
        data["zoom"] = this->zoom;
    }

    void CameraObject::load_from_json(nlohmann::json& data) {
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
            this->manager->set_active_camera(this);
        }
        this->camera.target = this->position;
        this->camera.rotation = this->rotation;
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
