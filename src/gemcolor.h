#pragma once

#include <cstdint>

enum class GemColor : std::uint8_t
{
	BLUE = 0,
	GREEN,
	PURPLE,
	RED,
	YELLOW,

	EMPTY = 0xFF
};
