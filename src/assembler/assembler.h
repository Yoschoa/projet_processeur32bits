#ifndef ASSEMBLER.H
#define ASSEMBLER.H
#include <stdio.h>

#define MAX_NUMBER_OF_INSTRUCTIONS 19

typedef enum  {
    FORMAT_R,
    FORMAT_I,
    FORMAT_J,
    FORMAT_JR,
    FORMAT_JI,
    FORMAT_B,
    FORMAT_S,
    FORMAT_H,

} FORMAT;

typedef struct  {
    int     opcode;
    char    mnemonic[10];
    FORMAT  format[2];
    int     numOfOperands;
} INSTRUCTION_LAYOUT;

/* Function to validate a line of code in assembly returns 0 if line is valid (in regards to the number of operands needed to execute the instruction called in the line), -1 otherwise*/
int validateCode(char mnemonic[10], int numOfOperands);

/*Function to get the format of and instruction(I, R, JI, JR, etc.) using it's mnemonic and 2nd operand*/
FORMAT getInstructionFormat(char mnemonic[10], char operand[10]);

/* Function to parse/encode instruction and operands into a 8 byte hexadecimal word*/
int encode(int opcode, int srcReg1,int imm, int srcReg2, int srcReg3);


/* Function to run the different steps of the assembler logic*/
void runASM(FILE *fptr);

#endif
