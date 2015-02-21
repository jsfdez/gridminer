#include "gemsurface.h"

#include <SDL.h>
#include <cstdint>

#include "files.h"
#include "imageloader.h"

decltype(GemSurface::s_gemFileNames) GemSurface::s_gemFileNames
{ 
	{ GemSurface::Color::BLUE, IMAGE_GEM_BLUE },
	{ GemSurface::Color::GREEN, IMAGE_GEM_GREEN },
	{ GemSurface::Color::PURPLE, IMAGE_GEM_PURPLE },
	{ GemSurface::Color::RED, IMAGE_GEM_RED },
	{ GemSurface::Color::YELLOW, IMAGE_GEM_YELLOW },
};


GemSurface::GemSurface(const GameSurface& game)
	: m_game(game)
{

}

GemSurface::GemSurface(const GameSurface& game, Color color)
	: m_game(game)
	, m_color(color)
{
	auto it = s_gemFileNames.find(color);
	SDL_assert(it != s_gemFileNames.end());
	m_image = ImageLoader::Load(it->second);
}

GemSurface::~GemSurface()
{
}

GemSurface::Status GemSurface::Update(const SDL_Event&)
{
	SDL_assert(false);
	return Status::EXIT;
}

AbstractSurface::Status GemSurface::Update(const std::chrono::time_point<std::chrono::system_clock>&)
{
	return AbstractSurface::Status::CONTINUE;
}

void GemSurface::Render(SDL_Surface& surface)
{
	SDL_Rect rect
	{
		m_position.X - WIDTH / 2,
		m_position.Y - HEIGHT / 2,
		WIDTH, 
		HEIGHT,
	};

	SDL_Log("%d, %d", m_position.X, m_position.Y);

	std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> copy(nullptr, 
		SDL_FreeSurface);

	if (IsHover() || IsSelected())
	{
		copy = CloneSurface();
		const auto size = copy->w * copy->h;
		const auto& format = *copy->format;
		auto pixels = reinterpret_cast<std::uint32_t*>(copy->pixels);
		for (std::remove_const<decltype(size)>::type i = 0; i < size; ++i)
		{
			if ((pixels[i] & format.Amask) == format.Amask && IsSelected())
			{
				pixels[i] &= format.Rmask | format.Gmask | format.Bmask;
				pixels[i] |= 0xFF << format.Ashift;
			}
			else if (pixels[i] & format.Amask)
			{
				auto r = (pixels[i] & format.Rmask) >> format.Rshift;
				auto g = (pixels[i] & format.Gmask) >> format.Gshift;
				auto b = (pixels[i] & format.Bmask) >> format.Bshift;
				auto a = (pixels[i] & format.Amask) >> format.Ashift;
				r = (r + 0x80 > 0xFF) ? 0xff : r + 0x80;
				g = (g + 0x80 > 0xFF) ? 0xff : g + 0x80;
				b = (b + 0x80 > 0xFF) ? 0xff : b + 0x80;
				pixels[i] = (r << format.Rshift)
					+ (g << format.Gshift)
					+ (b << format.Bshift)
					+ (a << format.Ashift);
			}
		}
	}
	SDL_BlitSurface(!copy ? m_image.get() : copy.get(), nullptr, &surface, 
		&rect);
}

const Position& GemSurface::GetPosition() const
{
	return m_position;
}

void GemSurface::SetPosition(const Position& position)
{
	m_position = position;
}

void GemSurface::SetPosition(std::size_t x, std::size_t y)
{
	m_position.X = x;
	m_position.Y = y;
}

bool GemSurface::Contains(const Position& position) const
{
	return m_position.X - WIDTH / 2 < position.X
		&& m_position.X + WIDTH / 2 > position.X
		&& m_position.Y - HEIGHT / 2 < position.Y
		&& m_position.Y + HEIGHT / 2 > position.Y;
}

bool GemSurface::IsHover() const
{
	return m_hover;
}

void GemSurface::SetHover(bool value)
{
	m_hover = value;
}

bool GemSurface::IsSelected() const
{
	return m_selected;
}

void GemSurface::SetSelected(bool value)
{
	m_selected = value;
}

std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> 
GemSurface::CloneSurface()
{
	decltype(CloneSurface()) ret(nullptr, SDL_FreeSurface);
	ret.reset(SDL_CreateRGBSurface(m_image->flags, 
		m_image->w, 
		m_image->h, 
		m_image->format->BitsPerPixel, 
		m_image->format->Rmask, 
		m_image->format->Gmask, 
		m_image->format->Bmask, 
		m_image->format->Amask)
	);
	SDL_BlitSurface(m_image.get(), nullptr, ret.get(), &ret->clip_rect);
	return ret;
}

GemSurface::Color GemSurface::GetColor() const
{
	return m_color;
}

void GemSurface::SetColor(Color color)
{
	auto it = s_gemFileNames.find(color);
	m_color = color;
	if (it == s_gemFileNames.end())
		m_image.reset();
	else
		m_image = ImageLoader::Load(it->second);
}
