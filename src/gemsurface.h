#pragma once

#include "abstractsurface.h"

#include <map>
#include <SDL.h>
#include <memory>
#include <utility>

#include "size.h"
#include "position.h"

struct SDL_Surface;

class GemSurface : public AbstractSurface
{
	std::shared_ptr<SDL_Surface> m_image;
	Position m_position;
	bool m_hover = false;
	bool m_selected = false;

	std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> CloneSurface();

public:
	enum class Color
	{
		BLUE,
		GREEN,
		PURPLE,
		RED,
		YELLOW,
	};

	enum
	{
		WIDTH = 42u,
		HEIGHT = WIDTH,
		COLOR_COUNT = static_cast<std::size_t>(Color::YELLOW) + 1,
	};

	GemSurface(Color color);
	~GemSurface();

	virtual Status Update(const SDL_Event& event) override;
	virtual void Render(SDL_Surface& surface) override;

	const Position& GetPosition() const;
	void SetPosition(const Position& position);
	void SetPosition(std::size_t x, std::size_t y);

	bool Contains(const Position& position) const;

	bool IsHover() const;
	void SetHover(bool value);

	bool IsSelected() const;
	void SetSelected(bool value);

private:
	Color m_color;
	static const std::map<Color, std::string> s_gemFileNames;
};
