#ifndef ASSEMBLER_H
#define ASSEMBLER_H

// the maximum number of tokens per istructions
#define MAX_TOKENS 4
#define MAX_TOKEN_LENGTH 32;

typedef enum {

    FORMAT_R,
    FORMAT_I,
    FORMAT_B,
    FORMAT_JI,
    FORMAT_JR,

} FORMAT;

struct INSTRUCTION_FORMAT {
    char    *mnemonic;
    int     opCode;
    FORMAT  format;
};



/* Function prototype to fetch a line of code and tokenise it.
The function returns a 2D array of tokens where each row represents tokens per line in .asm file */  
int tokenise(char *line, char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH]);

/* Function prototype to parse each row of the 2D token array into a hexadecimal instruction word*/
int  parseTokens(char tokens[][MAX_TOKEN_LENGTH], int count);

// the function runASM will runs the cycle fetch -> tokenise -> parse / encode -> store in bin file
void runASM();

#endif
