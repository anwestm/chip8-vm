//
// Created by andre on 2022-01-29.
//

#include "chip8.h"
#include "stdlib.h"
#include "stdbool.h"
#include "stdint.h"

#define MEMORY_INTERPRETER  0x0000
#define MEMORY_PROGRAM      0x0200
#define MEMORY_STACK        0x0EA0
#define MEMORY_WORK_AREA    0x0ED0
#define MEMORY_VAR          0x0EF0
#define MEMORY_DISPLAY      0x0F00

unsigned char chip8_fontset[80] =
        {
                0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                0x20, 0x60, 0x20, 0x20, 0x70, // 1
                0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };


/* 35 opcodes */
uint16_t opcode;

/* 4K memory */
/* 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
   0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
   0x200-0xFFF - Program ROM and work RAM */
uint8_t memory[4096];

/* 15 8-bit registers. 16th is carry */
uint8_t *V;

/* Index and pc */
uint16_t I;
uint16_t pc;

/* Graphics */
uint8_t disp[64*32];

/* 60 Hz timer registers */
unsigned char delay_timer;
unsigned char sound_timer;

/* Stack and pointer */
uint16_t stack[16];
uint16_t sp;

/* keypad */
uint8_t key[16];

void chip8_initialize()
{
    opcode = 0;

    //stack = &memory[MEMORY_STACK];
    V = &memory[MEMORY_VAR];
    //disp = &memory[MEMORY_DISPLAY];

    I = 0;
    sp = 0;
    pc = 0x01FC;

    for (int i = 0; i < 80; i++) {
        memory[i] = chip8_fontset[i];
    }

    /* Permanent instructions executed before program. */

    // 0x00E0
    memory[0x01FC] = 0x00;
    memory[0x01FD] = 0xE0;

    // 0x004B
    memory[0x01FE] = 0x00;
    memory[0x01FF] = 0x4B;

    printf("Sizeof char: %d\n", sizeof(char));

}

void chip8_load_game(const char *path)
{
    FILE *fp;
    fp = fopen(path, "rb");
    if (!fp) {
        printf("Error: fopen(%s) error number %d \n",path);
    }
    unsigned short c = MEMORY_PROGRAM;
    while (!feof(fp)) {
        memory[c] = fgetc(fp);
        if (c > MEMORY_STACK) {
            printf("Error: ROM TO LARGE");
            break;
        }
        ++c;
    }
    fclose(fp);
}

void chip8_emulate_cycle()
{
    chip8_fetch_opcode();
    chip8_decode_opcode();
    printf("pc %d\n", pc);
}

static void chip8_fetch_opcode()
{
    /* One opcode is two bytes long and therefore needs to be conc. (8 bit memory)*/
    opcode = memory[pc] << 8 | memory[pc+1];
    printf("Fetched opcode: %X (%d), pc: %d\n", opcode, opcode, pc);

}
static void chip8_decode_opcode() {
    unsigned short X = (opcode & 0x0F00) >> 8;
    unsigned short Y = (opcode & 0x00F0) >> 4;
    unsigned short o = opcode;
    unsigned short o4 = opcode >> 12;
    if (pc == 702)
        printf("PC: %d\n", pc);


    switch ((opcode >> 12)) {
        case 0x0:
            switch (opcode) {
                case 0x00E0:
                    // Clear screen
                    for (int i = 0; i < (64*32) / 8; i++) {
                        disp[i] = 0;
                    }
                    pc += 2;
                    break;
                case 0x00EE:
                    // Return from subroutine
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;
                case 0x004B:
                    printf("Interpreter Command; Turn screen On. Opcode: %d\n", opcode);
                    pc += 2;
                    break;
                default:
                    // TODO: Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN. Not necessary for most ROMs.
                    printf("Error; Opcode: %d not implemented.", opcode);
                    exit(1);
            }
            break;
        case 0x1:
            /* Only one opcode 1NNN */
            // TODO: Jumps to address NNN.
            pc = opcode & 0x0FFF;
            break;
        case 0x2:
            /* Only one opcode 2NNN */
            // TODO: Calls subroutine at NNN.
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;
        case 0x3:
            /* Only one opcode 3XNN */
            // Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block);
            if (V[X] == (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;
        case 0x4:
            /* Only one opcode 4XNN */
            // Skips the next instruction if VX does not equal NN. (Usually the next instruction is a jump to skip a code block);
            if (V[X] != (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;
        case 0x5:
            /* Only one opcode 5XY0 */
            // Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block);
            if (V[X] == V[Y])
                pc += 4;
            else
                pc += 2;
            break;
        case 0x6:
            /* Only one opcode 6XNN */
            // Sets VX to NN.
            V[X] = opcode & 0x00FF;
            pc += 2;
            break;
        case 0x7:
            /* Only one opcode 7XNN */
            // Adds NN to VX. (Carry flag is not changed);.
            V[X] += opcode & 0x00FF;
            pc += 2;
            break;
        case 0x8:
            switch (opcode & 0x000F) {
                case 0x0000:
                    V[X] = V[Y];
                    break;
                case 0x0001:
                    V[X] |= V[Y];
                    break;
                case 0x0002:
                    V[X] &= V[Y];
                    break;
                case 0x0003:
                    /* 8XY3[a]	BitOp	Vx ^= Vy	    Sets VX to VX xor VY. */
                    V[X] ^= V[Y];
                    break;
                case 0x0004:
                    /* 8XY4	    Math	Vx += Vy	    Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not. */
                    V[X] += V[Y];
                    if (V[Y] > (0xFF - V[X]))
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    break;
                case 0x0005:
                    /* 8XY5	    Math	Vx -= Vy	    VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not. */
                    if (V[Y] > (V[X]))
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;
                    V[X] -= V[Y];
                    break;
                case 0x0006:
                    /* 8XY6[a]	BitOp	Vx >>= 1	    Stores the least significant bit of VX in VF and then shifts VX to the right by 1.[b] */
                    V[0xF] = V[X] & 0x1;
                    V[X] >>= 1;
                    break;
                case 0x0007:
                    /* 8XY7[a]	Math	Vx = Vy - Vx	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not. */
                    if(V[X] > V[Y])	// VY-VX
                        V[0xF] = 0; // there is a borrow
                    else
                        V[0xF] = 1;
                    V[X] = V[Y] - V[X];
                case 0x000E:
                    /* 8XYE[a]	BitOp	Vx <<= 1	    Stores the most significant bit of VX in VF and then shifts VX to the left by 1.[b]- */
                    V[0xF] = V[X] >> 7;
                    V[X] <<= 1;
                    break;
                default:
                    printf("\nUnknown op code: %.4X\n", opcode);
                    exit(1);
            }
            pc += 2;
            break;
        case 0x9:
            /* Only one opcode 9XY0 */
            // Skips the next instruction if VX does not equal VY. (Usually the next instruction is a jump to skip a code block);
            if (V[X] != V[Y])
                pc += 4;
            else
                pc += 2;
            break;
        case 0xA:
            /* Only one opcode ANNN */
            // Sets I to the address NNN.
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        case 0xB:
            /* Only one opcode BNNN */
            // Jumps to the address NNN plus V0.
            pc = (opcode & 0x0FFF) + V[0];
            break;
        case 0xC:
            /* Only one opcode CXNN */
            // Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
            V[X] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
            pc += 2;
            break;
        case 0xD:
            /* Only one opcode DXYN
                TODO: raws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
                        Each row of 8 pixels is read as bit-coded starting from memory location I;
                        I value does not change after the execution of this instruction. As described above,
                        VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen.
                        */
            V[0xF] = 0;
            unsigned short x = V[X];
            unsigned short y = V[Y];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            for (int yline = 0; yline < height; yline++)
            {
                pixel = memory[I + yline];
                for(int xline = 0; xline < 8; xline++)
                {
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        if(disp[(x + xline + ((y + yline) * 64))] == 1)
                            V[0xF] = 1;
                    }
                    disp[x + xline + ((y + yline) * 64)] ^= 1;
                }
            }
            pc += 2;
            break;
        case 0xE:
            switch (opcode & 0x00FF) {
                // EX9E - Skips the next instruction if the key stored
                // in VX is pressed.
                case 0x009E:
                    if (key[V[X]] != 0)
                        pc += 4;
                    else
                        pc += 2;
                    break;

                    // EXA1 - Skips the next instruction if the key stored
                    // in VX isn't pressed.
                case 0x00A1:
                    if (key[V[X]] == 0)
                        pc +=  4;
                    else
                        pc += 2;
                    break;

                default:
                    printf("\nUnknown op code: %d\n", opcode);
                    exit(1);
            }
            break;
        case 0xF:
            switch(opcode & 0x00FF)
            {
                // FX07 - Sets VX to the value of the delay timer
                case 0x0007:
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;
                    break;

                    // FX0A - A key press is awaited, and then stored in VX
                case 0x000A:
                {
                    bool key_pressed = false;

                    for(int i = 0; i < 16; ++i)
                    {
                        if(key[i] != 0)
                        {
                            V[(opcode & 0x0F00) >> 8] = i;
                            key_pressed = true;
                        }
                    }

                    // If no key is pressed, return and try again.
                    if(!key_pressed)
                        return;

                    pc += 2;
                }
                    break;

                    // FX15 - Sets the delay timer to VX
                case 0x0015:
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                    // FX18 - Sets the sound timer to VX
                case 0x0018:
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                    // FX1E - Adds VX to I
                case 0x001E:
                    // VF is set to 1 when range overflow (I+VX>0xFFF), and 0
                    // when there isn't.
                    if(I + V[(opcode & 0x0F00) >> 8] > 0xFFF)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                    // FX29 - Sets I to the location of the sprite for the
                    // character in VX. Characters 0-F (in hexadecimal) are
                    // represented by a 4x5 font
                case 0x0029:
                    I = V[(opcode & 0x0F00) >> 8] * 0x5;
                    pc += 2;
                    break;

                    // FX33 - Stores the Binary-coded decimal representation of VX
                    // at the addresses I, I plus 1, and I plus 2
                case 0x0033:
                    memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = V[(opcode & 0x0F00) >> 8] % 10;
                    pc += 2;
                    break;

                    // FX55 - Stores V0 to VX in memory starting at address I
                case 0x0055:
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                        memory[I + i] = V[i];

                    // On the original interpreter, when the
                    // operation is done, I = I + X + 1.
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                    break;

                case 0x0065:
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                        V[i] = memory[I + i];

                    // On the original interpreter,
                    // when the operation is done, I = I + X + 1.
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                    break;

                default:
                    printf ("Unknown opcode [0xF000]: 0x%X\n", opcode);
            }
            break;

        default:
            printf("Unknown opcode %d", opcode);
    }
    // Update timers
    if (delay_timer > 0)
        --delay_timer;

    if (sound_timer > 0)
        if(sound_timer == 1)
            ; // TODO: Implement sound
    --sound_timer;
}

static void chip8_execute_opcode() {}

static void chip8_update_timers() {}

