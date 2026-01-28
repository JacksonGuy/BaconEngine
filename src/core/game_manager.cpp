#include "game_manager.h"

#include "core/util.h"

namespace bacon {
    GameManager::GameManager() {
        this->m_uid_count = 0;
    }

    GameManager::~GameManager() {
        debug_warn("Destructor for GameManager has not been implemented yet.");
    }
}
