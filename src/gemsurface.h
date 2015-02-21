#pragma once

#include "abstractsurface.h"

#include <map>
#include <SDL.h>
#include <memory>
#include <cstdint>
#include <utility>

#include "size.h"
#include "position.h"

class GameSurface;
struct SDL_Surface;

class GemSurface : public AbstractSurface
{
	std::shared_ptr<SDL_Surface> m_image;
	Position m_position;
	bool m_hover = false;
	bool m_selected = false;
	const GameSurface& m_game;

	std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> CloneSurface();

public:
	enum class Color : std::uint8_t
	{
		BLUE = 0,
		GREEN,
		PURPLE,
		RED,
		YELLOW,

		EMPTY = 0xFF
	};

	enum
	{
		WIDTH = 42u,
		HEIGHT = WIDTH,
		COLOR_COUNT = static_cast<std::size_t>(Color::YELLOW) + 1,
	};

	GemSurface(const GameSurface& game);
	GemSurface(const GameSurface& game, Color color);
	~GemSurface();

	virtual Status Update(const SDL_Event& event) override;
	virtual Status Update(const std::chrono::time_point<std::chrono::system_clock>& time) override;
	virtual void Render(SDL_Surface& surface) override;

	const Position& GetPosition() const;
	void SetPosition(const Position& position);
	void SetPosition(std::size_t x, std::size_t y);

	bool Contains(const Position& position) const;

	bool IsHover() const;
	void SetHover(bool value);

	bool IsSelected() const;
	void SetSelected(bool value);

	Color GetColor() const;
	void SetColor(Color color);

	GemSurface& operator=(const GemSurface& other);

private:
	Color m_color = Color::EMPTY;
	static const std::map<Color, std::string> s_gemFileNames;
};
