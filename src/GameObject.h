#pragma once

#include <string>
#include <vector>

#include "raylib.h"

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

        // UI Info
        bool showDetails;

        // Functions
        GameObject();
        ~GameObject();
};
