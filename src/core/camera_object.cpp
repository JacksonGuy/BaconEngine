#include "camera_object.h"

#include "util.h"

namespace bacon {
    CameraObject::CameraObject(uid_t uid) : GameObject(uid) {
        this->m_camera = {0};
        this->m_is_active = false;
        this->m_zoom = 1.0f;
    }

    void CameraObject::move_camera(Vector2 delta) {
        this->position.x += delta.x;
        this->position.y += delta.y;
        this->m_camera.target = this->position;
    }

    void CameraObject::set_position(Vector2 position) {
        this->position = position;
        this->m_camera.target = position;
    }

    void CameraObject::calculate_size(Vector2 window_size) {
        this->size = {
            window_size.x * this->m_camera.zoom,
            window_size.y * this->m_camera.zoom
        };
    }

    void CameraObject::save_to_json() const {
        debug_error("This function has not been implemented yet.");
    }

    void CameraObject::load_from_json() {
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
