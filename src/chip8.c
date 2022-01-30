//
// Created by andre on 2022-01-29.
//

#include "chip8.h"

/* 35 opcodes */
unsigned short opcode;

/* 4K memory */
/* 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
   0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
   0x200-0xFFF - Program ROM and work RAM */
unsigned char memory[4096];

/* 15 8-bit registers. 16th is carry */
unsigned char V[16];

/* Index and pc */
unsigned short I;
unsigned short pc;

/* Graphics */
unsigned char *gfx;

/* 60 Hz timer registers */
unsigned char delay_timer;
unsigned char sound_timer;

/* Stack and pointer */
unsigned short stack[16];
unsigned short sp;

/* keypad */
unsigned char key[16];

void chip8_initialize()
{
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0xfa0;

    gfx = &memory[0xF00];

    for (int i = 0; i < 80; i++) {
        memory[i] = chip8_fontset[i];
    }
}
void chip8_load_game(char *str)
{

}
void chip8_emulate_cycle()
{
    chip8_fetch_opcode();
    chip8_decode_opcode();
}

static void chip8_fetch_opcode()
{
    /* One opcode is two bytes long and therefore needs to be conc. (8 bit memory)*/
    opcode = memory[pc] << 8 | memory[pc+1];
    pc += 2;

}
static void chip8_decode_opcode() {
    switch ((opcode >> 12)) {
        case 0x0:
            switch (opcode) {
                case 0x00E0:
                    // Clear screen
                    for (int i = 0; i < 64*32; i++) {
                        gfx[i] = 0;
                    }
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
            pc += V[(opcode & 0x0F00) >> 2] == (opcode & 0x00FF);
            break;
        case 0x4:
            /* Only one opcode 4XNN */
            // Skips the next instruction if VX does not equal NN. (Usually the next instruction is a jump to skip a code block);
            pc += V[(opcode & 0x0F00) >> 2] != (opcode & 0x00FF);
            break;
        case 0x5:
            /* Only one opcode 5XY0 */
            // Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block);
            pc += V[(opcode & 0x0F00) >> 2] == V[(opcode & 0x00F0) >> 1];
            break;
        case 0x6:
            /* Only one opcode 6XNN */
            // Sets VX to NN.
            V[opcode & 0x0F00] = opcode & 0x00FF;
            break;
        case 0x7:
            /* Only one opcode 7XNN */
            // Adds NN to VX. (Carry flag is not changed);.
            V[opcode & 0x0F00] += opcode & 0x00FF;
            break;
        case 0x8:
            switch (opcode & 0x000F) {
                case 0x0000:
                    V[opcode & 0x0]

            }
            /*
             * TODO: IMPLEMENT FOLLOW INSTRUCTIONS
             * 8XY0	    Assig	Vx = Vy	        Sets VX to the value of VY.
             * 8XY1 	BitOp	Vx |= Vy	    Sets VX to VX or VY. (Bitwise OR operation);
             * 8XY2	    BitOp	Vx &= Vy	    Sets VX to VX and VY. (Bitwise AND operation);
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
        case 0xD:
            /* Only one opcode DXYN	*/
            // TODO: raws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen.
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

