#include "game_object.h"

#include "core/util.h"

namespace bacon {
    GameObject::GameObject(uid_t uid) {
        this->name = "Object (" + std::to_string(uid) + ")";
        this->tag = "Default";
        this->position = {0.f, 0.f};
        this->size = {1.f, 1.f};
        this->rotation = 0.f;
        this->is_visible = true;
        this->layer = 0;

        this->m_uid = uid;
        this->parent = nullptr;
    }

    GameObject::GameObject(uint8_t* bytes) {
        this->deserialize(bytes);
    }

    uid_t GameObject::get_uid() const {
        return this->m_uid;
    }

    GameObject* GameObject::get_parent() const {
        return this->parent;
    }

    const std::vector<GameObject*>& GameObject::get_children() const {
        return this->children;
    }

    void GameObject::save_to_json() const {
        debug_error("default save_to_json has not been implemented yet.");
    }

    void GameObject::load_from_json() {
        debug_error("default load_from_json has not been implemented yet.");
    }

    size_t GameObject::calculate_size() const {
        debug_error("default calculate_size has not been implemented yet.");
        return 0;
    }

    uint8_t* GameObject::serialize() const {
        debug_error("default serialize has not been implemented yet.");
        return nullptr;
    }

    void GameObject::deserialize(uint8_t* bytes) {
        debug_error("default deserialize has not been implemented yet.");
    }
}
