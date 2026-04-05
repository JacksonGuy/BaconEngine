#include "lua_api.h"

#include "raylib.h"

#include "core/2D/entity_2d.h"

void register_core_classes(sol::state& state)
{
	sol::usertype<Vector2> vector2_type = state.new_usertype<Vector2>(
		"Vector2",
		sol::constructors<Vector2()>()
	);
	vector2_type["x"] = &Vector2::x;
	vector2_type["y"] = &Vector2::y;

	sol::usertype<Color> color_type = state.new_usertype<Color>(
		"Color",
		sol::constructors<Color()>()
	);
	color_type["r"] = &Color::r;
	color_type["g"] = &Color::g;
	color_type["b"] = &Color::b;
	color_type["a"] = &Color::a;
}

namespace bacon
{
	LuaVar::LuaVar()
	{
		type = LuaVar_t::NONE;
		float_val = 0.f;
		int_val = 0;
		bool_val = false;
		str_val = std::string();
	}

	void Lua::register_classes_2d(sol::state& state)
	{
		register_core_classes(state);

		sol::usertype<Entity2D> entity_type = state.new_usertype<Entity2D>(
			"Entity2D",
			sol::constructors<Entity2D()>(),
			sol::base_classes, sol::bases<Object2D, GameObject>()
		);
	}
}
