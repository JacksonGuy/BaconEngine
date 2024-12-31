#pragma once

#include <vector>
#include <unordered_map>

#include "raylib.h"

#include "util.h"
#include "GameObject.h"

struct RenderingLayer {
    std::vector<GameObject*> objects;
    u8 layerNum;
};

namespace Rendering {
    extern std::unordered_map<std::string, Font> fonts;
    extern std::vector<RenderingLayer> layers;
    extern RenderTexture2D frame;

    Font b_LoadFont(std::string path);
    void DrawGameObjects();
    void CreateLayers(int count);
    void AddToLayer(GameObject* obj);
    void RemoveFromLayer(GameObject* obj);
};
