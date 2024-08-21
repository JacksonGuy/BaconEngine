#include "Rendering.h"
#include "Entity.h"

namespace Rendering {
    std::unordered_map<std::string, Font*> fonts;
    std::vector<RenderingLayer> layers;
    RenderTexture2D frame;

    Font* LoadFont(std::string path) {
        // Font hasn't been loaded in already
        if (fonts.find(path) == fonts.end()) {
            Font* font = new Font();
            font = LoadFont(path);
            return font;
        }
        else {
            return fonts[path];
        }
    }

    void DrawGameObjects() {
        BeginTextureMode(frame);
            ClearBackground(DARKGRAY);

            for (RenderingLayer layer : layers) {
                for (GameObject* obj : layer.objects) {
                    if (obj->type == ENTITY) {
                        Entity* e = (Entity*)obj;
                        if (e->isVisible) {
                            DrawTexture(e->texture, e->position.x, e->position.y, WHITE);
                        }
                    }
                }
            }  
        EndTextureMode();
    }

    void CreateLayers(int count) {
        size_t size = layers.size();
        for (int i = 0; i < count; i++) {
            RenderingLayer layer;
            layer.layerNum = size + i;
            layers.push_back(layer);
        }
    }

    void AddToLayer(GameObject* obj) {
        // Create new layers if necessary
        if (obj->layer >= layers.size()) {
            CreateLayers(obj->layer - layers.size() + 1);
        }

        layers[obj->layer].objects.push_back(obj);
    }

    void RemoveFromLayer(GameObject* obj) {
        for (size_t i = 0; i < layers[obj->layer].objects.size(); i++) {
            if (layers[obj->layer].objects[i]->ID == obj->ID) {
                layers[obj->layer].objects.erase(layers[obj->layer].objects.begin() + i);
                break;
            }
        }
    }
};
