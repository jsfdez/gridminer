#pragma once

struct Position
{
	std::int32_t X = 0; 
	std::int32_t Y = 0;

	Position() = default;

	Position(std::int32_t x, std::int32_t y)
		: X(x), Y(y)
	{
	}
};

inline Position operator+(const Position& left, const Position& right)
{
	Position ret;
	ret.X = left.X + right.X;
	ret.Y = left.Y + right.Y;
	return ret;
}

inline Position operator-(const Position& left, const Position& right)
{
	Position ret;
	ret.X = left.X - right.X;
	ret.Y = left.Y - right.Y;
	return ret;
}
