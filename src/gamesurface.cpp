#include "gamesurface.h"

#include <chrono>
#include <cstdint>
#include <algorithm>
#include <functional>

#include "SDL.h"
#include "files.h"
#include "imageloader.h"
#include "gamesurface.h"

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
	decltype(FindSets()) sets;
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
		//sets = std::move(FindSets());
		++times;
	} while (!sets.empty());
	SDL_Log("Acceptable grid found in %d iterations", times);
}

void GameSurface::Destroy()
{
	m_background.reset();
}

std::set<std::set<std::size_t>> GameSurface::FindSets() const
{
	decltype(FindSets()) ret;
	typedef std::function<void(std::set<std::size_t>&)> MatchFunction;
	const MatchFunction matchLeft = [&](std::set<std::size_t>& set)
	{
		auto first = *set.begin();
		if ((first % COLUMNS) != 0 
			&& m_gems[first].GetColor() == m_gems[first - 1].GetColor())
		{
			set.insert(first - 1);
			matchLeft(set);
		}
	};
	const MatchFunction matchRight = [&](std::set<std::size_t>& set)
	{
		auto last = *set.rbegin();
		if ((last % COLUMNS) != COLUMNS - 1
			&& m_gems[last].GetColor() == m_gems[last + 1].GetColor())
		{
			set.insert(last);
			matchLeft(set);
		}
	};
	const MatchFunction matchUp = [&](std::set<std::size_t>& set)
	{
		auto first = *set.begin();
		if ((first / COLUMNS) != 0
			&& m_gems[first].GetColor() == m_gems[first - COLUMNS].GetColor())
		{
			set.insert(first - COLUMNS);
			matchUp(set);
		}
	};
	const MatchFunction matchDown = [&](std::set<std::size_t>& set)
	{
		auto last = *set.rbegin();
		if (last + COLUMNS < GEM_COUNT
			&& m_gems[last].GetColor() == m_gems[last + COLUMNS].GetColor())
		{
			set.insert(last + COLUMNS);
			matchDown(set);
		}
	};
	const auto size = m_gems.size();
	for (auto i = 0u; i < size; ++i)
	{
		auto start = std::chrono::system_clock::now();
		std::set<std::size_t> left, right, up, down;
		left.insert(i);
		matchLeft(left);
		right.insert(i);
		matchRight(right);

		if (left.size() > 1 && right.size() > 1)
		{
			for (const auto i : right)
				left.insert(i);
			if (left.size() >= 3)
				ret.insert(left);
		}
		else
		{
			if (left.size() >= 3) 
				ret.insert(left);
			if (right.size() >= 3) 
				ret.insert(right);
		}

		up.insert(i);
		matchUp(up);
		down.insert(i);
		matchDown(down);
		if (up.size() > 1 && down.size() > 1)
		{
			for (const auto i : down) 
				up.insert(i);
			if (up.size() >= 3) 
				ret.insert(up);
		}
		else
		{
			if (up.size() >= 3) 
				ret.insert(up);
			if (down.size() >= 3) 
				ret.insert(down);
		}
		auto elapsed = std::chrono::system_clock::now() - start;
		SDL_Log("Finding sets %d ms", 
			std::chrono::duration_cast<std::chrono::seconds>(elapsed));
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
	for (auto& child : m_gems)
	{
		if (child.Contains(position))
		{
			if (!child.IsSelected())
				child.SetSelected(true);
		}
		else if (child.IsSelected())
			child.SetSelected(false);
	}
}

bool GameSurface::Contains(const Position&) const
{
	return true;
}
