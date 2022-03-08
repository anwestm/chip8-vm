//
// Created by andre on 2022-01-29.
//

#include "chip8.h"

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
unsigned short opcode;

/* 4K memory */
/* 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
   0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
   0x200-0xFFF - Program ROM and work RAM */
unsigned char memory[4096];

/* 15 8-bit registers. 16th is carry */
unsigned char *V;

/* Index and pc */
unsigned short I;
unsigned short pc;

/* Graphics */
unsigned char *disp;

/* 60 Hz timer registers */
unsigned char delay_timer;
unsigned char sound_timer;

/* Stack and pointer */
unsigned char *stack;
unsigned short sp;

/* keypad */
unsigned char key[16];

void chip8_initialize()
{
    //pc = 0x01FC;
    opcode = 0;
    I = 0;
    //sp = MEMORY_STACK;

    stack = &memory[MEMORY_STACK];
    V = &memory[MEMORY_VAR];
    disp = &memory[MEMORY_DISPLAY];

    sp = MEMORY_STACK;
    pc = 0x01FC;

    for (int i = 0; i < 80; i++) {
        memory[i] = chip8_fontset[i];
    }

    /* Permanent instructions executed before program. */
    memory[0x01FC] = 0x00E0;
    memory[0x01FE] = 0x004B;

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
        memory[c++] = fgetc(fp);
        if (c > MEMORY_STACK) {
            printf("Error: ROM TO LARGE");
            break;
        }
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
    pc += 2;

}
static void chip8_decode_opcode() {
    unsigned short X = (opcode & 0x0F00) >> 8;
    unsigned short Y = (opcode & 0x00F0) >> 4;
    switch ((opcode >> 12)) {
        case 0x0:
            switch (opcode) {
                case 0x00E0:
                    // Clear screen
                    for (int i = 0; i < 64*32; i++) {
                        disp[i] = 0;
                    }
                    pc += 2;
                    break;
                case 0x00EE:
                    // Return from subroutine
                    pc = stack[sp];
                    sp--;
                    break;
                default:
                    // TODO: Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN. Not necessary for most ROMs.
                    break;
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
            sp++;
            pc = opcode & 0x0FFF;
            break;
        case 0x3:
            /* Only one opcode 3XNN */
            // Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block);
            pc += 2*(V[X] == (opcode & 0x00FF));
            break;
        case 0x4:
            /* Only one opcode 4XNN */
            // Skips the next instruction if VX does not equal NN. (Usually the next instruction is a jump to skip a code block);
            pc += 2*(V[X] != (opcode & 0x00FF));
            break;
        case 0x5:
            /* Only one opcode 5XY0 */
            // Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block);
            pc += 2 * (V[X] == V[Y]);
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
                    V[X] ^= V[Y];
                    break;
                case 0x0004:
                    V[0xF] = (V[X] & 0x80) == (V[Y] & 0x80); // set carry
                    V[X] += V[Y];
                    break;
            }
            pc += 2;
            /*
             * TODO: IMPLEMENT FOLLOW INSTRUCTIONS
             *
             *
             *
             * 8XY3[a]	BitOp	Vx ^= Vy	    Sets VX to VX xor VY.
             * 8XY4	    Math	Vx += Vy	    Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not.
             * 8XY5	    Math	Vx -= Vy	    VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not.
             * 8XY6[a]	BitOp	Vx >>= 1	    Stores the least significant bit of VX in VF and then shifts VX to the right by 1.[b]
             * 8XY7[a]	Math	Vx = Vy - Vx	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not.
             * 8XYE[a]	BitOp	Vx <<= 1	    Stores the most significant bit of VX in VF and then shifts VX to the left by 1.[b]-
             */
        case 0x9:
            /* Only one opcode 9XY0 */
            // TODO: Skips the next instruction if VX does not equal VY. (Usually the next instruction is a jump to skip a code block);
            break;
        case 0xA:
            /* Only one opcode ANNN */
            // TODO: Sets I to the address NNN.
            break;
        case 0xB:
            /* Only one opcode BNNN */
            // TODO: Jumps to the address NNN plus V0..
            break;
        case 0xC:
            /* Only one opcode CXNN */
            // TODO: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
            break;
        case 0x0D:
            /* Only one opcode DXYN
                TODO: raws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
                        Each row of 8 pixels is read as bit-coded starting from memory location I;
                        I value does not change after the execution of this instruction. As described above,
                        VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen.
                        */
            V[0xF] = 0;
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
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
                        disp[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }
            pc += 2;
            break;
        case 0xE:
            break;
        case 0xF:
            break;

        default:
            printf("Unknown opcode %d", opcode);

    }
}

static void chip8_execute_opcode() {}

static void chip8_update_timers() {}

