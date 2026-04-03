#include "lua_api.h"

#include "raylib.h"

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
	void Lua::register_classes_2d(sol::state& state)
	{
		register_core_classes(state);


	}
}
