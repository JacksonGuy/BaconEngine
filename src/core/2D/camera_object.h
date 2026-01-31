#pragma once

#include "core/2D/game_object.h"

namespace bacon {
    class CameraObject : public GameObject {
        public:
            friend class GameManager;

            Camera2D camera;
            bool is_active;
            float zoom;

            void move_camera(Vector2 delta);
            void set_position(Vector2 position);
            void calculate_size(Vector2 window_size);

            void draw() const override;
            void save_to_json() const override;
            void load_from_json() override;
            size_t calculate_size() const override;
            uint8_t* serialize() const override;
            void deserialize(uint8_t* bytes) override;

        protected:
            CameraObject(uid_t uid);
            CameraObject(const CameraObject& camera) = delete;
            CameraObject& operator=(const CameraObject& camera) = delete;
            CameraObject(CameraObject&& camera) = delete;
            CameraObject& operator=(CameraObject&& camera) = delete;
            ~CameraObject() = default;
    };
}
