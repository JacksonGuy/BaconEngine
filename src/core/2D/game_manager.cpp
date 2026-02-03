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
        this->m_uid_count = 0;
        this->m_camera = nullptr;
        this->m_renderer = nullptr;

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
        debug_warn("Destructor for GameManager has not been implemented yet.");
    }

    Entity* GameManager::instantiate_entity(body_t type) {
        uid_t uid = this->acquire_uid();
        Entity* entity = new Entity(uid);

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

        // Reclaim uid
        this->m_unused_uids.push_back(entity->get_uid());

        // Delete
        delete(entity);
    }

    CameraObject* GameManager::instantiate_camera() {
        uid_t uid = this->acquire_uid();
        CameraObject* camera = new CameraObject(uid);

        this->m_objects.push_back(camera);

        this->m_renderer->add_to_layer(camera, camera->layer);

        return camera;
    }

    void GameManager::deinstantiate_camera(CameraObject* camera) {
        debug_error("This function has not been implemented yet.");
    }

    const std::vector<GameObject*>& GameManager::get_objects() const {
        return this->m_objects;
    }

    void GameManager::set_object_layer(GameObject* object, size_t layer) {
        this->m_renderer->remove_from_layer(object);
        this->m_renderer->add_to_layer(object, layer);
    }

    void GameManager::set_active_camera(CameraObject* camera) {
        this->m_camera = camera;
    }

    void GameManager::create_physics_bodies() {
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
        debug_error("This function has not been implemented yet.");
    }

    void GameManager::reset() {
        // Clear UID count
        this->m_uid_count = 0;
        this->m_unused_uids.clear();

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
        b2DestroyWorld(m_world);

        // Reset Lua
        m_lua_state.reset();
    }

    uid_t GameManager::acquire_uid() {
        if (!this->m_unused_uids.empty()) {
            uid_t uid = this->m_unused_uids.back();
            this->m_unused_uids.pop_back();
            return uid;
        }

        return this->m_uid_count++;
    }
}
