#pragma once

#include <string>

#include "nfd.h"

namespace bacon
{
	namespace file
	{
		nfdresult_t save_project();
		nfdresult_t load_project(bool show_dialog);
		nfdresult_t create_new_project();
		std::string abs_path_to_relative(std::string path);
		std::string relative_path_to_abs(std::string path);
	} // namespace file
} // namespace bacon
