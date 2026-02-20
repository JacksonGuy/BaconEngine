#pragma once

#include <cmath>
#include <cstdarg>
#include <cstdio>

#include "raylib.h"

namespace bacon
{
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

	inline bool close_enough(float x, float y, float eps = 1)
	{
		return std::abs(x - y) <= eps;
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

	inline double b_fmod(double x, double y)
	{
		double result = std::fmod(x, y);
		if (result < 0)
		{
			result += std::abs(y);
		}
		return result;
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
