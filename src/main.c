#include <stdio.h>
#include <stdlib.h>

#include "montor.h"
#include "chip8.h"

int main(int argc, char *argv[]) {
    printf("Hello, World!\n");

    monitor_setup();

    chip8_initialize();
    chip8_emulate_cycle();

    return 0;
}
