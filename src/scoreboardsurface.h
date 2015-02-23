#pragma once

#include "AbstractTimerSurface.h"

#include <chrono>
#include <memory>
#include <cstdint>
#include <SDL_ttf.h>

class GameSurface;

class ScoreboardSurface : public AbstractTimerSurface
{
	std::uint32_t m_score = 0;
	const GameSurface& m_game;
	std::unique_ptr<TTF_Font, decltype(TTF_CloseFont)*> m_titleFont;
	std::unique_ptr<TTF_Font, decltype(TTF_CloseFont)*> m_font;
	std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> m_title;
	std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> m_timeTitle;
	std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> m_timeText;
	std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> m_scoreTitle;
	std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> m_scoreText;
	const SDL_Color m_defaultColor;

public:
	ScoreboardSurface(GameSurface& gameSurface);
	~ScoreboardSurface();

	virtual Status Update(const SDL_Event& event) override;
	virtual Status Update(const std::chrono::time_point<std::chrono::system_clock>& time) override;
	virtual void Render(SDL_Surface& surface) override;
	virtual bool Contains(const Position& position) const override;
	virtual void Start() override;

	void AddScore(std::uint32_t points);

	void UpdateScore();

};
