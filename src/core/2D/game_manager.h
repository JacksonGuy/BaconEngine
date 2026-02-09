#pragma once

#include <stdint.h>

#include <vector>

#include "box2d/box2d.h"
#include "file/resource_manager.h"
#include "raylib.h"
#include "sol/sol.hpp"

#include "core/2D/entity.h"
#include "core/2D/game_object.h"
#include "core/2D/text_object.h"
#include "rendering/2D/renderer.h"

namespace bacon
{
    class GameManager
    {
         public:
        ResourceManager resources;

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
        void load_default_font(const char* path);

        CameraObject* instantiate_camera();
        void deinstantiate_camera(CameraObject* camera);

        const std::vector<GameObject*>& get_objects() const;
        GameObject* find_object_by_uuid(std::string uuid) const;
        GameObject* find_object_by_uuid(UUID uuid) const;

        void set_object_layer(GameObject* object, size_t layer);

        const std::vector<CameraObject*>& get_cameras() const;
        void set_active_camera(CameraObject* camera);
        CameraObject* get_active_camera() const;

        void create_physics_bodies();
        void initialize_renderer(uint32_t width, uint32_t height);
        Renderer* get_renderer();

        void simulation_step();
        void draw_entities(Camera2D* camera = nullptr) const;

        float get_gravity() const;
        void set_gravity(float gravity);

        void reset();
        void create_physics_world();

         private:
        std::vector<GameObject*> m_objects;
        std::vector<Entity*> m_entities;
        std::vector<CameraObject*> m_cameras;

        Renderer* m_renderer = nullptr;
        CameraObject* m_camera;
        std::string m_default_font_path;
        std::shared_ptr<Font> m_default_font;

        b2WorldId m_world;
        float m_length_units_per_meter;
        float m_gravity;

        std::unique_ptr<sol::state> m_lua_state;
    };
} // namespace bacon
