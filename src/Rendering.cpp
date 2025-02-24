#include "Rendering.h"
#include "GameManager.h"
#include "Entity.h"
#include "TextObject.h"

namespace Rendering {
    std::unordered_map<std::string, Font> fonts;
    std::vector<RenderingLayer> layers;
    RenderTexture2D frame;

    /**
     * @brief Loads a new font
     * 
     * @param path path to the .ttf file
     * @return Font
     */
    Font b_LoadFont(std::string path) {
        // Font hasn't been loaded in already
        if (fonts.find(path) == fonts.end()) {
            // Raylib LoadFont will always return a font,
            // even if the font we want to load doesn't
            // exist. If so, then the font returned will
            // just be the Raylib default font.
            Font newFont = LoadFont(path.c_str());
            fonts[path] = newFont;
            return newFont;
        }
        else {
            return fonts[path];
        }
    }

    /**
     * @brief Draws GameObject, in order of their layer
     * 
     */
    void DrawGameObjects() {
        BeginTextureMode(frame);
            ClearBackground(DARKGRAY);

            BeginMode2D(GameManager::current_camera->camera);
                for (RenderingLayer layer : layers) {
                    for (GameObject* obj : layer.objects) {
                        // Entities
                        if (obj->type == ENTITY) {
                            Entity* e = (Entity*)obj;
                            if (e->isVisible) {
                                DrawTextureEx(e->texture, e->position, e->rotation, 1, WHITE);
                            }
                        }

                        // TextObjects
                        if (obj->type == TEXT) {
                            TextObject* text = (TextObject*)obj;
                            if (text->isVisible) {
                                DrawTextPro(
                                    text->font,
                                    text->text.c_str(),
                                    text->position,
                                    {0,0},
                                    text->rotation,
                                    text->fontSize,
                                    text->charSpacing,
                                    text->color
                                );
                            }
                        }

                        // Camera
                        if (obj->type == CAMERA) {
                            Camera* cam = (Camera*)obj;
                        }

                        // Draw bounding box on top of texture
                        if (obj->showBoundingBox) {
                            DrawRectangleLinesEx(
                                {obj->position.x, obj->position.y,
                                obj->size.x, obj->size.y},
                                2.f,
                                RED
                            );
                        }
                    }
                }
            EndMode2D(); 
        EndTextureMode();
    }

    /**
     * @brief Creates new RenderingLayers
     * 
     * @param count the number of layers to create
     */
    void CreateLayers(int count) {
        size_t size = layers.size();
        for (int i = 0; i < count; i++) {
            RenderingLayer layer;
            layer.layerNum = size + i;
            layers.push_back(layer);
        }
    }

    /**
     * @brief Adds a GameObject to their layer
     * 
     * @param obj the GameObject to add
     */
    void AddToLayer(GameObject* obj) {
        // Create new layers if necessary
        if (obj->layer >= layers.size()) {
            CreateLayers(obj->layer - layers.size() + 1);
        }

        layers[obj->layer].objects.push_back(obj);
    }

    /**
     * @brief Removes a GameObject from their layer
     * 
     * @param obj the GameObject to remove
     */
    void RemoveFromLayer(GameObject* obj) {
        for (size_t i = 0; i < layers[obj->layer].objects.size(); i++) {
            if (layers[obj->layer].objects[i]->ID == obj->ID) {
                layers[obj->layer].objects.erase(layers[obj->layer].objects.begin() + i);
                break;
            }
        }
    }
};
