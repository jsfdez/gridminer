#pragma once

#include "abstractsurface.h"

#include <chrono>
#include <cstdint>

class GameSurface;

class ScoreboardSurface : public AbstractSurface
{
	std::chrono::system_clock::time_point m_startTime;
	bool m_started = false;
	bool m_running = false;
	std::uint32_t m_score = 0;
	const GameSurface& m_game;

public:
	enum Constants
	{
		MAX_TIME = 60,
	};

	ScoreboardSurface(GameSurface& gameSurface);
	~ScoreboardSurface();

	virtual Status Update(const SDL_Event& event) override;
	virtual Status Update(const std::chrono::time_point<std::chrono::system_clock>& time) override;
	virtual void Render(SDL_Surface& surface) override;
	virtual bool Contains(const Position& position) const override;

	void Start();
	void Stop();
};
