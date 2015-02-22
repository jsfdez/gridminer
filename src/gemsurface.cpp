#include "gemsurface.h"

#include <SDL.h>
#include <cstdint>

#include "files.h"
#include "gamesurface.h"
#include "loader.h"

decltype(GemSurface::s_gemFileNames) GemSurface::s_gemFileNames
{ 
	{ GemColor::BLUE, k_imageGemBlue },
	{ GemColor::GREEN, k_imageGemGreen },
	{ GemColor::PURPLE, k_imageGemPurple },
	{ GemColor::RED, k_imageGemRed },
	{ GemColor::YELLOW, k_imageGemYellow },
};


GemSurface::GemSurface(const GameSurface& game)
	: m_game(game)
{
}

GemSurface::GemSurface(const GameSurface& game, GemColor color)
	: m_game(game)
	, m_color(color)
{
	auto it = s_gemFileNames.find(color);
	SDL_assert(it != s_gemFileNames.end());
	m_image = loader::Image(it->second);
}

GemSurface::~GemSurface()
{
}

GemSurface::Status GemSurface::Update(const SDL_Event&)
{
	if (m_offset.X || m_offset.Y 
		|| (m_destruction != Destruction::ALIVE && m_alpha))
	{
		return Status::ANIMATION;
	}
	return Status::CONTINUE;
}

AbstractSurface::Status GemSurface::Update(
	const std::chrono::time_point<std::chrono::system_clock>&)
{
	if (m_offset.X != 0)
		m_offset.X += m_offset.X > 0 ? -2 : 2;
	else if (m_offset.Y != 0)
		m_offset.Y += m_offset.Y > 0 ? -2 : 2;
	else if (m_destruction != Destruction::ALIVE && m_alpha) m_alpha -= 1;
	else return Status::CONTINUE;
	return Status::ANIMATION;
}

void GemSurface::Render(SDL_Surface& surface)
{
	std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> copy(nullptr,
		SDL_FreeSurface);
	auto position = GetPosition() + m_offset;
	SDL_Rect rect
	{
		position.X - WIDTH / 2,
		position.Y - HEIGHT / 2,
		WIDTH, 
		HEIGHT,
	};
	SDL_Rect sourceRect{ 0, 0, WIDTH, HEIGHT };
	if (m_destruction != Destruction::ALIVE)
	{
		auto copy = CloneSurface(true);
		const auto size = copy->w * copy->h;
		const auto& format = *copy->format;
		auto pixels = reinterpret_cast<std::uint32_t*>(copy->pixels);
		for (std::remove_const<decltype(size)>::type i = 0; i < size; ++i)
		{
			auto alpha = (pixels[i] & format.Amask) >> format.Ashift;
			alpha = alpha - m_alpha > 0 ? alpha - m_alpha : 0;
			auto rgbMask = format.Rshift + format.Gshift + format.Bshift;
			pixels[i] = pixels[i] & rgbMask + (alpha << format.Amask);
		}
	}
	else if (IsHover() || IsSelected())
	{
		auto copy = CloneSurface(true);
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
	auto result = SDL_BlitSurface(!copy ? m_image.get() : copy.get(), 
		&sourceRect, &surface, &rect);
	if (result)
	{
		SDL_Log("Error in SDL_BlitSurface: %s\n", SDL_GetError());
	}
}

const Position GemSurface::GetPosition() const
{
	auto index = m_game.GetGemIndex(*this);
	return m_game.CalculateGemPosition(index);
}

bool GemSurface::Contains(const Position& position) const
{
	auto currentPosition = GetPosition() + m_offset;
	return currentPosition.X - WIDTH / 2 < position.X
		&& currentPosition.X + WIDTH / 2 > position.X
		&& currentPosition.Y - HEIGHT / 2 < position.Y
		&& currentPosition.Y + HEIGHT / 2 > position.Y;
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
GemSurface::CloneSurface(bool copy)
{
	decltype(CloneSurface(copy)) ret(nullptr, SDL_FreeSurface);
	ret.reset(SDL_CreateRGBSurface(m_image->flags,
		WIDTH,
		HEIGHT,
		m_image->format->BitsPerPixel,
		m_image->format->Rmask,
		m_image->format->Gmask,
		m_image->format->Bmask,
		m_image->format->Amask)
	);
	if (copy)
	{
		SDL_BlitSurface(m_image.get(), nullptr, ret.get(), &ret->clip_rect);
	}
	return ret;
}

GemColor GemSurface::GetColor() const
{
	return m_color;
}

void GemSurface::SetColor(GemColor color)
{
	auto it = s_gemFileNames.find(color);
	m_color = color;
	if (it == s_gemFileNames.end())
		m_image.reset();
	else
		m_image = loader::GemSurface(color);
}

GemSurface& GemSurface::operator=(const GemSurface& other)
{
	SetColor(other.GetColor());
	return *this;
}

void GemSurface::StartSwapping(GemSurface& first, GemSurface& second)
{
	std::swap(first, second);
	first.m_offset = second.GetPosition() - first.GetPosition();
	second.m_offset = first.GetPosition() - second.GetPosition();
	SDL_assert(first.m_offset.X || first.m_offset.Y);
	SDL_assert(second.m_offset.X || second.m_offset.Y);
}

bool GemSurface::IsDestroyed() const
{
	return m_destruction != Destruction::ALIVE;
}

void GemSurface::Destroy(Destruction destruction)
{
	SDL_assert(destruction != Destruction::ALIVE);
	m_destruction = destruction;
}
