//
// Created by andre on 2022-01-29.
//

#include "monitor.h"

#define WINDOW_WIDTH = 1024
#define WINDOW_HEIGHT = 512


SDL_Window *window;
SDL_Renderer *renderer;

extern uint8_t disp[64*32];

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
    SDL_RenderSetLogicalSize(renderer, 1024, 512);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);


}

void monitor_update() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    /*int scale = 8;
    for (uint8_t y = 0; y < 32; y++) {
        for (uint8_t x = 0; x < 64; x++) {
            uint8_t xb = 1;
            while (xb != 0x80) {
                    if (disp[x + y * 64] & xb) {
                        for (int i = 0; i < scale; i++) {
                            for (int j = 0; j < scale; j++) {
                                SDL_RenderDrawPoint(renderer, (x+xb) * scale + i, y * scale + j);
                            }
                        }

                }
                xb = xb << 1;
            }
        }
    }*/

    SDL_Texture* sdlTexture = SDL_CreateTexture(renderer,
                                                SDL_PIXELFORMAT_ARGB8888,
                                                SDL_TEXTUREACCESS_STREAMING,
                                                64, 32);
    // Temporary pixel buffer
    uint32_t pixels[2048];
    for (int i = 0; i < 2048; ++i) {
        uint8_t pixel = disp[i];
        pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
    }
    // Update SDL texture
    SDL_UpdateTexture(sdlTexture, NULL, pixels, 64 * sizeof(Uint32));
    // Clear screen and render
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_RenderPresent(renderer);

}

void monitor_destroy()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}


