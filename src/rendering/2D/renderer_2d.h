#pragma once

#include <array>
#include <vector>

#include "raylib.h"

#include "core/2D/object_2d.h"

namespace bacon
{
    typedef struct
    {
        size_t layer_num;
        bool visible;
        std::vector<Object2D*> objects;
    } RenderLayer;

    class Renderer2D
    {
    public:
        static constexpr size_t _MAX_LAYERS = 10;

        RenderTexture2D frame;

        Renderer2D(uint32_t width, uint32_t height);
        ~Renderer2D() = default;

        void create_frame(uint32_t width, uint32_t height);
        void add_to_layer(Object2D* object, size_t layer);
        void remove_from_layer(Object2D* object);
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
