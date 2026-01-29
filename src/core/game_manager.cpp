#include "game_manager.h"

#include "box2d/box2d.h"
#include "box2d/math_functions.h"
#include "box2d/types.h"
#include "raylib.h"

#include "core/util.h"

namespace bacon {
    GameManager::GameManager() {
        this->m_uid_count = 0;
        this->m_camera = nullptr;

        this->m_length_units_per_meter = 128.0f;
        this->m_gravity = 9.8f * this->m_length_units_per_meter;
        b2WorldDef world_def = b2DefaultWorldDef();
        world_def.gravity = (b2Vec2){0.0f, this->m_gravity};
        this->m_world = b2CreateWorld(&world_def);
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

        this->m_objects.push_back(entity);
        this->m_entities.push_back(entity);

        return entity;
    }

    void GameManager::create_physics_bodies() {
        for (Entity* entity : this->m_entities) {
            entity->create_body(this->m_world);
        }
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

    void GameManager::draw_entities() const {
        for (const Entity* entity : this->m_entities) {
            entity->draw(this->m_world);
        }
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
