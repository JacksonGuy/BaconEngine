#pragma once

#include <stdint.h>

#include <vector>

#include "raylib.h"
#include "sol/sol.hpp"
#include "box2d/box2d.h"

#include "core/2D/game_object.h"
#include "core/2D/entity.h"
#include "core/2D/text_object.h"
#include "rendering/2D/renderer.h"

namespace bacon {
    class GameManager {
        public:
            GameManager();
            GameManager(const GameManager& manager) = delete;
            GameManager& operator=(const GameManager& manager) = delete;
            GameManager(GameManager&& manager) = delete;
            GameManager& operator=(GameManager&& manager) = delete;
            ~GameManager();

            Entity* instantiate_entity(BodyType type);
            Entity* copy_entity(Entity* entity);
            void entity_create_body(Entity* entity);
            void deinstantiate_entity(Entity* entity);

            TextObject* instantiate_text();
            void deinstantiate_text(TextObject* text);

            CameraObject* instantiate_camera();
            void deinstantiate_camera(CameraObject* camera);

            const std::vector<GameObject*>& get_objects() const;
            const GameObject* find_object_by_uuid(std::string uuid) const;
            const GameObject* find_object_by_uuid(UUID uuid) const;

            void set_object_layer(GameObject* object, size_t layer);
            void set_active_camera(CameraObject* camera);

            void create_physics_bodies();
            void initialize_renderer(uint32_t width, uint32_t height);
            Renderer* get_renderer();

            void simulation_step();
            void draw_entities(Camera2D* camera = nullptr) const;
            float get_gravity() const;
            void set_gravity(float gravity);

            void reset();

        private:
            std::vector<GameObject*> m_objects;
            std::vector<Entity*> m_entities;

            Renderer* m_renderer = nullptr;
            CameraObject* m_camera;

            b2WorldId m_world;
            float m_length_units_per_meter;
            float m_gravity;

            std::unique_ptr<sol::state> m_lua_state;
    };
}
