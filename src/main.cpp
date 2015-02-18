#include <stack>
#include <memory>
#include <stdio.h>
#include <cstdlib>

#include "SDL.h"
#include "game.h"
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
			window.reset(SDL_CreateWindow("Grid Miner", SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED, background->w, background->h,
				SDL_WINDOW_OPENGL));
		}
	}

    if (!window) 
	{
        printf("Could not create window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    auto& surface = *SDL_GetWindowSurface(window.get());
	struct _Scenes : std::stack<std::unique_ptr<AbstractScene>>
	{
		AbstractScene* operator->()
		{
			return top().get();
		}
	} scenes;
	SDL_Event event;
	scenes.emplace(new Game);
    while(true)
	{
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
    }
    SDL_Quit();
    return EXIT_SUCCESS;
}
