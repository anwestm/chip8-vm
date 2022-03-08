//
// Created by andre on 2022-01-29.
//

#ifndef CHIP8_VM_CHIP8_H
#define CHIP8_VM_CHIP8_H

#include <stdlib.h>
#include <stdio.h>

void chip8_initialize();
void chip8_load_game(const char *path);
void chip8_emulate_cycle();

static void chip8_fetch_opcode();
static void chip8_decode_opcode();
static void chip8_execute_opcode();

static void chip8_update_timers();


#endif //CHIP8_VM_CHIP8_H
