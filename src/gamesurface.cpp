#include "gamesurface.h"

#include <cstdint>
#include <algorithm>
#include <functional>

#include "SDL.h"
#include "files.h"
#include "imageloader.h"

GameSurface::GameSurface()
{
	Create();
}

GameSurface::~GameSurface()
{
	Destroy();
}

GameSurface::Status GameSurface::Update(const SDL_Event& event)
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
	for (std::size_t i = 0, size = 8 * 8; i < size; ++i)
	{
		Position position;
		position.X = OFFSET_X + GemSurface::WIDTH * (i % COLUMNS);
		position.Y = OFFSET_Y + GemSurface::HEIGHT * (i / COLUMNS);
		auto color = static_cast<GemSurface::Color>(
			std::rand() % GemSurface::COLOR_COUNT);
		m_gems[i].SetColor(color);
		m_gems[i].SetPosition(position);
	}
}

void GameSurface::Destroy()
{
	m_background.reset();
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
