#include "gamesurface.h"

#include <chrono>
#include <cstdint>
#include <algorithm>
#include <functional>

#include "SDL.h"
#include "files.h"
#include "imageloader.h"
#include "gamesurface.h"

std::uint8_t k_unselectedGem = std::numeric_limits<std::uint8_t>::max();

GameSurface::GameSurface()
{
	Create();
}

GameSurface::~GameSurface()
{
	Destroy();
}

AbstractSurface::Status GameSurface::Update(const SDL_Event& event)
{
	auto status = Status::CONTINUE;

	switch (event.type)
	{
	case SDL_MOUSEMOTION:
		OnMouseMoveEvent(reinterpret_cast<const SDL_MouseMotionEvent&>(event));
		break;
	case SDL_MOUSEBUTTONDOWN:
		OnMouseClickEvent(reinterpret_cast<const SDL_MouseButtonEvent&>(event));
		break;
	}
    return status;
}

AbstractSurface::Status GameSurface::Update(
	const std::chrono::time_point<std::chrono::system_clock>& time)
{
	auto status = Status::CONTINUE;
	const auto bind = std::bind((Status(GemSurface::*)(decltype(time)))
		&GemSurface::Update, std::placeholders::_1, std::ref(time));
	std::for_each(m_gems.begin(), m_gems.end(), bind);
	return status;
}

void GameSurface::Render(SDL_Surface& surface)
{
	SDL_BlitSurface(m_background.get(), nullptr, &surface, nullptr);
	std::for_each(m_gems.begin(), m_gems.end(), 
		std::bind(
			&GemSurface::Render, std::placeholders::_1, 
			std::ref(surface)
		)
	);
}

void GameSurface::Create()
{
	m_background = ImageLoader::Load(IMAGE_BACKGROUND);
	decltype(FindGroups()) sets;
	m_selectedGem = k_unselectedGem;
	std::size_t times = 0;
	do
	{
		m_gems.clear();
		for (std::size_t i = 0, size = GEM_COUNT; i < size; ++i)
		{
			m_gems.emplace_back(*this);
			Position position;
			position.X = OFFSET_X + GemSurface::WIDTH * (i % COLUMNS);
			position.Y = OFFSET_Y + GemSurface::HEIGHT * (i / COLUMNS);
			auto color = static_cast<GemSurface::Color>(
				std::rand() % GemSurface::COLOR_COUNT);
			m_gems.back().SetColor(color);
			m_gems.back().SetPosition(position.X, position.Y);
		}
		sets.swap(FindGroups());
		++times;
	} while (!sets.empty());
	
	SDL_Log("Acceptable grid found in %d iterations", times);
}

void GameSurface::Destroy()
{
	m_background.reset();
}

std::set<std::vector<std::uint8_t>> GameSurface::FindGroups() const
{
	decltype(FindGroups()) ret;
	typedef std::function<void(std::vector<std::uint8_t>&)> MatchFunction;
	const MatchFunction matchLeft = [&](std::vector<std::uint8_t>& data)
	{
		auto last = data.back();
		auto next = last - 1;
		if ((last % COLUMNS) != 0 
			&& m_gems[last].GetColor() == m_gems[next].GetColor())
		{
			data.push_back(last - 1);
			matchLeft(data);
		}
	};
	const MatchFunction matchRight = [&](std::vector<std::uint8_t>& data)
	{
		auto last = data.back();
		auto next = last + 1;
		if ((last % COLUMNS) != COLUMNS - 1
			&& m_gems[last].GetColor() == m_gems[next].GetColor())
		{
			data.push_back(last + 1);
			matchRight(data);
		}
	};
	const MatchFunction matchUp = [&](std::vector<std::uint8_t>& data)
	{
		auto last = data.back();
		auto next = last - COLUMNS;
		if ((last / COLUMNS) != 0
			&& m_gems[last].GetColor() == m_gems[next].GetColor())
		{
			data.push_back(last - COLUMNS);
			matchUp(data);
		}
	};
	const MatchFunction matchDown = [&](std::vector<std::uint8_t>& data)
	{
		auto last = data.back();
		auto next = last + COLUMNS;
		if (last + COLUMNS < GEM_COUNT
			&& m_gems[last].GetColor() == m_gems[next].GetColor())
		{
			data.push_back(last + COLUMNS);
			matchDown(data);
		}
	};
	const auto size = m_gems.size();
	for (std::uint8_t i = 0u; i < size; ++i)
	{
		auto start = std::chrono::system_clock::now();
		std::vector<std::uint8_t> horizontal{ i }, vertical{ i };
		matchLeft(horizontal);
		std::reverse(horizontal.begin(), horizontal.end());
		matchRight(horizontal);

		matchUp(vertical);
		std::reverse(vertical.begin(), vertical.end());
		matchDown(vertical);

		if (horizontal.size() >= 3)
			ret.insert(horizontal);
		if (vertical.size() >= 3)
			ret.insert(vertical);
	}
	return ret;
}

void GameSurface::OnMouseMoveEvent(const SDL_MouseMotionEvent& event)
{
	Position position;
	position.X = event.x;
	position.Y = event.y;
	for (auto& child : m_gems)
	{
		if (child.Contains(position))
		{
			if (child.IsHover())
				child.SetHover(true);
		}
		else if (child.IsHover())
			child.SetHover(false);
	}
}

void GameSurface::OnMouseClickEvent(const SDL_MouseButtonEvent& event)
{
	Position position;
	position.X = event.x;
	position.Y = event.y;
	for (auto i = 0u; i < m_gems.size(); ++i)
	{
		auto& child = m_gems[i];
		if (m_selectedGem != k_unselectedGem)
		{
			if (child.Contains(position))
			{
				if (AreContiguous(m_selectedGem, i))
				{
					std::swap(m_gems[m_selectedGem], m_gems[i]);
					auto&& sets = FindGroups();
					if (sets.empty())
					{
						std::swap(m_gems[m_selectedGem], m_gems[i]);
						SDL_assert(FindGroups().empty());
					}
					m_selectedGem = k_unselectedGem;
				}
				else
				{
					m_selectedGem = i;
				}
			}
		}
		else if (child.Contains(position))
		{
			m_selectedGem = i;
		}
	}
}

bool GameSurface::Contains(const Position&) const
{
	return true;
}

bool GameSurface::AreContiguous(std::uint8_t first, std::uint8_t second) const
{
	if (first == second)
		return false;
	if (first / COLUMNS == second / COLUMNS)
		return first + 1 == second || first - 1 == second;
	else
	{
		const auto firstColumn = first % COLUMNS;
		const auto secondColumn = second % COLUMNS;
		return firstColumn == secondColumn;
	}
}
