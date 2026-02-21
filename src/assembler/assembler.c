#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"

const INSTRUCTION_LAYOUT INSTRUCTIONS[MAX_NUMBER_OF_INSTRUCTIONS] = {
    {.opcode = 0, .mnemonic = "stop", .format = {FORMAT_H}, .numOfOperands = 0},
    {.opcode = 1, .mnemonic = "add", .format = {FORMAT_R, FORMAT_I}, .numOfOperands = 3},
    {.opcode = 2, .mnemonic = "sub", .format = {FORMAT_R, FORMAT_I}, .numOfOperands = 3},
    {.opcode = 3, .mnemonic = "mul", .format = {FORMAT_R, FORMAT_I}, .numOfOperands = 3},
    {.opcode = 4, .mnemonic = "div", .format = {FORMAT_R, FORMAT_I}, .numOfOperands = 3},
    {.opcode = 5, .mnemonic = "and", .format = {FORMAT_R, FORMAT_I}, .numOfOperands = 3},
    {.opcode = 6, .mnemonic = "or", .format = {FORMAT_R, FORMAT_I}, .numOfOperands = 3},
    {.opcode = 7, .mnemonic = "xor", .format = {FORMAT_R, FORMAT_I}, .numOfOperands = 3},
    {.opcode = 8, .mnemonic = "shl", .format = {FORMAT_R, FORMAT_I}, .numOfOperands = 3},
    {.opcode = 9, .mnemonic = "shr", .format = {FORMAT_R, FORMAT_I}, .numOfOperands = 3},
    {.opcode = 10, .mnemonic = "slt", .format = {FORMAT_R, FORMAT_I}, .numOfOperands = 3},
    {.opcode = 11, .mnemonic = "sle", .format = {FORMAT_R, FORMAT_I}, .numOfOperands = 3},
    {.opcode = 12, .mnemonic = "seq", .format = {FORMAT_R, FORMAT_I}, .numOfOperands = 3},
    {.opcode = 13, .mnemonic = "load", .format = {FORMAT_R, FORMAT_I}, .numOfOperands = 3},
    {.opcode = 14, .mnemonic = "store", .format = {FORMAT_R, FORMAT_I}, .numOfOperands = 3},
    {.opcode = 15, .mnemonic = "jmp", .format = {FORMAT_JR, FORMAT_JI}, .numOfOperands = 2},
    {.opcode = 16, .mnemonic = "braz", .format = {FORMAT_B}, .numOfOperands = 2},
    {.opcode = 17, .mnemonic = "branz", .format = {FORMAT_B}, .numOfOperands = 2},
    {.opcode = 18, .mnemonic = "scall", .format = {FORMAT_S}, .numOfOperands = 1},
};

int validateCode(char mnemonic[10], int numOfOperands){
    int i = 0;
    for (i = 0; i < MAX_NUMBER_OF_INSTRUCTIONS; i++){
        
        if ( strcmp(INSTRUCTIONS[i].mnemonic, mnemonic) == 0){
            if (INSTRUCTIONS[i].numOfOperands == numOfOperands)
                return 0;
            else
                return -1;
        }
    }
}

int encode(int opcode, int srcReg1, int imm, int srcReg2, int destReg){

    FORMAT opFormat;

    opFormat = INSTRUCTIONS[opcode].format[0];

    switch (opFormat)
    {
        case FORMAT_R:
            /* code */
            break;
        case FORMAT_JR:
            /* code */
            break;
        case FORMAT_B:
            /* code */
            break; 
        case FORMAT_S:
            /* code */
            break;
        default:
            break;
    }
}