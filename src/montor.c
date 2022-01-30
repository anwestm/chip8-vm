//
// Created by andre on 2022-01-29.
//

#include "montor.h"

#define WINDOW_WIDTH = 800
#define WINDOW_HEIGHT = 800


SDL_Window *window;

void monitor_setup()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
            "SDL2Test",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            800,
            800,
            0
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

}

void monitor_destroy()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}


