#pragma once

#include <string>

#include "nfd.h"

#include "core/2D/game_manager.h"

namespace bacon
{
namespace file
{
nfdresult_t save_project(const GameManager& manager);
nfdresult_t load_project(GameManager& manager, bool show_dialog);
nfdresult_t create_new_project(const GameManager& manager);
std::string abs_path_to_relative(std::string path);
std::string relative_path_to_abs(std::string path);
} // namespace file
} // namespace bacon
