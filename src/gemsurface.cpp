#include "gemsurface.h"

#include <SDL.h>
#include <cstdint>

#include "files.h"
#include "gamesurface.h"
#include "imageloader.h"

decltype(GemSurface::s_gemFileNames) GemSurface::s_gemFileNames
{ 
	{ GemSurface::Color::BLUE, k_imageGemBlue },
	{ GemSurface::Color::GREEN, k_imageGemGreen },
	{ GemSurface::Color::PURPLE, k_imageGemPurple },
	{ GemSurface::Color::RED, k_imageGemRed },
	{ GemSurface::Color::YELLOW, k_imageGemYellow },
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
	if (m_offset.X || m_offset.Y)
		return Status::ANIMATION;
	return Status::CONTINUE;
}

void GemSurface::Animate(
	const std::chrono::time_point<std::chrono::system_clock>&)
{
	if (m_offset.X != 0)
		m_offset.X += m_offset.X > 0 ? -2 : 2;
	else if (m_offset.Y != 0)
		m_offset.Y += m_offset.Y > 0 ? -2 : 2;
}

void GemSurface::Render(SDL_Surface& surface)
{
	auto position = GetPosition() + m_offset;
	SDL_Rect rect
	{
		position.X - WIDTH / 2,
		position.Y - HEIGHT / 2,
		WIDTH, 
		HEIGHT,
	};

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
