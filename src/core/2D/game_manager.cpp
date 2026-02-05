#include "core/2D/game_manager.h"

#include <memory>

#include "box2d/box2d.h"
#include "box2d/id.h"
#include "box2d/math_functions.h"
#include "box2d/types.h"
#include "raylib.h"

#include "core/util.h"

namespace bacon {
    GameManager::GameManager() {
        this->m_camera = nullptr;
        this->m_renderer = nullptr;
        this->m_default_font = {0};

        this->m_length_units_per_meter = 128.0f;
        this->m_gravity = 9.8f * this->m_length_units_per_meter;

        b2WorldDef world_def = b2DefaultWorldDef();

        world_def.gravity = (b2Vec2){0.0f, this->m_gravity};
        world_def.enableSleep = false;

        this->m_world = b2CreateWorld(&world_def);

        this->m_lua_state = std::make_unique<sol::state>();
    }

    /*
     * Does this even need to exist?
     * GameManager has the same lifetime as the entire game, so
     * what's the point in having a cleanup procedure?
     * Other than saving the game state, which should
     * be optional anyways...
     */
    GameManager::~GameManager() {
        delete(m_renderer);
        for (GameObject* object : m_objects) {
            delete(object);
        }
        b2DestroyWorld(m_world);
    }

    Entity* GameManager::instantiate_entity(BodyType type) {
        Entity* entity = new Entity();

        entity->body_type = type;
        entity->manager = this;

        this->m_objects.push_back(entity);
        this->m_entities.push_back(entity);

        this->m_renderer->add_to_layer(entity, entity->layer);

        return entity;
    }

    void GameManager::entity_create_body(Entity* entity) {
        entity->create_body(this->m_world);
    }

    void GameManager::deinstantiate_entity(Entity* entity) {
        // Erase from entities list
        bool found_entity = false;
        for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
            if (*it == entity) {
                m_entities.erase(it);
                found_entity = true;
                break;
            }
        }
        if (!found_entity) {
            debug_error("Failed to remove entity from entities list.");
        }

        // Erase from objects list
        bool found_object = false;
        for (auto it = m_objects.begin(); it != m_objects.end(); it++) {
            if (*it == entity) {
                m_objects.erase(it);
                found_object = true;
                break;
            }
        }
        if (!found_object) {
            debug_error("Failed to remove entity from objects list.");
        }

        // Destroy body
        b2DestroyBody(entity->physics_body);
        entity->physics_body = b2_nullBodyId;

        // Remove from layer
        this->m_renderer->remove_from_layer(entity);

        // Delete
        delete(entity);
    }

    TextObject* GameManager::instantiate_text() {
        TextObject* text = new TextObject();
        text->manager = this;

        text->set_font(this->m_default_font_path);

        this->m_objects.push_back(text);
        this->m_renderer->add_to_layer(text, text->layer);

        return text;
    }

    void GameManager::deinstantiate_text(TextObject* text) {
        // Erase from objects list
        bool found_object = false;
        for (auto it = m_objects.begin(); it != m_objects.end(); it++) {
            if (*it == text) {
                m_objects.erase(it);
                found_object = true;
                break;
            }
        }
        if (!found_object) {
            debug_error("Failed to remove entity from objects list.");
        }

        // Remove from layer
        this->m_renderer->remove_from_layer(text);

        delete(text);
    }

    void GameManager::load_default_font(const char* path) {
        this->m_default_font = this->resources.load_font(path);

        // Check if font loaded correctly
        if (this->m_default_font->baseSize <= 0) {
            debug_error("Failed to load default font!");
        }
        else {
            this->m_default_font_path = path;
            debug_log("Set default font path: %s", m_default_font_path.c_str());
        }
    }

    CameraObject* GameManager::instantiate_camera() {
        CameraObject* camera = new CameraObject();

        camera->manager = this;

        this->m_objects.push_back(camera);
        this->m_cameras.push_back(camera);

        return camera;
    }

    void GameManager::deinstantiate_camera(CameraObject* camera) {
        // Erase from objects list
        bool found_object = false;
        for (auto it = m_objects.begin(); it != m_objects.end(); it++) {
            if (*it == camera) {
                m_objects.erase(it);
                found_object = true;
                break;
            }
        }
        if (!found_object) {
            debug_error("Failed to remove camera from objects list.");
        }

        // Erase from cameras list
        bool found_camera = false;
        for (auto it = m_cameras.begin(); it != m_cameras.end(); it++) {
            if (*it == camera) {
                m_cameras.erase(it);
                found_camera = true;
                break;
            }
        }
        if (!found_camera) {
            debug_error("Failed to remove camera from cameras list.");
        }

        delete(camera);
    }

    const std::vector<GameObject*>& GameManager::get_objects() const {
        return this->m_objects;
    }

    GameObject* GameManager::find_object_by_uuid(std::string uuid) const {
        GameObject* object = nullptr;

        for (GameObject* obj : this->m_objects)
        {
            if (obj->uuid.get_uuid() == uuid)
            {
                object = obj;
                break;
            }
        }

        return object;
    }

    GameObject* GameManager::find_object_by_uuid(UUID uuid) const {
        GameObject* object = nullptr;

        for (GameObject* obj : this->m_objects)
        {
            if (obj->uuid == uuid)
            {
                object = obj;
                break;
            }
        }

        return object;
    }

    void GameManager::set_object_layer(GameObject* object, size_t layer) {
        this->m_renderer->remove_from_layer(object);
        this->m_renderer->add_to_layer(object, layer);
    }

    const std::vector<CameraObject*>& GameManager::get_cameras() const {
        return this->m_cameras;
    }

    void GameManager::set_active_camera(CameraObject* camera) {
        if (this->m_camera != nullptr && this->m_camera != camera)
        {
            this->m_camera->is_active = false;
        }

        this->m_camera = camera;
    }

    CameraObject* GameManager::get_active_camera() const {
        return this->m_camera;
    }

    void GameManager::create_physics_bodies() {
        debug_log("Creating physics bodies");
        for (Entity* entity : this->m_entities) {
            entity->create_body(this->m_world);
        }
    }

    void GameManager::initialize_renderer(uint32_t width, uint32_t height) {
        this->m_renderer = new Renderer(width, height);
    }

    Renderer* GameManager::get_renderer() {
        return this->m_renderer;
    }

    void GameManager::simulation_step() {
        float delta_time = GetFrameTime();
        b2World_Step(this->m_world, delta_time, 4);

        // Update entity variables
        for (Entity* entity : this->m_entities) {
            b2Vec2 pos = b2Body_GetPosition(entity->physics_body);
            b2Rot rotation = b2Body_GetRotation(entity->physics_body);
            float radians = b2Rot_GetAngle(rotation);

            entity->position = {pos.x, pos.y};
            entity->rotation = radians * RAD2DEG;
        }
    }

    void GameManager::draw_entities(Camera2D* camera) const {
        if (camera != nullptr) {
            this->m_renderer->draw(camera);
            return;
        }

        if (this->m_camera == nullptr) {
            debug_error("Scene does not have a camera.");
            return;
        }

        this->m_renderer->draw(&this->m_camera->camera);
    }

    float GameManager::get_gravity() const {
        return this->m_gravity / this->m_length_units_per_meter;
    }

    void GameManager::set_gravity(float gravity) {
        this->m_gravity = gravity * this->m_length_units_per_meter;
    }

    void GameManager::reset() {
        // Clear renderer
        this->m_renderer->reset();

        // Clear objects
        for (GameObject* object : this->m_objects) {
            delete(object);
        }
        this->m_objects.clear();
        this->m_entities.clear();

        this->m_camera = nullptr;

        // Delete physics world
        this->create_physics_world();

        // Reset Lua
        m_lua_state.reset();
    }

    void GameManager::create_physics_world() {
        if (b2World_IsValid(m_world)) {
            b2DestroyWorld(m_world);
        }

        b2WorldDef world_def = b2DefaultWorldDef();

        world_def.gravity = (b2Vec2){0.0f, this->m_gravity};
        world_def.enableSleep = false;

        this->m_world = b2CreateWorld(&world_def);
    }
}
