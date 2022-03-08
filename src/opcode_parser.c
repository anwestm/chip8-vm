/*//
// Created by andre on 2022-01-29.
//

#include "opcode_parser.h"
#include "chip8.h"

extern short opcode;

void opNULL(){}

void opCode(){

    opTable[(opcode & 0xF000)>>12]();
}

void op0Code() {
    switch (opcode) {
        case 0x00E0:
            // TODO: Clear screen
            break;
        case 0x00EE:
            //TODO: Return from subroutine
            break;
        default:
            // TODO: Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN. Not necessary for most ROMs.
            break;
    }
}*/