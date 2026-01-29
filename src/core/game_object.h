#pragma once

#include <stdint.h>

#include <vector>
#include <string>

#include "raylib.h"

namespace bacon {
    class GameObject
    {
        public:
            std::string name;
            std::string tag;
            Vector2 position;
            Vector2 size;
            float rotation;
            bool is_visible;

            uid_t get_uid();

            virtual void save_to_json() const;
            virtual void load_from_json();
            virtual size_t calculate_size() const;
            virtual uint8_t* serialize() const;
            virtual void deserialize(uint8_t* bytes);

        protected:
            GameObject(uid_t uid);
            GameObject(uint8_t* bytes);
            GameObject(const GameObject& obj) = delete;
            GameObject(GameObject&& obj) = delete;
            GameObject& operator=(const GameObject& obj) = delete;
            GameObject& operator=(GameObject&& obj) = delete;
            virtual ~GameObject() = default;

            GameObject* parent;
            std::vector<GameObject*> children;

        private:
            uid_t m_uid;
            uint8_t m_layer;
    };
}
