/*//
// Created by andre on 2022-01-29.
//

#ifndef CHIP8_VM_OPCODE_PARSER_H
#define CHIP8_VM_OPCODE_PARSER_H

void (*opTable[17])();
void (*op0CodeTable[16])();

void opNULL();
void opCode();
void op0Code();

void (*opTable[17]) = {
        op0Code, opNULL, opNULL, opNULL,
        opNULL, opNULL, opNULL, opNULL,
        opNULL, opNULL, opNULL, opNULL,
        opNULL, opNULL, opNULL, opNULL, opNULL
};

void (*op0CodeTable[16]) = {
        op0Code, opNULL, opNULL, opNULL,
        opNULL, opNULL, opNULL, opNULL,
        opNULL, opNULL, opNULL, opNULL,
        opNULL, opNULL, opNULL, opNULL
};



#endif //CHIP8_VM_OPCODE_PARSER_H
*/