#pragma once

#include "abstractsurface.h"

#include <map>
#include <vector>
#include <memory>

#include "gemsurface.h"

struct Position;
struct SDL_MouseMotionEvent;
struct SDL_MouseButtonEvent;

class GameSurface : public AbstractSurface
{
	enum
	{
		ROWS = 8,
		COLUMNS = ROWS,
		OFFSET_X = 330u + GemSurface::WIDTH / 2,
		OFFSET_Y = 100u + GemSurface::HEIGHT / 2,
	};

	std::shared_ptr<SDL_Surface> m_background;
	std::vector<std::unique_ptr<AbstractSurface>> m_children;

	void OnMouseMoveEvent(const SDL_MouseMotionEvent& event);
	void OnMouseClickEvent(const SDL_MouseButtonEvent& event);

public:
    GameSurface();
    ~GameSurface();

	virtual Status Update(const SDL_Event& event) override;
	virtual void Render(SDL_Surface& surface) override;
	virtual bool Contains(const Position& position) const override;

	void Create();
	void Destroy();
};
