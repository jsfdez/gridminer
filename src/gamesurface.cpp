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

	for (auto& gem : m_gems)
	{
		if (gem.Update(event) == AbstractSurface::Status::ANIMATION)
		{
			status = AbstractSurface::Status::ANIMATION;
			break;
		}
	}

	if (status == Status::CONTINUE) 
	{
		switch (event.type)
		{
		case SDL_MOUSEMOTION:
			OnMouseMoveEvent(event);
			break;
		case SDL_MOUSEBUTTONDOWN:
			OnMouseClickEvent(event);
			break;
		}
	}
    return status;
}

void GameSurface::Animate(
	const std::chrono::time_point<std::chrono::system_clock>& time)
{
	const auto bind = std::bind((Status(GemSurface::*)(decltype(time)))
		&GemSurface::Animate, std::placeholders::_1, std::ref(time));
	std::for_each(m_gems.begin(), m_gems.end(), bind);
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
	m_background = ImageLoader::Load(k_imageBackground);
	decltype(FindGroups()) sets;
	m_selectedGem = k_unselectedGem;
	std::size_t times = 0;
	do
	{
		m_gems.clear();
		for (std::size_t i = 0, size = GEM_COUNT; i < size; ++i)
		{
			m_gems.emplace_back(*this);
			auto position = CalculateGemPosition(i);
			auto color = static_cast<GemSurface::Color>(
				std::rand() % GemSurface::COLOR_COUNT);
			m_gems.back().SetColor(color);
			//m_gems.back().SetPosition(position.X, position.Y);
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

void GameSurface::OnMouseMoveEvent(const SDL_Event& event)
{
	auto& e = reinterpret_cast<const SDL_MouseMotionEvent&>(event);
	Position position;
	position.X = e.x;
	position.Y = e.y;
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

void GameSurface::OnMouseClickEvent(const SDL_Event& event)
{
	auto& e = reinterpret_cast<const SDL_MouseButtonEvent&>(event);
	Position position;
	position.X = e.x;
	position.Y = e.y;
	for (auto i = 0u; i < m_gems.size(); ++i)
	{
		auto& child = m_gems[i];
		if (m_selectedGem != k_unselectedGem)
		{
			if (child.Contains(position))
			{
				if (AreContiguous(m_selectedGem, i))
				{
					Swap(m_selectedGem, i);
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

std::uint8_t GameSurface::GetGemIndex(const GemSurface& gem) const
{
	decltype(m_gems.size()) size = m_gems.size();
	for (decltype(size) i = 0; i < size; ++i)
	{
		if (&m_gems[i] == &gem)
		{
			return static_cast<decltype(GetGemIndex(gem))>(i);
		}
	}
	return std::numeric_limits<decltype(GetGemIndex(gem))>::max();
}

Position GameSurface::CalculateGemPosition(std::uint8_t i) const
{
	Position position;
	position.X = OFFSET_X + GemSurface::WIDTH * (i % COLUMNS);
	position.Y = OFFSET_Y + GemSurface::HEIGHT * (i / COLUMNS);
	return position;
}

void GameSurface::Swap(std::uint8_t first, std::uint8_t second)
{
	GemSurface::StartSwapping(m_gems[first], m_gems[second]);
	m_swapping = std::make_pair(first, second);
}
