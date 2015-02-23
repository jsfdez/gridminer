#pragma once

#include "abstracttimersurface.h"

#include <SDL.h>
#include <vector>
#include <memory>

#include "position.h"

class GameSurface;

class CandlewickSurface : public AbstractTimerSurface
{
	const GameSurface& m_game;
	std::vector<Position> m_path;
	SDL_Rect m_destRect;
	std::size_t m_frame = 0;
	enum
	{
		WIDTH = 10,
		HEIGHT = WIDTH,
	};

	void CreatePath();

public:
	CandlewickSurface(const GameSurface& game);
	~CandlewickSurface();

	virtual Status Update(const SDL_Event& event) override;
	virtual Status Update(const std::chrono::time_point<std::chrono::system_clock>& time) override;
	virtual void Render(SDL_Surface& surface) override;
	virtual bool Contains(const Position& position) const override;
};
