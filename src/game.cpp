#include "game.h"

#include <cstdint>

#include "SDL.h"
#include "files.h"
#include "imageloader.h"

Game::Game()
{
	Create();
}

Game::~Game()
{
	Destroy();
}

Game::Status Game::Update(const SDL_Event& event, SDL_Surface& surface)
{
	const auto offsetX = 330u;
	const auto offsetY = 100u;
	const auto w = 42u;
	const auto h = 42u;
	const auto columns = static_cast<std::size_t>(std::sqrt(m_gridData.size()));
	auto status = Status::CONTINUE;
    SDL_BlitSurface(m_background.get(), nullptr, &surface, nullptr);
	for (decltype(m_gridData.size()) i = 0; i < m_gridData.size(); ++i)
	{
		SDL_Rect rect{ offsetX + w * (i % columns), 
			offsetY + h * (i / columns), w, h };
		auto image = m_gems[m_gridData[i]];
		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)*> copy(
				nullptr, SDL_FreeSurface);
			copy.reset(SDL_CreateRGBSurface(0, image->w, image->h, 
				image->format->BitsPerPixel, 
				image->format->Rmask, 
				image->format->Gmask, 
				image->format->Bmask, 
				image->format->Amask));
			SDL_BlitSurface(image.get(), nullptr, copy.get(), nullptr);
			if (copy)
			{
				auto size = copy->w * copy->h;
				for (decltype(size) i = 0; i < size; i += 4)
				{
					auto pixel = static_cast<std::uint32_t*>(copy->pixels)[i];
					auto rgba = reinterpret_cast<std::uint8_t*>(&pixel);
					//rgba[0] = 0xFF;
				}
			}
			SDL_BlitSurface(copy.get(), nullptr, &surface, &rect);
		}
		else
		{
			SDL_BlitSurface(image.get(), nullptr, &surface, &rect);
		}
	}
    return status;
}

void Game::Create()
{
	m_background = ImageLoader::Load(IMAGE_BACKGROUND);
	m_gems[Gem::BLUE] = ImageLoader::Load(IMAGE_GEM_BLUE);
	m_gems[Gem::GREEN] = ImageLoader::Load(IMAGE_GEM_GREEN);
	m_gems[Gem::PURPLE] = ImageLoader::Load(IMAGE_GEM_PURPLE);
	m_gems[Gem::RED] = ImageLoader::Load(IMAGE_GEM_RED);
	m_gems[Gem::YELLOW] = ImageLoader::Load(IMAGE_GEM_YELLOW);

	for (auto& gem : m_gridData)
	{
		auto pos = std::rand() % m_gems.size();
		auto it = m_gems.begin();
		std::advance(it, pos);
		gem = it->first;
	}
}

void Game::Destroy()
{
	m_background.reset();
	m_gems.clear();
}
