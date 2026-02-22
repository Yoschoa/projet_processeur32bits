#ifndef ASSEMBLER.H
#define ASSEMBLER.H
#include <stdio.h>

#define MAX_NUMBER_OF_INSTRUCTIONS 19
#define MAX_NUM_TOKENS 4

// Define the index used to access elements of the array instrTokens
#define OPCODE_INDEX 0
#define SRC_REG_INDEX1 1
#define IMM_FLAG_INDEX 2
#define SRC_REG_INDEX2 3
#define DEST_REG_INDEX 4


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

typedef struct {
    char name[100];
    int lineNum;
} LABEL;

/* Function procedure to parse the opcode, register numbers, immFlag, etc, then store them in array*/
void extractTokens(char *opName, char *operandString, int instrTokens[MAX_NUM_TOKENS]);

/* Function to parse/encode instruction and operands into a 8 byte hexadecimal word*/
int encode(int opcode, int srcReg1,int immFlag, int srcReg2, int srcReg3, int adress);


/* Function to run the different steps of the assembler logic*/
void runASM(FILE *fptr);

#endif
