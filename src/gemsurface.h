#pragma once

#include "abstractsurface.h"

#include <map>
#include <SDL.h>
#include <memory>
#include <cstdint>
#include <utility>

#include "size.h"
#include "position.h"
#include "gemcolor.h"

class GameSurface;
struct SDL_Surface;

class GemSurface : public AbstractSurface
{
	std::shared_ptr<SDL_Surface> m_image;
	Position m_offset = Position(0, 0);
	bool m_hover = false;
	bool m_selected = false;
	const GameSurface& m_game;
	std::uint8_t m_fallIncrement = 10;
	bool m_dragging = false;

	std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> CloneSurface(bool copy);

public:
	enum
	{
		WIDTH = 42u,
		HEIGHT = WIDTH,
		COLOR_COUNT = static_cast<std::size_t>(GemColor::YELLOW) + 1,
	};

	enum class Destruction
	{
		ALIVE,
		HORIZONTAL,
		VERTICAL,
	};

	GemSurface(const GameSurface& game);
	GemSurface(const GameSurface& game, GemColor color);
	GemSurface(const GameSurface& game, GemColor color, Position offset);
	GemSurface(const GameSurface& game, Position offset);
	~GemSurface();

	virtual Status Update(const SDL_Event& event) override;
	virtual Status Update(const std::chrono::time_point<std::chrono::system_clock>& time) override;
	virtual void Render(SDL_Surface& surface) override;

	const Position GetPosition() const;

	bool Contains(const Position& position) const;

	bool IsHover() const;
	void SetHover(bool value);

	bool IsEmpty() const;
	bool IsSelected() const;
	void SetSelected(bool value);
	void Destroy(Destruction destruction);
	bool IsDestroyed() const;

	GemColor GetColor() const;
	void SetColor(GemColor color);
	void SetOffset(const Position& position);
	void SetDragging(bool value);
	bool IsDragging() const;

	static void StartSwapping(GemSurface& first, GemSurface& second);

	GemSurface& operator=(const GemSurface& other);

private:
	GemColor m_color = GemColor::EMPTY;
	Destruction m_destruction = Destruction::ALIVE;
	//std::uint8_t m_size = 40;
	std::uint8_t m_alpha = 0xFF;
	std::size_t m_currentFrame = 0;
	static const std::map<GemColor, std::string> s_gemFileNames;
};
