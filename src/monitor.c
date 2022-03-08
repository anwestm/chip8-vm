//
// Created by andre on 2022-01-29.
//

#include "monitor.h"

#define WINDOW_WIDTH = 1024
#define WINDOW_HEIGHT = 512


SDL_Window *window;
SDL_Renderer *renderer;

extern unsigned char *disp;

void monitor_setup()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
            "SDL2Test",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1024,
            512,
            0
    );

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

}

void monitor_update() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    int scale = 16;
    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 32; y++) {
            if (disp[x + y * 32]) {
                for (int i = 0; i < scale; i++) {
                    for (int j = 0; j < scale; j++) {
                        SDL_RenderDrawPoint(renderer, x*scale+i, y*scale+j);
                    }
                }

            }

        }
    }
    SDL_RenderPresent(renderer);

}

void monitor_destroy()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}


