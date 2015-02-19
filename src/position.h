#pragma once

#include <utility>

struct Position : private std::pair<std::size_t, std::size_t>
{
	Position& operator=(const Position& other) = default;

	decltype(first)& X = first;
	decltype(second)& Y = second;
};
