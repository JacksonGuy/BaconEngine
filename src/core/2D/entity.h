#pragma once

#include <string>

#include "raylib.h"
#include "box2d/id.h"

#include "game_object.h"

namespace bacon {
    enum body_t {
        NONE,
        STATIC,
        DYNAMIC,
        KINEMATIC
    };

    class Entity : public GameObject {
        public:
            friend class GameManager;

            void set_texture(const char* path);

            void draw() const override;
            void save_to_json() const override;
            void load_from_json() override;
            size_t calculate_size() const override;
            uint8_t* serialize() const override;
            void deserialize(uint8_t* bytes) override;

        protected:
            b2BodyId physics_body;
            body_t body_type;

            Entity(uid_t uid);
            Entity(const Entity& entity) = delete;
            Entity& operator=(const Entity& entity) = delete;
            Entity(Entity&& entity) = delete;
            Entity& operator=(Entity&& entity) = delete;
            ~Entity() = default;

            void create_body(b2WorldId world_id);

        private:
            Texture2D m_texture;
            std::string m_texture_path;
    };
}
