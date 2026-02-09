#pragma once

#include <array>
#include <vector>

#include "raylib.h"

#include "core/2D/camera_object.h"
#include "core/2D/game_object.h"

namespace bacon
{
    typedef struct
    {
        size_t layer_num;
        bool visible;
        std::vector<GameObject*> objects;
    } RenderLayer;

    class Renderer
    {
         public:
        static constexpr size_t _MAX_LAYERS = 10;

        RenderTexture2D frame;

        Renderer(uint32_t width, uint32_t height);
        ~Renderer() = default;

        void create_frame(uint32_t width, uint32_t height);
        void add_to_layer(GameObject* object, size_t layer);
        void remove_from_layer(GameObject* object);
        void draw(Camera2D* camera) const;

        void reset();

        uint32_t get_width() const;
        uint32_t get_height() const;

         private:
        uint32_t m_frame_width = 0;
        uint32_t m_frame_height = 0;
        std::array<RenderLayer, _MAX_LAYERS> m_layers;
    };
} // namespace bacon
