#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
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

const char *patternEmpty = "^[ \t]*$";
const char *patternComment = "^[ \t]*(;;|#|//).*$";
const char *patternLabel = "^[ \t]*([a-zA-Z_][a-zA-Z0-9_]*)[ \t]*:[ \t]*(;;.*|#.*|//.*)?$";
const char *patternInstr = "^[ \t]*([a-zA-Z]+)([ \t]+[^;/]+)?[ \t]*(;;.*|#.*|//.*)?$";
const char *patternLabelInstr = "^[ \t]*([a-zA-Z_][a-zA-Z0-9_]*)[ \t]*?:[ \t]+([a-zA-Z]+)([ \t]+[^;/]+)?[ \t]*(;;.*|#.*|//.*)?$";

void extractTokens(char *opName, char *operandString, int instrTokens[MAX_NUM_TOKENS]){
    
    FORMAT opFormat;
    int opCode;
    const char *delimiters = ", \t";

    char *op1 = strtok(operandString, delimiters);
    char *op2 = strtok(NULL, delimiters);
    char *op3 = strtok(NULL, delimiters);




    memset(instrTokens, 0, MAX_NUM_TOKENS * sizeof(int));

    for (int i = 0; i < MAX_NUMBER_OF_INSTRUCTIONS; i++){
        if (strcasecmp(INSTRUCTIONS[i].mnemonic, opName) == 0){
            
            opFormat = INSTRUCTIONS[i].format[0];
            opCode   = INSTRUCTIONS[i].opcode;

            int expectedOps = INSTRUCTIONS[i].numOfOperands;

            // 1. Check for missing operands based on what the instruction NEEDS
            if (expectedOps >= 1 && op1 == NULL) {
                fprintf(stderr, "Error: Instruction '%s' requires at least 1 operand.\n", opName);
                exit(EXIT_FAILURE);
            }
            if (expectedOps >= 2 && op2 == NULL) {
                fprintf(stderr, "Error: Instruction '%s' requires at least 2 operands.\n", opName);
                exit(EXIT_FAILURE);
            }
            if (expectedOps >= 3 && op3 == NULL) {
                fprintf(stderr, "Error: Instruction '%s' requires 3 operands.\n", opName);
                exit(EXIT_FAILURE);

            }

            // 2. Check for TOO MANY operands based on what the instruction NEEDS
            if ((expectedOps == 0 && op1 != NULL) || 
                (expectedOps == 1 && op2 != NULL) || 
                (expectedOps == 2 && op3 != NULL) || 
                (expectedOps == 3 && strtok(NULL, delimiters) != NULL)) {
                
                fprintf(stderr, "Error: Too many operands provided for instruction '%s'.\n", opName);
                exit(EXIT_FAILURE);
            }

            printf("Opcode: %d\n", opCode);
            printf("Operand 1: %s\n", op1 ? op1 : "None");
            printf("Operand 2: %s\n", op2 ? op2 : "None");
            printf("Operand 3: %s\n", op3 ? op3 : "None");
            switch (opFormat)
            {
                case FORMAT_R:
                    instrTokens[OPCODE_INDEX] = opCode;
                    instrTokens[SRC_REG_INDEX1] = atoi(&op1[1]);
                    if (op2[0] == 'r' || op2[0] == 'R'){
                        instrTokens[IMM_FLAG_INDEX] = 0;
                        instrTokens[SRC_REG_INDEX2] = atoi(&op2[1]);
                    }
                    else if(op2[0] == '#'){
                        instrTokens[IMM_FLAG_INDEX] = 1;
                        instrTokens[SRC_REG_INDEX2] = atoi(&op2[1]);
                    }
                    else if(isdigit(op2[0]) || op2[0] == '-'){
                        instrTokens[IMM_FLAG_INDEX] = 1;
                        instrTokens[SRC_REG_INDEX2] = atoi(op2);
                    }
                    instrTokens[DEST_REG_INDEX] = atoi(&op3[1]);
    
                    break;

                case FORMAT_JR:

                    instrTokens[OPCODE_INDEX] = opCode;

                    for (int h = 0; h <= MAX_LABELS; h++){
                        if (strcmp(op1, labelTable[h].name) == 0){
                            instrTokens[IMM_FLAG_INDEX] = 0;
                            instrTokens[SRC_REG_INDEX1] = labelTable[h].lineNum;
                            break;
                        }
                    }
                    if (op1[0] == 'r' || op1[0] == 'R'){
                        instrTokens[IMM_FLAG_INDEX] = 0;
                        instrTokens[SRC_REG_INDEX1] = atoi(&op1[1]);
                    }
                    else if (isdigit(op1[0]) ){
                        instrTokens[IMM_FLAG_INDEX] = 1;
                        instrTokens[SRC_REG_INDEX1] = atoi(op1);
                    }
                    else if (op1[0] == '#'){
                        instrTokens[IMM_FLAG_INDEX] = 1;
                        instrTokens[SRC_REG_INDEX1] = atoi(&op1[1]);
                    }
                    
                    instrTokens[SRC_REG_INDEX2] = 0;
                    if (op2[0] != '\0') instrTokens[DEST_REG_INDEX] = atoi(&op2[1]);

                    break;

                case FORMAT_B:
                    instrTokens[OPCODE_INDEX] = opCode;
                    instrTokens[SRC_REG_INDEX1] = strtol(op1 + 1, NULL, 10);
                    for (int k = 0; k < MAX_LABELS; k++){
                        if (strcmp(op2, labelTable[k].name) == 0){
                            instrTokens[SRC_REG_INDEX2] = labelTable[k].lineNum;
                            break;
                        }
                        else{

                            if (op2[0] == '#'){
                                instrTokens[IMM_FLAG_INDEX] = 1;
                                instrTokens[SRC_REG_INDEX2] = atoi(&op2[1]);
                            }
                            else if (isdigit(op2[0]) || op2[0] == '-'){
                                instrTokens[IMM_FLAG_INDEX] = 1;
                                instrTokens[SRC_REG_INDEX2] = atoi(op2);
                            }

                        }
                    }
                    instrTokens[DEST_REG_INDEX] = 0;

                    break; 
                case FORMAT_S:
                    instrTokens[OPCODE_INDEX] = opCode;
                    instrTokens[SRC_REG_INDEX1] = atoi(&op1[1]);
                case FORMAT_H:
                    instrTokens[OPCODE_INDEX] = opCode;
                    break;
                default:
                    break;
    }


    break;

        }

        
    }
    

}


uint32_t encode(int instrTokens[MAX_NUM_TOKENS]){


    FORMAT opFormat;

    uint32_t instr = 0;

    int opcode = instrTokens[OPCODE_INDEX];
    int srcReg1 = instrTokens[SRC_REG_INDEX1];
    int immFlag = instrTokens[IMM_FLAG_INDEX];
    int srcReg2 = instrTokens[SRC_REG_INDEX2];
    int destReg = instrTokens[DEST_REG_INDEX];

    opFormat = INSTRUCTIONS[opcode].format[0];

    // 16-bit signed bounds check for FORMAT_R and FORMAT_I
    if (immFlag == 1 && (opFormat == FORMAT_R || opFormat == FORMAT_I)) {
        if (srcReg2 < -32768 || srcReg2 > 32767) {
            fprintf(stderr, "Error: Immediate value %d is out of bounds for 16-bit field!\n", srcReg2);
            exit(EXIT_FAILURE);
        }
    }

    switch (opFormat)
    {
        case FORMAT_R:
            instr |= ((uint32_t)(opcode  & 0x1F))   << 27;
            instr |= ((uint32_t)(srcReg1 & 0x1F))   << 22;
            instr |= ((uint32_t)(immFlag & 0x1))    << 21;
            instr |= ((uint32_t)(srcReg2 & 0xFFFF)) << 5;
            instr |= ((uint32_t)(destReg & 0x1F))   << 0;
            break;
        case FORMAT_JR:
            instr |= ((uint32_t)(opcode  & 0x1F))   << 27;
            instr |= ((uint32_t)(srcReg1 & 0x1F))   << 22;
            instr |= ((uint32_t)(immFlag & 0x1))    << 21;
            instr |= ((uint32_t)(srcReg2 & 0xFFFF)) << 5;
            instr |= ((uint32_t)(destReg & 0x1F))   << 0;
            break;
        case FORMAT_B:
            instr |= ((uint32_t)(opcode  & 0x1F))       << 27;
            instr |= ((uint32_t)(srcReg1 & 0x1F))       << 22;
            instr |= ((uint32_t)(srcReg2  & 0x3FFFFF))   << 0;
            break; 
        case FORMAT_S:
            instr |= ((uint32_t)(opcode & 0x1F))        << 27;
            instr |= ((uint32_t)(srcReg1 & 0x7FFFFFF))   << 0;
            break;
        case FORMAT_H:
            instr |= ((uint32_t)(opcode & 0x1F))        << 27;
            break;
        default:
            break;
    }

    return instr;
}


void runASM(FILE *fptr, FILE *outputFile){

    int firstPass = 1, labelIndex = 0;
    int locationCounter = 0;
    char line[256];
    // Variables for all regex patterns
    regex_t rexComment, rexEmpty, rexLabel, rexInstr, rexLabelInstr;

    
    int lenLabel;
    // Variable to hold the instruction word
    int decInstrWrd;

    char opName[10], operands[50];
    int lenOp, lenOperand;

    regcomp(&rexEmpty, patternEmpty, REG_EXTENDED);
    regcomp(&rexComment, patternComment, REG_EXTENDED);
    regcomp(&rexLabel, patternLabel, REG_EXTENDED);
    regcomp(&rexInstr, patternInstr, REG_EXTENDED);
    regcomp(&rexLabelInstr, patternLabelInstr, REG_EXTENDED);

    do {

        while (fgets(line, sizeof(line), fptr)){

            // This searches for '\r' or '\n' and replaces the first one it finds with a null-terminator.
            line[strcspn(line, "\r\n")] = '\0';

            regmatch_t matches[5];
                printf("--------------------------------------------------------------------------------------------------------------\n");
                printf("line: %s\n", line);

            if (regexec(&rexEmpty, line, 0, NULL, 0) == 0)
                continue;

            else if (regexec(&rexComment, line, 0, NULL, 0) == 0)
                continue;

            else if (regexec(&rexLabelInstr, line, 5, matches, 0) == 0){
                if (firstPass == 1){
                    // Extract Label in matches[1]
                    lenLabel = matches[1].rm_eo - matches[1].rm_so;
                    strncpy(labelTable[labelIndex].name, line + matches[1].rm_so, lenLabel);
                    labelTable[labelIndex].lineNum = locationCounter;
                    labelIndex++;
                }
                else {
                    // Extract Instruction in matches[2] and operands in matches[3]
                    lenOp = matches[2].rm_eo - matches[2].rm_so;

                    strncpy(opName, line + matches[2].rm_so, lenOp);
                    opName[lenOp] = '\0'; // Null-terminate the opcode string
            
                    if (matches[3].rm_so != -1) // Check if operands are present
                    {
                        lenOperand = matches[3].rm_eo - matches[3].rm_so;
                         strncpy(operands, line + matches[3].rm_so, lenOperand);
                          operands[lenOperand] = '\0';
                    }
                    else
                    {
                        operands[0] = '\0'; // No operands
                    }
                    
                    printf("Opname: %s\n", opName);
                    printf("Operands: %s\n", operands);
                    extractTokens(opName, operands, instrTokens);
                    decInstrWrd = encode(instrTokens);
                    printf("Instruction Word: %d\n", decInstrWrd);
                    fprintf(outputFile, "0x%08X 0x%08X\n", locationCounter, decInstrWrd);
                    
                }

                locationCounter ++;
                
            }

            else if(regexec(&rexInstr, line, 3, matches, 0) == 0){
            
                if(firstPass == 0){
                    // Extract instructions in matches[1] and operands in matches[2]
                    lenOp = matches[1].rm_eo - matches[1].rm_so;
                    strncpy(opName, line + matches[1].rm_so, lenOp);
                    opName[lenOp] = '\0'; 

                    if (matches[2].rm_so != -1) // Check if operands are present
                    {
                        lenOperand = matches[2].rm_eo - matches[2].rm_so;
                         strncpy(operands, line + matches[2].rm_so, lenOperand);
                          operands[lenOperand] = '\0';
                    }
                    else
                    {
                        operands[0] = '\0'; // No operands
                    }
                   
                   
                    printf("Opname: %s\n", opName);
                    printf("Operands: %s\n", operands);
                    extractTokens(opName, operands, instrTokens);
                    decInstrWrd = encode(instrTokens);
                    printf("Instruction Word: %d\n", decInstrWrd);
                    fprintf(outputFile, "0x%08X 0x%08X\n", locationCounter, decInstrWrd);
                }
                locationCounter ++;

            }

            else if(regexec(&rexLabel, line, 2, matches, 0) == 0){
                if (firstPass == 1){
                    // Extract label in matches[1]
                    lenLabel = matches[1].rm_eo - matches[1].rm_so;
                    strncpy(labelTable[labelIndex].name, line + matches[1].rm_so, lenLabel);
                    labelTable[labelIndex].lineNum = locationCounter;
                    labelIndex++;
                }
                else
                    continue;
            }

            else {
                
                printf("Syntax error at line: %s\n", line);
                exit(EXIT_FAILURE);
            }

            
    }

    locationCounter = 0;
    rewind(fptr);
    firstPass --;
    if (firstPass == 0){
        printf("First pass completed. Label table:\n");
        for (int i = 0; i < labelIndex; i++){
            printf("Label: %s, Line: %d\n", labelTable[i].name, labelTable[i].lineNum);
        }

    }
}
    while(firstPass >= 0);
    
    regfree(&rexEmpty);
    regfree(&rexComment);
    regfree(&rexLabel);
    regfree(&rexInstr);
    regfree(&rexLabelInstr);

}


/* int main (int argc, char *argv[]){

    if (argc != 2){
        fprintf(stderr, "Usage: %s <assembly_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *fptr = fopen(argv[1], "r");
    if (fptr == NULL){
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    FILE *outputFile = fopen("./output.bin", "wb");
    if (outputFile == NULL) {
        fprintf(stderr, "Error creating output file.\n");
        fclose(fptr);
        return EXIT_FAILURE;
    }

    runASM(fptr, outputFile);

    fclose(fptr);
    fclose(outputFile);
    return EXIT_SUCCESS;

} */