#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "args.h"
#include "tok/tok.h"





void parse_token(char* tokenBuffer, size_t bufferEndIndex, size_t bufferLen) {
    if (bufferEndIndex == 0) return;
    printf("%s\n", tokenBuffer);
}



int main(int argc, char* argv[]) {
    // printf("\033[1m\033[32m START_LEXER \033[0m \n");

    size_t bufferLen = 1024;
    size_t bufferEndIndex = 0;
    char* tokenBuffer = malloc(bufferLen);

    if (tokenBuffer == NULL) {
        puts("ERR: Allocated token buffer is nullptr!");
        exit(EXIT_FAILURE);
    }

    memset(tokenBuffer, 0, bufferLen);


    char prevChar = 0;
    char c = 0;

    for (size_t i = 0; i < 1024; i++) {
        prevChar = c;
        c = getchar();

        switch (c) {
        case EOF:
            parse_token(tokenBuffer, bufferEndIndex, bufferLen);
            free(tokenBuffer);
            return 0;

        case ' ':
        case '\t':
        case '\n':
        case '\r':
        case '\a':
            parse_token(tokenBuffer, bufferEndIndex, bufferLen);
            memset(tokenBuffer, 0, bufferLen);
            bufferEndIndex = 0;
            break;

        default:
            tokenBuffer[bufferEndIndex] = c;
            bufferEndIndex++;

            if (bufferEndIndex >= bufferLen) {
                bufferLen *= 2;
                tokenBuffer = realloc(tokenBuffer, bufferLen);

                if (tokenBuffer == NULL) {
                    puts("ERR: Reallocated token buffer is nullptr!");
                    exit(EXIT_FAILURE);
                }
            }
            break;
        }
    }

    // printf("\033[1m\033[32m END_LEXER \033[0m \n");
    return 0;
}


