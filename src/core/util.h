#pragma once

#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <exception>
#include <string>

#include "nlohmann/detail/exceptions.hpp"
#include "raylib.h"
#include "raymath.h"
#include "nlohmann/json.hpp"

namespace bacon
{
	inline bool close_enough(float x, float y, float eps = 1)
	{
		return std::abs(x - y) <= eps;
	}

	inline Vector2 rotate_about_point(Vector2 point, Vector2 pivot, float rotation)
	{
		float radians = rotation * DEG2RAD;
		float cosr = cosf(radians);
		float sinr = sinf(radians);

		Vector2 translated = Vector2Subtract(point, pivot);

		float new_x = translated.x * cosr - translated.y * sinr;
		float new_y = translated.x * sinr + translated.y * cosr;

		Vector2 new_point = {new_x + pivot.x, new_y + pivot.y};
		return new_point;
	}

	inline void DrawRectangleLinesPro(Rectangle rect, float rotation, float thickness, Color color)
	{
		float radians = rotation * DEG2RAD;
		float cosr = cosf(radians);
		float sinr = sinf(radians);

		float hx = rect.width / 2.f;
		float hy = rect.height / 2.f;

		Vector2 local[4] = {
			{-hx, -hy},
			{hx, -hy},
			{hx, hy},
			{-hx, hy},
		};

		Vector2 points[4];
		for (int i = 0; i < 4; i++)
		{
			float rx = local[i].x * cosr - local[i].y * sinr;
			float ry = local[i].x * sinr + local[i].y * cosr;

			points[i] = {rect.x + rx, rect.y + ry};
		}

		for (int i = 0; i < 4; i++)
		{
			DrawLineEx(points[i], points[(i + 1) % 4], thickness, color);
		}
	}

	inline bool IsMouseLeftDoubleClick()
	{
		static double last_click_time = 0.0f;
		static double double_click_interval = 0.3f; // Seconds

		bool double_click = false;

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			double current_time = GetTime();
			if ((current_time - last_click_time) < double_click_interval)
			{
				double_click = true;
			}

			last_click_time = current_time;
		}

		return double_click;
	}

	inline double b_fmod(double x, double y)
	{
		double result = std::fmod(x, y);
		if (result < 0)
		{
			result += std::abs(y);
		}
		return result;
	}

	inline std::string json_read_string(const nlohmann::json& json, const char* key)
	{
		try { return json.at(key); }
		catch (const std::exception&) { return ""; }
	}

	inline bool json_read_bool(const nlohmann::json& json, const char* key)
	{
		try { return json.at(key); }
		catch (const std::exception&) { return false; }
	}

	inline int8_t json_read_int8(const nlohmann::json& json, const char* key)
	{
		try { return json.at(key); }
		catch (const std::exception&) { return 0; }
	}

	inline int16_t json_read_int16(const nlohmann::json& json, const char* key)
	{
		try { return json.at(key); }
		catch (const std::exception&) { return 0; }
	}

	inline int32_t json_read_int32(const nlohmann::json& json, const char* key)
	{
		try { return json.at(key); }
		catch (const std::exception&) { return 0; }
	}

	inline int64_t json_read_int64(const nlohmann::json& json, const char* key)
	{
		try { return json.at(key); }
		catch (const std::exception&) { return 0; }
	}

	inline uint8_t json_read_uint8(const nlohmann::json& json, const char* key)
	{
		try { return json.at(key); }
		catch (const std::exception&) { return 0; }
	}

	inline uint16_t json_read_uint16(const nlohmann::json& json, const char* key)
	{
		try { return json.at(key); }
		catch (const std::exception&) { return 0; }
	}

	inline uint32_t json_read_uint32(const nlohmann::json& json, const char* key)
	{
		try { return json.at(key); }
		catch (const std::exception&) { return 0; }
	}

	inline uint64_t json_read_uint64(const nlohmann::json& json, const char* key)
	{
		try { return json.at(key); }
		catch (const std::exception&) { return 0; }
	}

	inline float json_read_float(const nlohmann::json& json, const char* key)
	{
		try { return json.at(key); }
		catch (const std::exception&) { return 0.0f; }
	}

	inline double json_read_double(const nlohmann::json& json, const char* key)
	{
		try { return json.at(key); }
		catch (const std::exception&) { return 0.0; }
	}

	inline size_t json_read_size_t(const nlohmann::json& json, const char* key)
	{
		try { return json.at(key); }
		catch (const std::exception&) { return 0; }
	}

	inline Vector2 json_read_vector2(const nlohmann::json& json, const char* key)
	{
		try { return (Vector2){json.at(key)[0], json.at(key)[1]}; }
		catch (const std::exception&) { return (Vector2){0.0f, 0.0f}; }
	}

	inline Color json_read_color(const nlohmann::json& json, const char* key)
	{
		try { return (Color){json.at(key)[0], json.at(key)[1], json.at(key)[2], json.at(key)[3]}; }
		catch (const std::exception&) { return (Color){0, 0, 0, 0}; }
	}

	inline void _debug_log_impl(const char* level, const char* file, int line,
								const char* text, ...)
	{
		std::fprintf(stderr, "[%s][%s:%d] ", level, file, line);

		va_list args;
		va_start(args, text);
		std::vfprintf(stderr, text, args);
		va_end(args);

		std::fprintf(stderr, "\n");
	}

#ifdef DEBUG_BUILD
#define debug_log(text, ...) \
	_debug_log_impl("INFO", __FILE__, __LINE__, text, ##__VA_ARGS__)

#define debug_warn(text, ...) \
	_debug_log_impl("WARN", __FILE__, __LINE__, text, ##__VA_ARGS__)

#define debug_error(text, ...) \
	_debug_log_impl("ERROR", __FILE__, __LINE__, text, ##__VA_ARGS__)
#else
#define debug_log(...) ((void)0);
#define debug_warn(...) ((void)0);
#define debug_error(...) ((void)0);
#endif

} // namespace bacon
