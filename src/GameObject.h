#pragma once

#include <string>
#include <vector>

#include "raylib.h"
#include "json.hpp"

#include "util.h"

enum ClassType {
    OBJECT,
    ENTITY,
    TEXT,
    CAMERA
};

class GameObject {
    public:
        // Meta information
        static u32 IDCount;
        static std::vector<u32> unusedIDs;
        u32 ID;
        std::string name;
        std::string tag;
        ClassType type;

        // Physical information
        Vector2 position;
        Vector2 size;
        float rotation;
        bool isVisible;
        u8 layer;

        // Tree data
        GameObject* parent;
        std::vector<GameObject*> children;
        Vector2 previousPosition;

        // UI info
        bool showBoundingBox; 

        // Functions
        GameObject();
        GameObject(const GameObject* obj);
        ~GameObject();
        void SaveGameObjectJson(nlohmann::json& data);
        void LoadGameObjectJson(nlohmann::json& data);
        void SaveGameObjectPrefab(nlohmann::json& data);
        void LoadGameObjectPrefab(nlohmann::json& data);
};
