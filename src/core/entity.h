#pragma once

#include <string>

#include "raylib.h"

#include "game_object.h"

namespace bacon {
    class Entity : GameObject {
        public:
            void set_texture(const char* path);
            void draw();

            void save_to_json() const override;
            void load_from_json() override;
            size_t calculate_size() const override;
            uint8_t* serialize() const override;
            void deserialize(uint8_t* bytes) override;

        private:
            Texture2D m_texture;
            std::string m_texture_path;

            Entity(uid_t uid);
            Entity(const Entity& entity) = delete;
            Entity& operator=(const Entity& entity) = delete;
            Entity(Entity&& entity) = delete;
            Entity& operator=(Entity&& entity) = delete;
            ~Entity() = default;
    };
}
