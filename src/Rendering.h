#pragma once

#include <vector>
#include <unordered_map>

#include "raylib.h"

#include "util.h"
#include "GameObject.h"

namespace Rendering {
    struct RenderingLayer {
        std::vector<GameObject*> objects;
        u8 layerNum;
    };

    extern std::unordered_map<std::string, Image> images;
    extern std::unordered_map<std::string, Font> fonts;
    extern std::vector<RenderingLayer> layers;
    extern RenderTexture2D frame;

    Image b_LoadImage(std::string image);
    Font b_LoadFont(std::string font);
    void DrawGameObjects();
    void CreateLayers(int count);
    void AddToLayer(GameObject* obj);
    void RemoveFromLayer(GameObject* obj);
};