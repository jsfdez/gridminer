#pragma once

struct Position
{
	std::size_t X; 
	std::size_t Y;

	Position()
		: X(std::numeric_limits<decltype(X)>::max())
		, Y(std::numeric_limits<decltype(Y)>::max())
	{
	}
};
