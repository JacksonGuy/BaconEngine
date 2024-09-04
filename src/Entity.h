#pragma once

#include "GameObject.h"

#include "raylib.h"
#include "json.hpp"

enum EntityVar_t {
    NUMBER,
    STRING
};

enum PhysicsBody_t {
    STATIC,
    DYNAMIC
};

typedef struct {
    std::string name;
    EntityVar_t type;
    std::string stringval;
    f64 numval;
} EntityVar;

class Entity : public GameObject {
    public:
        // Sprite information
        Texture2D texture;
        std::string texturePath;
       
        // Physics
        Rectangle rect;
        PhysicsBody_t bodytype;
        bool solid;
        bool physicsObject;
        bool grounded;
        Vector2 velocity;

        // UI
        bool showHitbox;

        // Lua Scripting
        std::vector<std::string> lua_scripts;
        std::vector<EntityVar> variables;

        // Functions
        Entity();
        ~Entity();
        void SetTexture(std::string path);
        void UpdateRect();
        void SaveEntityJson(nlohmann::json& data);
        void LoadEntityJson(nlohmann::json& data);
        void DrawPropertiesUI();
};
