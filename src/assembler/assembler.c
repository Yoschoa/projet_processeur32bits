#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"


static const INSTRUCTION_FORMAT instructions[] = { {"STOP", 0, FORMAT_H}, {"ADD", 1, FORMAT_R}, {"SUB", 2, FORMAT_R}, {"MUL", 3, FORMAT_R}, {"DIV", 4, FORMAT_R}, {"AND", 4, FORMAT_R} }


int tokenise(char *line, char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH]){
   

}
