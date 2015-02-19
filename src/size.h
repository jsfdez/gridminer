#pragma once

#include <utility>

struct Size : private std::pair<std::size_t, std::size_t>
{
	decltype(first)& Width = first;
	decltype(second)& Height = second;
};
