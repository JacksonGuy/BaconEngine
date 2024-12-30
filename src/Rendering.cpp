#include "Rendering.h"
#include "GameManager.h"
#include "Entity.h"
#include "TextObject.h"

namespace Rendering {
    std::unordered_map<std::string, Image> images;
    std::unordered_map<std::string, Font> fonts;
    std::vector<RenderingLayer> layers;
    RenderTexture2D frame;

    /**
     * @brief Loads a new Image from an image file
     * 
     * @param image the path to the image file
     * @return Image 
     */
    Image b_LoadImage(std::string image) {
        // Image hasn't already been loaded yet
        if (images.find(image) == images.end()) {
            Image img = LoadImage(image.c_str());
            images[image] = img;
            return img;
        }
        else {
            return images[image];
        }
    }

    /**
     * @brief Loads a new font
     * 
     * @param path path to the .ttf file
     * @return Font* 
     */
    Font b_LoadFont(std::string path) {
        // Font hasn't been loaded in already
        if (fonts.find(path) == fonts.end()) {
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
                                DrawTextEx(
                                    text->font,
                                    text->text.c_str(),
                                    text->position,
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
