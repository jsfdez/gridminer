#include <stack>
#include <memory>
#include <chrono>
#include <cstdint>
#include <stdio.h>
#include <cstdlib>

#include "SDL.h"
#include "gamesurface.h"
#include "files.h"
#include "loader.h"

void TestGem(SDL_Window& window)
{
	SDL_Surface* surface = SDL_GetWindowSurface(&window);
	auto gem = loader::GemSurface(GemColor::RED);
	while (true)
	{
		auto background = loader::Image(k_imageBackground);
		SDL_BlitSurface(background.get(), nullptr, surface, nullptr);
		SDL_Rect dest{ 0, 0, gem->w, gem->h };
		SDL_Log("%d, %d\n", gem->w, gem->h);
		auto result = SDL_BlitSurface(gem.get(), nullptr, surface, &dest);
		if (result)
			SDL_Log("%s", SDL_GetError());
		SDL_UpdateWindowSurface(&window);
		SDL_Event event;
		SDL_PollEvent(&event);
		SDL_Delay(3000);
		SDL_Quit();
		exit(EXIT_SUCCESS);
	}
}

int main(int, char**)
{
    SDL_Init(SDL_INIT_VIDEO);
    std::unique_ptr<SDL_Window, decltype(SDL_DestroyWindow)*> window(nullptr,
        SDL_DestroyWindow);
	{
		auto background = loader::Image(k_imageBackground);
		if (background)
		{
			window.reset(SDL_CreateWindow(
				"Grid Miner by Jesus Fernandez (jsfdez@gmail.com). Press ESC to Quit", 
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED, 
				background->w, 
				background->h,
				SDL_WINDOW_OPENGL)
			);
		}
		else 
		{ 
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Grid Miner",
				"Cannot load data.", nullptr);
			return EXIT_FAILURE;
		}
	}

    if (!window) 
	{
        printf("Could not create window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    auto& surface = *SDL_GetWindowSurface(window.get());
	struct _Scenes : std::stack<std::unique_ptr<AbstractSurface>>
	{
		AbstractSurface* operator->()
		{
			return top().get();
		}
	} scenes;
	//TestGem(*window);
	std::srand(static_cast<unsigned int>(time(nullptr)));
	SDL_Event event;
	scenes.emplace(new GameSurface);
    while(true)
	{
		auto startTime = std::chrono::system_clock::now();
		while (SDL_PollEvent(&event))
		{
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
					SDL_Quit();
					return EXIT_SUCCESS;
                }
            }
			else scenes->Update(event);
		}
		scenes->Update(std::chrono::system_clock::now());
		scenes->Render(surface);
        SDL_UpdateWindowSurface(window.get());
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now() - startTime);
		const std::chrono::milliseconds minTime(1000 / 60);
		auto waitTime = minTime - elapsed;
		if (waitTime.count() > 0)
			SDL_Delay(static_cast<std::uint32_t>(waitTime.count()));
    }
    SDL_Quit();
    return EXIT_SUCCESS;
}
