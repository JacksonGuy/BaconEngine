#include "entity.h"

namespace bacon {
    Entity::Entity(uid_t uid) : GameObject(uid) {
        this->m_texture = {0};
        this->m_texture_path = "";
    }

    void Entity::save_to_json() const {

    }

    void Entity::load_from_json() {

    }

    size_t Entity::calculate_size() const {
        return 0;
    }

    uint8_t* Entity::serialize() const {
        return nullptr;
    }

    void Entity::deserialize(uint8_t* bytes) {

    }
}
