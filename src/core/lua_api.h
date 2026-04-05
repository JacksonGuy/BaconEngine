#pragma once

#include <stdint.h>

#include <string>

#include "sol/sol.hpp"

namespace bacon
{
	enum class LuaVar_t : uint8_t
	{
		NONE = 0,
		STRING,
		FLOAT,
		INT,
		BOOL,
	};

	typedef struct LuaVar {
		LuaVar_t type;
		float float_val;
		int int_val;
		bool bool_val;
		std::string str_val;

		LuaVar();
	} LuaVar;

	namespace Lua
	{
		void register_functions_2d(sol::state& state);
		void register_classes_2d(sol::state& state);
	}
}
