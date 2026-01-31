#pragma once

#include <stdint.h>

#include <vector>
#include <string>

#include "raylib.h"

namespace bacon {
    class GameObject
    {
        public:
            friend class GameManager;
            friend class Renderer;

            std::string name;
            std::string tag;
            Vector2 position;
            Vector2 size;
            float rotation;
            bool is_visible;

            uid_t get_uid() const;
            GameObject* get_parent() const;
            const std::vector<GameObject*>& get_children() const;

            virtual void draw() const = 0;
            virtual void draw_properties_editor() = 0;
            virtual void save_to_json() const;
            virtual void load_from_json();
            virtual size_t calculate_size() const;
            virtual uint8_t* serialize() const;
            virtual void deserialize(uint8_t* bytes);

        protected:
            GameObject* parent;
            std::vector<GameObject*> children;
            size_t layer;

            GameObject(uid_t uid);
            GameObject(uint8_t* bytes);
            GameObject(const GameObject& obj) = delete;
            GameObject(GameObject&& obj) = delete;
            GameObject& operator=(const GameObject& obj) = delete;
            GameObject& operator=(GameObject&& obj) = delete;
            virtual ~GameObject() = default;

        private:
            uid_t m_uid;
    };
}
