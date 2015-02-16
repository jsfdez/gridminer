#include <SDL.h>
#include <SDL_image.h>
#include <memory>
#include <stdio.h>
#include <cstdlib>

#include "imageloader.h"

int main(int, char**)
{
    SDL_Init(SDL_INIT_VIDEO);

    std::unique_ptr<SDL_Window, decltype(SDL_DestroyWindow)*> window(nullptr,
        SDL_DestroyWindow);
    auto background = ImageLoader::Load("data/BackGround.jpg");

    if (background)
    {
        window.reset(SDL_CreateWindow("Grid Miner", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, background->w, background->h,
            SDL_WINDOW_OPENGL));
    }

    if (!window) {
        printf("Could not create window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    auto surface = SDL_GetWindowSurface(window.get());
//    auto background = IMG_Load("data/BackGround.jpg");
	SDL_assert(background);
	SDL_Event event;
    while(true)
	{
		if (SDL_PollEvent(&event))
		{
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    break;
                }
            }
		}
        SDL_BlitSurface(background.get(), nullptr, surface, nullptr);
        SDL_UpdateWindowSurface(window.get());
    }
//    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
