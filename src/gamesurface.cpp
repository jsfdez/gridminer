#include "gamesurface.h"

#include <cstdint>
#include <algorithm>
#include <functional>

#include "SDL.h"
#include "files.h"
#include "imageloader.h"

GameSurface::GameSurface()
{
	Create();
}

GameSurface::~GameSurface()
{
	Destroy();
}

GameSurface::Status GameSurface::Update(const SDL_Event& event)
{
	auto status = Status::CONTINUE;

	switch (event.type)
	{
	case SDL_MOUSEMOTION:
		OnMouseMoveEvent(reinterpret_cast<const SDL_MouseMotionEvent&>(event));
		break;
	case SDL_MOUSEBUTTONDOWN:
		OnMouseClickEvent(reinterpret_cast<const SDL_MouseButtonEvent&>(event));
		break;
	}
    return status;
}

void GameSurface::Render(SDL_Surface& surface)
{
	SDL_BlitSurface(m_background.get(), nullptr, &surface, nullptr);
	std::for_each(m_children.begin(), m_children.end(), 
		std::bind(
			&AbstractSurface::Render, std::placeholders::_1, 
			std::ref(surface)
		)
	);
	//for (decltype(m_gridData.size()) i = 0; i < m_gridData.size(); ++i)
	//{
	//	SDL_Rect rect{ offsetX + w * (i % columns),
	//		offsetY + h * (i / columns), w, h };
	//	auto image = m_gems[m_gridData[i]];
	//	//if (event.type == SDL_MOUSEBUTTONDOWN)
	//	{
	//		std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> copy(
	//			nullptr, SDL_FreeSurface);
	//		copy.reset(SDL_CreateRGBSurface(0, image->w, image->h,
	//			image->format->BitsPerPixel,
	//			image->format->Rmask,
	//			image->format->Gmask,
	//			image->format->Bmask,
	//			image->format->Amask));
	//		if (copy)
	//		{
	//			auto d = reinterpret_cast<std::uint32_t*>(copy->pixels);
	//			const auto s = reinterpret_cast<std::uint32_t*>(image->pixels);
	//			std::uint32_t size = copy->w * copy->h;
	//			for (auto i = 0u; i < size; ++i)
	//			{
	//				if (s[i] & copy->format->Amask)
	//				{
	//					auto r = (s[i] & copy->format->Rmask)
	//						>> copy->format->Rshift;
	//					auto g = (s[i] & copy->format->Gmask)
	//						>> copy->format->Gshift;
	//					auto b = (s[i] & copy->format->Bmask)
	//						>> copy->format->Bshift;
	//					auto a = (s[i] & copy->format->Amask)
	//						>> copy->format->Ashift;
	//					r = (r + 0x80 > 0xFF) ? 0xff : r + 0x80;
	//					g = (g + 0x80 > 0xFF) ? 0xff : g + 0x80;
	//					b = (b + 0x80 > 0xFF) ? 0xff : b + 0x80;
	//					d[i] = (r << copy->format->Rshift)
	//						+ (g << copy->format->Gshift)
	//						+ (b << copy->format->Bshift)
	//						+ (a << copy->format->Ashift);
	//				}
	//				else
	//				{
	//					d[i] = s[i];
	//				}
	//			}
	//		}
	//		SDL_BlitSurface(copy.get(), nullptr, &surface, &rect);
	//	}
	//}
}

void GameSurface::Create()
{
	//const auto offsetX = 330u + GemSurface::WIDTH / 2;
	//const auto offsetY = 100u + GemSurface::HEIGHT / 2;
	m_background = ImageLoader::Load(IMAGE_BACKGROUND);
	//m_gems[Gem::BLUE] = ImageLoader::Load(IMAGE_GEM_BLUE);
	//m_gems[Gem::GREEN] = ImageLoader::Load(IMAGE_GEM_GREEN);
	//m_gems[Gem::PURPLE] = ImageLoader::Load(IMAGE_GEM_PURPLE);
	//m_gems[Gem::RED] = ImageLoader::Load(IMAGE_GEM_RED);
	//m_gems[Gem::YELLOW] = ImageLoader::Load(IMAGE_GEM_YELLOW);

	for (std::size_t i = 0, size = 8 * 8; i < size; ++i)
	{
		Position position;
		position.X = OFFSET_X + GemSurface::WIDTH * (i % COLUMNS);
		position.Y = OFFSET_Y + GemSurface::HEIGHT * (i / COLUMNS);
		auto color = static_cast<GemSurface::Color>(
			std::rand() % GemSurface::COLOR_COUNT);
		auto gem = new GemSurface(color);
		m_children.emplace_back(gem);
		gem->SetPosition(position);
	}
	//for (auto& gem : m_gridData)
	//{
	//	auto pos = std::rand() % m_gems.size();
	//	auto it = m_gems.begin();
	//	std::advance(it, pos);
	//	gem = it->first;
	//}
}

void GameSurface::Destroy()
{
	m_background.reset();
	//m_gems.clear();
}

void GameSurface::OnMouseMoveEvent(const SDL_MouseMotionEvent& event)
{
	Position position;
	position.X = event.x;
	position.Y = event.y;
	for (auto& child : m_children)
	{
		if (child->Contains(position))
		{
			if (auto gem = dynamic_cast<GemSurface*>(child.get()))
				if (!gem->IsHover())
					gem->SetHover(true);
		}
		else if (auto gem = dynamic_cast<GemSurface*>(child.get()))
			if (gem->IsHover())
				gem->SetHover(false);
	}
}

void GameSurface::OnMouseClickEvent(const SDL_MouseButtonEvent& event)
{
	Position position;
	position.X = event.x;
	position.Y = event.y;
	for (auto& child : m_children)
	{
		if (child->Contains(position))
		{
			if (auto gem = dynamic_cast<GemSurface*>(child.get()))
				if (!gem->IsSelected())
					gem->SetSelected(true);
		}
		else if (auto gem = dynamic_cast<GemSurface*>(child.get()))
			if (gem->IsSelected())
				gem->SetSelected(false);
	}
}

bool GameSurface::Contains(const Position&) const
{
	return true;
}
