#pragma once

#include "GameObject.h"

#include "raylib.h"
#include "json.hpp"
#include <sol/sol.hpp>

enum EntityVar_t {
    NUMBER,
    STRING,
    BOOLEAN
};

enum PhysicsBody_t {
    STATIC,
    DYNAMIC
};

typedef struct {
    EntityVar_t type;
    std::string stringval;
    f64 numval;
    bool boolval;
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

        // Lua Scripting
        std::vector<std::string> lua_scripts;
        std::map<std::string, EntityVar> variables;

        // Functions
        Entity();
        Entity(const Entity* entity);
        ~Entity();
        void SetTexture(std::string path);
        void UpdateRect();
        
        void SetVariable(std::string name, f64 value);
        void SetVariable(std::string name, std::string value);
        void SetVariable(std::string name, bool value);
        sol::lua_value GetVariable(std::string name);

        void SaveEntityJson(nlohmann::json& data);
        void LoadEntityJson(nlohmann::json& data);
        void SaveEntityPrefab(nlohmann::json& data);
        void LoadEntityPrefab(nlohmann::json& data);
        void DrawPropertiesUI();
};
