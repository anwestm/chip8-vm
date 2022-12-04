#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "monitor.h"
#include "input.h"
#include "chip8.h"
#include "SDL2/SDL.h"

#define CLOCK_FREQUENCY 500

int g_vm_running = 1;

int main(int argc, char *argv[]) {
    printf("Hello, World!\n");

    monitor_setup();

    chip8_load_game("D:/dev/c-dev/chip8-vm/rom/pong.rom");
    chip8_initialize();

    while (g_vm_running == 1) {
        Uint32 ticks = SDL_GetTicks();
        handle_input();

        chip8_emulate_cycle();
        monitor_update();

        if (SDL_GetTicks() - ticks < 1000 / CLOCK_FREQUENCY)
            SDL_Delay (1000 / CLOCK_FREQUENCY - (SDL_GetTicks () - ticks));
    }


    return 0;
}
