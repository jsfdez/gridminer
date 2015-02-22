#include "scoreboardsurface.h"

#include "gamesurface.h"

ScoreboardSurface::ScoreboardSurface(GameSurface& gameSurface)
	: m_game(gameSurface)
{
}

ScoreboardSurface::~ScoreboardSurface()
{
}

ScoreboardSurface::Status ScoreboardSurface::Update(const SDL_Event&)
{
	return Status::CONTINUE;
}

ScoreboardSurface::Status ScoreboardSurface::Update(
	const std::chrono::time_point<std::chrono::system_clock>& time)
{
	if (m_running && !m_started)
	{
		if (!m_started) m_startTime = time;
		else
		{
			auto diff = time - m_startTime;
			auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
				diff);
			if (seconds.count() >= MAX_TIME)
			{
				Stop();
				return Status::EXIT;
			}
		}
	}

	return Status::CONTINUE;
}

void ScoreboardSurface::Render(SDL_Surface& surface)
{
}

bool ScoreboardSurface::Contains(const Position& position) const
{
	return false;
}

void ScoreboardSurface::Start()
{
	m_started = false;
	m_running = true;
}

void ScoreboardSurface::Stop()
{
	m_running = m_started = false;
}
