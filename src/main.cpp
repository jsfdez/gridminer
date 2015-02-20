#include <stack>
#include <memory>
#include <chrono>
#include <cstdint>
#include <stdio.h>
#include <cstdlib>

#include "SDL.h"
#include "gamesurface.h"
#include "files.h"
#include "imageloader.h"

int main(int, char**)
{
    SDL_Init(SDL_INIT_VIDEO);
    std::unique_ptr<SDL_Window, decltype(SDL_DestroyWindow)*> window(nullptr,
        SDL_DestroyWindow);
	{
        auto background = ImageLoader::Load(IMAGE_BACKGROUND);
		if (background)
		{
			window.reset(SDL_CreateWindow(
				"Grid Miner", 
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED, 
				background->w, 
				background->h,
				SDL_WINDOW_OPENGL)
			);
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
			else
				scenes->Update(event);
		}
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
