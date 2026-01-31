#pragma once

#include <string>

#include "core/2D/game_manager.h"

namespace bacon {
    namespace file {
        bool save_project(const GameManager& manager, std::string filename);
        bool load_project(GameManager& manager, std::string filename);
        bool create_new_project(GameManager& manager, std::string filename);
        std::string abs_path_to_relative(std::string path);
        std::string relative_path_to_abs(std::string path);
    }
}
