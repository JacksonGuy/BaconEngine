#pragma once

#include <stdint.h>

#include <string>

#include "sol/sol.hpp"

namespace bacon
{
	enum class LuaVar_t : uint8_t
	{
		STRING = 0,
		FLOAT,
		INT,
		BOOL,
	};

	typedef struct {
		LuaVar_t type;
		union value {
			std::string str_val;
			float float_val;
			int int_val;
			bool bool_val;
		};
	} LuaVar;

	namespace Lua
	{
		void register_functions_2d(sol::state& state);
		void register_classes_2d(sol::state& state);
	}
}
