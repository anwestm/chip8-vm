//
// Created by andre on 2022-01-30.
//

#include "input.h"
#include "SDL2/SDL.h"

extern int g_vm_running;

void handle_input() {
    SDL_Event event;
    while (SDL_PollEvent (&event) != 0)
    {
        switch (event.type)
        {
            case SDL_KEYDOWN: if (event.key.keysym.sym == SDLK_ESCAPE)
                    g_vm_running = 0;
                break;
        }
    }
}
