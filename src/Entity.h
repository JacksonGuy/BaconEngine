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
        std::string texturePath; // This is just for UI purposes
       
        // Physics
        Rectangle rect;
        PhysicsBody_t bodytype;

        bool grounded;
        Vector2 velocity;

        // Lua Scripting
        std::vector<std::string> lua_scripts;
        std::vector<EntityVar> variables;

        // Game meta information
        bool isPlayer;

        // Functions
        Entity();
        ~Entity();
        void SetTexture(std::string path);
        void UpdateRect();
        void SaveEntityJson(nlohmann::json& data);
        void LoadFromJson(nlohmann::json& data);
};
