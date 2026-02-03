#pragma once

#include <string>

#include "core/2D/game_manager.h"

namespace bacon {
    namespace file {
        bool save_project(const GameManager& manager);
        bool load_project(GameManager& manager);
        bool create_new_project(const GameManager& manager);
        std::string abs_path_to_relative(std::string path);
        std::string relative_path_to_abs(std::string path);
    }
}
