#include "rendering/2D/renderer.h"

#include <vector>

#include "raylib.h"

#include "core/util.h"

namespace bacon {
    Renderer::Renderer(uint32_t width, uint32_t height) {
        this->create_frame(width, height);

        for (size_t i = 0; i < _MAX_LAYERS; i++) {
            this->m_layers[i] = (RenderLayer){
                i,
                true,
                std::vector<GameObject*>()
            };
        }
    }

    void Renderer::create_frame(uint32_t width, uint32_t height) {
        this->m_frame_width = width;
        this->m_frame_height = height;

        if (IsRenderTextureValid(this->frame)) {
            UnloadRenderTexture(this->frame);
        }
        this->frame = LoadRenderTexture(m_frame_width, m_frame_height);
    }

    void Renderer::add_to_layer(GameObject* object, size_t layer) {
        if (layer < 0 || layer >= _MAX_LAYERS) {
            debug_error("Layer %ul is out of bounds", layer);
            return;
        }

        object->layer = layer;
        this->m_layers[layer].objects.push_back(object);
    }

    void Renderer::remove_from_layer(GameObject* object) {
        size_t current_layer = object->layer;

        bool found = false;
        RenderLayer* layer = &this->m_layers[current_layer];
        for (auto it = layer->objects.begin(); it != layer->objects.end(); it++) {
            if (*it == object) {
                layer->objects.erase(it);
                found = true;
                break;
            }
        }

        if (!found) {
            debug_error("Object not found at expected RenderLayer");
        }
    }

    void Renderer::draw(Camera2D* camera) const {
        BeginTextureMode(this->frame);
            ClearBackground(DARKGRAY);

            BeginMode2D(*camera);
                for (const RenderLayer& layer : this->m_layers) {
                    for (GameObject* object : layer.objects) {
                        object->draw();
                    }
                }
            EndMode2D();

        EndTextureMode();
    }

    void Renderer::reset() {
        for (RenderLayer& layer : this->m_layers) {
            layer.objects.clear();
        }
    }

    uint32_t Renderer::get_width() const {
        return this->m_frame_width;
    }

    uint32_t Renderer::get_height() const {
        return this->m_frame_height;
    }
}
