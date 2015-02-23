#include "scoreboardsurface.h"

#include "files.h"
#include "constants.h"
#include "gamesurface.h"

ScoreboardSurface::ScoreboardSurface(GameSurface& gameSurface)
	: m_game(gameSurface)
	, m_titleFont(nullptr, TTF_CloseFont)
	, m_font(nullptr, TTF_CloseFont)
	, m_title(nullptr, SDL_FreeSurface)
	, m_timeTitle(nullptr, SDL_FreeSurface)
	, m_timeText(nullptr, SDL_FreeSurface)
	, m_scoreTitle(nullptr, SDL_FreeSurface)
	, m_scoreText(nullptr, SDL_FreeSurface)
	, m_defaultColor({ 0xFF, 0xFF, 0x00, 0xFF })
{
	TTF_Init();
	m_titleFont.reset(TTF_OpenFont(k_font.c_str(), 48));
	SDL_assert(m_titleFont);
	m_font.reset(TTF_OpenFont(k_font.c_str(), 30));
	SDL_assert(m_font);
	m_title.reset(TTF_RenderText_Solid(m_titleFont.get(), "Grid Miner",
		SDL_Color{ 0x00, 0x00, 0x00, 0xFF }));
	SDL_assert(m_title);

	m_timeTitle.reset(TTF_RenderText_Solid(m_font.get(), "Time", 
		m_defaultColor));
	m_scoreTitle.reset(TTF_RenderText_Solid(m_font.get(), "Score", 
		m_defaultColor));
	AddScore(0);
}

ScoreboardSurface::~ScoreboardSurface()
{
	TTF_Quit();
}

ScoreboardSurface::Status ScoreboardSurface::Update(const SDL_Event&)
{
	return Status::CONTINUE;
}

ScoreboardSurface::Status ScoreboardSurface::Update(
	const std::chrono::time_point<std::chrono::system_clock>& time)
{
	auto ret = AbstractTimerSurface::Update(time);

	std::uint32_t seconds = 0;
	if (m_started && m_running)
	{
		auto diff = time - m_startTime;
		seconds = static_cast<decltype(seconds)>(constants::MAX_TIME 
			- std::chrono::duration_cast<std::chrono::seconds>(diff).count());
	}

	char text[32];
	sprintf_s(text, "%d seconds left.", seconds);
	m_timeText.reset(TTF_RenderText_Solid(m_font.get(), text, m_defaultColor));
	return ret;
}

void ScoreboardSurface::Render(SDL_Surface& surface)
{
	{
		SDL_Rect rect{ constants::TITLE_X - m_title->w / 2, 
			constants::TITLE_Y - m_title->h, m_title->w, m_title->h };
		SDL_BlitSurface(m_title.get(), nullptr, &surface, &rect);
	}
	{
		SDL_Rect rect{ constants::TIME_TITLE_X - m_timeTitle->w / 2,
			constants::TIME_TITLE_Y - m_timeTitle->h, m_timeTitle->w, 
			m_timeTitle->h };
		SDL_BlitSurface(m_timeTitle.get(), nullptr, &surface, &rect);
	}
	{
		SDL_Rect rect{ constants::TIME_TITLE_X - m_timeTitle->w / 2,
			constants::TIME_TITLE_Y - m_timeTitle->h, m_timeTitle->w, 
			m_timeTitle->h };
		SDL_BlitSurface(m_timeTitle.get(), nullptr, &surface, &rect);
	}
	{
		SDL_Rect rect{ constants::TIME_X - m_timeText->w / 2,
			constants::TIME_Y - m_timeText->h, m_timeText->w, m_timeText->h };
		SDL_BlitSurface(m_timeText.get(), nullptr, &surface, &rect);
	}
	{
		SDL_Rect rect{ constants::SCORE_TITLE_X - m_scoreTitle->w / 2,
			constants::SCORE_TITLE_Y - m_scoreTitle->h, m_scoreTitle->w, 
			m_scoreTitle->h };
		SDL_BlitSurface(m_scoreTitle.get(), nullptr, &surface, &rect);
	}
	{
		SDL_Rect rect{ constants::SCORE_X - m_scoreText->w / 2,
			constants::SCORE_Y - m_scoreText->h, m_scoreText->w, 
			m_scoreText->h };
		SDL_BlitSurface(m_scoreText.get(), nullptr, &surface, &rect);
	}
}

bool ScoreboardSurface::Contains(const Position&) const
{
	return false;
}

void ScoreboardSurface::AddScore(std::uint32_t points)
{
	char string[32];
	m_score += points;
	sprintf_s(string, "%d", m_score);
	m_scoreText.reset(TTF_RenderText_Solid(m_font.get(), string, 
		m_defaultColor));
}
