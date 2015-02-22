#include "candlewicksurface.h"

#include <limits>

#include "files.h"
#include "loader.h"

CandlewickSurface::CandlewickSurface(const GameSurface& game)
	: m_game(game)
	, m_destRect({ 178, 372, 263 - 178, 556 - 372 })
{
	CreatePath();
}

CandlewickSurface::~CandlewickSurface()
{
}

CandlewickSurface::Status CandlewickSurface::Update(const SDL_Event&)
{
	return Status::CONTINUE;
}

CandlewickSurface::Status CandlewickSurface::Update(
	const std::chrono::time_point<std::chrono::system_clock>&)
{
	m_frame = (m_frame + 1) % m_path.size();
	return Status::CONTINUE;
}

void CandlewickSurface::Render(SDL_Surface& surface)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	auto Rmask = 0xff000000;
	auto Gmask = 0x00ff0000;
	auto Bmask = 0x0000ff00;
	auto Amask = 0x000000ff;
#else
	auto Rmask = 0x000000ff;
	auto Gmask = 0x0000ff00;
	auto Bmask = 0x00ff0000;
	auto Amask = 0xff000000;
#endif
	std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> frame(
		SDL_CreateRGBSurface(SDL_SWSURFACE, WIDTH, HEIGHT, 32, 
		Rmask, Gmask, Bmask, Amask), SDL_FreeSurface);
	SDL_assert(frame);
	const auto Rshift = frame->format->Rshift;
	const auto Gshift = frame->format->Gshift;

	const std::vector<std::uint32_t> colors
	{
		static_cast<std::uint32_t>(0xFF << Rshift | Amask),
		static_cast<std::uint32_t>(0xFF << Rshift | 0xF0 << Gshift | Amask),
		static_cast<std::uint32_t>(0xFF << Rshift | 0xFF << Gshift | Amask),
	};
	auto pixels = reinterpret_cast<std::uint32_t*>(frame->pixels);
	for (auto i = 0u; i < WIDTH * HEIGHT; ++i)
	{
		if (std::rand() % 3 == 0) 
			pixels[i] = colors[std::rand() % colors.size()];
		else pixels[i] = 0xCCCCCCFF;
	}
	m_destRect.x = m_path[m_frame].X - (WIDTH / 2);
	m_destRect.y = m_path[m_frame].Y - (HEIGHT / 2);
	m_destRect.w = WIDTH;
	m_destRect.h = HEIGHT;
	auto result = SDL_BlitSurface(frame.get(), nullptr, &surface, &m_destRect);
	if (result)
		SDL_Log(SDL_GetError());
}

bool CandlewickSurface::Contains(const Position&) const
{
	return false;
}

void CandlewickSurface::CreatePath()
{
	const std::vector<std::pair<Position, Position>> keyframes
	{
		{ { 264, 555 }, { 253, 546 } },
		{ { 253, 546 }, { 254, 517 } },
		{ { 254, 517 }, { 245, 512 } },
		{ { 245, 512 }, { 215, 507 } },
		{ { 215, 507 }, { 206, 373 } },
		{ { 206, 373 }, { 177, 373 } },
	};
	auto interpolate = [&](const Position& l, const Position& r)
	{
		if (SDL_abs(l.X - r.X) > SDL_abs(l.Y - r.Y))
		{
			for (auto x = l.X; x > r.X; --x)
			{
				auto y = l.Y + (r.Y - l.Y) * (x - l.X) / (r.X - l.X);
				m_path.emplace_back(x, y);
			}
		}
		else
		{
			for (auto y = l.Y; y > r.Y; --y)
			{
				auto x = l.X + (r.X - l.X) * (y - l.Y) / (r.Y - l.Y);
				m_path.emplace_back(x, y);
			}
		}
	};
	for (auto& keyframe : keyframes)
	{
		interpolate(keyframe.first, keyframe.second);
	}
}
