#pragma once

#include <vector>

#include "raylib.h"

namespace bacon {
    class ResourceManager {
        public:
            std::vector<Texture2D> textures;

            ResourceManager() = default;
            ~ResourceManager() = default;
            void load_texture(const char* path);

        private:
    };
}
