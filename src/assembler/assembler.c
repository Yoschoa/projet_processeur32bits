#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "assembler.h"

#define MAX_LABELS 500

// This array stores the individual parts needed to encode an instruction, ie: opcode, register N°s, immValue, address (for braz and branz)
static int instrTokens[MAX_NUM_TOKENS];

// This array stores the Labels identified in the code and thier line positions
static LABEL labelTable[MAX_LABELS];


// Defines the list of instructions and thier various formats
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

void extractTokens(char *opName, char *operandString, int instrTokens[MAX_NUM_TOKENS]){
    
    FORMAT opFormat;
    int opCode;
    const char *delimiters = ", \t";

    char *op1 = strtok(operandString, delimiters);
    char *op2 = strtok(NULL, delimiters);
    char *op3 = strtok(NULL, delimiters);

    for (int i = 0; i < MAX_NUMBER_OF_INSTRUCTIONS; i++){
        if (INSTRUCTIONS[i].mnemonic == opName){
            
            opFormat = INSTRUCTIONS[i].format[0];
            opCode   = INSTRUCTIONS[i].opcode;

            switch (opFormat)
            {
                case FORMAT_R:
                   
    
                    break;
                case FORMAT_JR:
           
                    break;
                case FORMAT_B:
                    instrTokens[OPCODE_INDEX] = opCode;
                    instrTokens[SRC_REG_INDEX1] = strtol(op1 + 1, NULL, 10);
                    for (int k = 0; k < MAX_LABELS; k++){
                        if (op2 != NULL && op2 == labelTable[k].name){
                            instrTokens[SRC_REG_INDEX2] = labelTable[k].lineNum;
                            break;
                        }
                        else{
                            if (op2[0] == 'r' || op2 == 'R'){
                                instrTokens[IMM_FLAG_INDEX] = 0;
                                instrTokens[SRC_REG_INDEX2] = atoi(&op2[1]);

                            }
                            else if (op2[0] == '#'){
                                instrTokens[IMM_FLAG_INDEX] = 1;
                                instrTokens[SRC_REG_INDEX2] = atoi(&op2[1]);
                            }
                            else if (isdigit(op2) || op2[0] == '-'){
                                instrTokens[IMM_FLAG_INDEX] = 1;
                                instrTokens[SRC_REG_INDEX2] = atoi(op2);
                            }

                        }
                    }

                    break; 
                case FORMAT_S:
                    instrTokens[OPCODE_INDEX] = opCode;
                    instrTokens[SRC_REG_INDEX1] = strtol(op1 +1, NULL, 10);
                    for (int j = 2; j <= MAX_NUM_TOKENS; j++) instrTokens[i] = 0;
                    break;
                case FORMAT_H:
                    for (int j = 0; j <= MAX_NUM_TOKENS; j++) instrTokens[i] = 0;
                    break;
                default:
                    break;
    }



        }
        
    }
    

}


int encode(int opcode, int srcReg1, int immFlag, int srcReg2, int destReg, int adress){

    FORMAT opFormat;

    int instr = 0;

    opFormat = INSTRUCTIONS[opcode].format[0];

    switch (opFormat)
    {
        case FORMAT_R:
            instr |= (opcode  & 0x1F)   << 27;
            instr |= (srcReg1 & 0x1F)   << 22;
            instr |= (immFlag & 0x1)    << 21;
            instr |= (srcReg2 & 0xFFFF) << 5;
            instr |= (destReg & 0x1F)   << 0;
            break;
        case FORMAT_JR:
            instr |= (opcode  & 0x1F)   << 27;
            instr |= (srcReg1 & 0x1F)   << 22;
            instr |= (immFlag & 0x1)    << 21;
            instr |= (srcReg2 & 0xFFFF) << 5;
            instr |= (destReg & 0x1F)   << 0;
            break;
        case FORMAT_B:
            instr |= (opcode  & 0x1F)       << 27;
            instr |= (srcReg1 & 0x1F)       << 22;
            instr |= (adress  & 0x3FFFFF)   << 0;
            break; 
        case FORMAT_S:
            instr |= (opcode & 0x1F)        << 27;
            instr |= (adress & 0x7FFFFFF)   << 0;
            break;
        case FORMAT_H:
            instr |= (opcode & 0x1F)        << 27;
            break;
        default:
            break;
    }

    return instr;
}

const char *patternEmpty = "^[ \t]*$";
const char *patternComment = "^[ \t]*(#|//).*$";
const char *patternLabel = "^[ \t]*([a-zA-Z_][a-zA-Z0-9_]*):[ \t]*(#.*|//.*)?$";
const char *patternInstr = "^[ \t]*([a-zA-Z]+)([ \t]+[^;#/]+)?[ \t]*(#.*|//.*)?$";
const char *patternLabelInstr = "^[ \t]*([a-zA-Z_][a-zA-Z0-9_]*):[ \t]+([a-zA-Z]+)([ \t]+[^;#/]+)?[ \t]*(#.*|//.*)?$";

void runASM(FILE *fptr){

    int firstPass = 1, i = 0;
    int locationCounter = 0;
    char line[256];

    // Variables for all regex patterns
    regex_t rexComment, rexEmpty, rexLabel, rexInstr, rexLabelInstr;

    
    int lenLabel;

    char opName[10], operands[50];
    int lenOp, lenOperand;

    regcomp(&rexEmpty, patternEmpty, REG_EXTENDED);
    regcomp(&rexComment, patternComment, REG_EXTENDED);
    regcomp(&rexLabel, patternLabel, REG_EXTENDED);
    regcomp(&rexInstr, patternInstr, REG_EXTENDED);
    regcomp(&rexLabelInstr, patternLabelInstr, REG_EXTENDED);

    do {

        while (fgets(line, sizeof(line), fptr)){

            regmatch_t matches[5];

            if (regexec(&rexEmpty, line, 0, NULL, 0) == 0)
                continue;

            else if (regexec(&rexComment, line, 0, NULL, 0) == 0)
                continue;

            else if (regexec(&rexLabelInstr, line, 5, matches, 0) == 0){
                if (firstPass){
                    // Extract Label in matches[1]
                    lenLabel = matches[1].rm_eo - matches[1].rm_so;
                    strncpy(labelTable[i].name, line + matches[1].rm_so, lenLabel);
                    labelTable[i].lineNum = locationCounter;
                }
                else {
                    // Extract Instruction in matches[2] and operands in matches[3]
                    lenOp = matches[2].rm_eo - matches[2].rm_so;
                    lenOperand = matches[3].rm_eo - matches[3].rm_so;
                    strncpy(opName, line + matches[2].rm_so, lenOp);
                    strncpy(opName, line + matches[3].rm_so, lenOperand);
                }
            }

            else if(regexec(&rexInstr, line, 3, matches, 0) == 0){
            
                if(!firstPass){
                    // Extract instructions in matches[1] and operands in matches[2]
                    lenOp = matches[1].rm_eo - matches[1].rm_so;
                    lenOperand = matches[2].rm_eo - matches[2].rm_so;
                    strncpy(opName, line + matches[2].rm_so, lenOp);
                    strncpy(opName, line + matches[3].rm_so, lenOperand);
                }

            }

            else if(regexec(&rexLabel, line, 2, matches, 0) == 0){
                if (firstPass){
                    // Extract label in matches[1]
                    lenLabel = matches[1].rm_eo - matches[1].rm_so;
                    strncpy(labelTable[i].name, line + matches[1].rm_so, lenLabel);
                    labelTable[i].lineNum = locationCounter;
                }
                else
                    continue;
            }
            i ++;
    }

    i = 0;
    locationCounter = 0;
    rewind(fptr);
    firstPass --;

    }
    while(firstPass <= 0);
    
    regfree(&rexEmpty);
    regfree(&rexComment);
    regfree(&rexLabel);
    regfree(&rexInstr);
    regfree(&rexLabelInstr);

}