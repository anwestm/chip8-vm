#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "monitor.h"
#include "input.h"
#include "chip8.h"
#include "SDL2/SDL.h"

#define CLOCK_FREQUENCY 500

int g_vm_running = 1;

struct data {
    int first:2;
    int second:2;

} Age;

int main(int argc, char *argv[]) {
    printf("Hello, World!\n");

    struct data a;
    Age.first = 1;

    printf("%d\n", Age);


    monitor_setup();

    chip8_load_game("D:/dev/c-dev/chip8-vm/rom/test_opcode.ch8");
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
