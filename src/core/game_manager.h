#pragma once

#include <stdint.h>

#include <vector>

#include "raylib.h"
#include "sol/sol.hpp"

#include "game_object.h"
#include "entity.h"

namespace bacon {
    typedef uid_t uint64_t;

    class GameManager {
        public:
            GameManager();
            GameManager(const GameManager& manager) = delete;
            GameManager& operator=(const GameManager& manager) = delete;
            GameManager(GameManager&& manager) = delete;
            GameManager& operator=(GameManager&& manager) = delete;
            ~GameManager();

            Entity* instantiate_entity();
            Entity* copy_entity(Entity* entity);
            void deinstantiate_entity(Entity* entity);
            void deinstantiate_entity(uid_t uid);

        private:
            uid_t m_uid_count;
            std::vector<uid_t> m_unused_uids;

            std::vector<GameObject*> m_objects;
            std::vector<Entity*> m_entities;

            Camera2D* m_camera;

            sol::state m_lua_state;
    };
}
