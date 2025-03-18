#pragma once

#include <stdint.h>
#include <algorithm>
#include <cmath>

namespace types
{
	struct color8
	{
		uint8_t r = 0, g = 0, b = 0;
	};

	struct Pos
	{
		int x = 0;
		int y = 0;
	};

	inline color8 operator*(const color8& c, float s)
	{
		color8 color;
		color.r = static_cast<uint8_t>(std::clamp(c.r * s, 0.0f, 255.0f));
		color.g = static_cast<uint8_t>(std::clamp(c.g * s, 0.0f, 255.0f));
		color.b = static_cast<uint8_t>(std::clamp(c.b * s, 0.0f, 255.0f));
		
		return color;
	}

	inline int genRandom(int min, int max)
	{
		return (min + rand() % (max - min + 1));
	}
}