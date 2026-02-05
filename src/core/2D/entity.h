#pragma once

#include <string>

#include "raylib.h"
#include "box2d/id.h"

#include "game_object.h"

namespace bacon {
    enum BodyType : uint8_t {
        NONE = 0,
        STATIC,
        DYNAMIC,
        KINEMATIC
    };

    class Entity : public GameObject {
        public:
            friend class GameManager;

            void set_texture(const char* path);
            void set_size(float width, float height);

            void draw() const override;
            void draw_properties_editor() override;
            void save_to_json(nlohmann::json& data) const override;
            void load_from_json(nlohmann::json& data) override;
            size_t calculate_size() const override;
            uint8_t* serialize() const override;
            void deserialize(uint8_t* bytes) override;

        protected:
            b2BodyId physics_body;
            BodyType body_type;

            Entity();
            Entity(const Entity& entity) = delete;
            Entity& operator=(const Entity& entity) = delete;
            Entity(Entity&& entity) = delete;
            Entity& operator=(Entity&& entity) = delete;
            ~Entity() = default;

            void create_body(b2WorldId world_id);

        private:
            std::shared_ptr<Texture2D> m_texture;
            std::string m_texture_path;
    };
}
