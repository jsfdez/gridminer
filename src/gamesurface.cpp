#include "gamesurface.h"

#include <chrono>
#include <cstdint>
#include <algorithm>
#include <functional>

#include "SDL.h"
#include "files.h"
#include "loader.h"
#include "gamesurface.h"

std::uint8_t k_noGem = std::numeric_limits<std::uint8_t>::max();

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

	if (m_animation == Animation::NO_ANIMATION) switch (event.type)
	{
	case SDL_MOUSEMOTION:
		OnMouseMoveEvent(event);
		break;
	case SDL_MOUSEBUTTONDOWN:
		OnMouseClickEvent(event);
		break;
	}
    return status;
}

AbstractSurface::Status GameSurface::Update(
	const std::chrono::time_point<std::chrono::system_clock>& time)
{
	Status status = AbstractSurface::Status::CONTINUE;
	for (auto& gem : m_gems)
	{
		if (gem.Update(time) == AbstractSurface::Status::ANIMATION)
		{
			status = AbstractSurface::Status::ANIMATION;
		}
	}
	if (status == AbstractSurface::Status::CONTINUE)
	{
		if (m_animation == Animation::FALL_ANIMATION)
		{
			m_animation = Animation::NO_ANIMATION;
		}
		else if (m_animation == Animation::SWAPPING_ANIMATION)
		{
			SDL_assert(m_swapping.first != m_swapping.second);
			m_destroyingGems.swap(FindGroups());
			if (m_destroyingGems.empty())
			{
				Swap(m_swapping.first, m_swapping.second, true);
			}
			else StartDestruction();
		}
		else if (m_animation == Animation::DESTROY_ANIMATION)
		{
			while(!FillRow());
			m_destroyingGems.swap(FindGroups());
			if (!m_destroyingGems.empty())
				StartDestruction();
		}
		else if (m_animation == Animation::ROLLBACK_ANIMATION)
		{
			SDL_assert(m_swapping.first != m_swapping.second);
#ifdef _DEBUG
			auto&& groups = FindGroups();
			SDL_assert(groups.empty());
#endif
			m_animation = Animation::NO_ANIMATION;
			m_selectedGem = k_noGem;
		}
	}
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
	m_background = loader::Image(k_imageBackground);
	decltype(FindGroups()) sets;
	m_selectedGem = k_noGem;
	std::size_t times = 0;
	do
	{
		// i = gems.size() to avoid unnecesary clears
		for (auto i = m_gems.size(); i < GEM_COUNT; ++i)
		{
			auto position = CalculateGemPosition(i);
			position.X = 0;
			position.Y = -OFFSET_Y - position.Y;
			m_gems.emplace_back(*this, position);
		}
		sets.swap(FindGroups());
		++times;
		for (auto& set : sets) // Randomize found sets
		{
			for (auto& i : set)
			{
				auto color = static_cast<GemColor>(
					std::rand() % GemSurface::COLOR_COUNT);
				m_gems[i].SetColor(color);
			}
		}
	} while (!sets.empty());

	m_animation = Animation::FALL_ANIMATION;
	
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
		if (m_selectedGem != k_noGem)
		{
			if (child.Contains(position))
			{
				if (AreContiguous(m_selectedGem, i))
				{
					Swap(m_selectedGem, i, false);
					m_selectedGem = k_noGem;
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
		return SDL_abs(first - second) == COLUMNS;
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

void GameSurface::Swap(std::uint8_t first, std::uint8_t second, bool rollback)
{
	GemSurface::StartSwapping(m_gems[first], m_gems[second]);
	m_swapping = std::make_pair(first, second);
	m_animation = rollback ? Animation::ROLLBACK_ANIMATION 
		: Animation::SWAPPING_ANIMATION;
}

bool GameSurface::FillRow()
{
	const auto findAboveGem = [&](decltype(m_gems.size()) gemIndex)
	{
		for (int i = gemIndex - COLUMNS; i >= 0; i -= COLUMNS)
		{
			if (m_gems[i].GetColor() != GemColor::EMPTY)
			{
				return static_cast<decltype(k_noGem)>(i);
			}
		}
		return k_noGem;
	};
	bool nextRow = true;
	for (auto row = 0; row < ROWS && nextRow; ++row)
	{
		for (auto column = 0; column < COLUMNS; ++column)
		{
			const auto currentRow = ROWS - row - 1;
			auto index = column + currentRow * COLUMNS;
			if (m_gems[index].IsEmpty())
			{
				auto otherIndex = findAboveGem(index);
				auto position = CalculateGemPosition(index);
				position.X = 0;
				if (otherIndex != k_noGem)
				{
					position.Y = -(position.Y - CalculateGemPosition(otherIndex).Y);
					std::swap(m_gems[index], m_gems[otherIndex]);
					m_gems[index].SetOffset(position);
				}
				else
				{
					position.Y = -OFFSET_Y - position.Y;
					GemSurface gem(*this, position);
					m_gems[index] = gem;
					m_gems[index].SetOffset(position);
				}
				m_animation = Animation::FALL_ANIMATION;
				nextRow = false;
			}
		}
	}
	return nextRow;
}

void GameSurface::StartDestruction()
{
	for (auto& group : m_destroyingGems)
	{
		SDL_assert(group.size() >= 3);
		auto diff = group[1] - group[0];
		for (auto& i : group)
		{
			m_gems[i].Destroy(diff == 1
				? GemSurface::Destruction::HORIZONTAL
				: GemSurface::Destruction::VERTICAL);
		}
	}
	m_animation = Animation::DESTROY_ANIMATION;
	m_swapping.first = m_swapping.second;
	m_selectedGem = k_noGem;
}
