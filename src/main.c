#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "args.h"
#include "preprocessor/preprocessor.h"
#include "preprocessor/token.h"




void parse_token(char* tokenBuffer, size_t bufferEndIndex, size_t bufferLen) {
    if (bufferEndIndex == 0) return;
    printf("%s\n", tokenBuffer);
}



int main(int argc, char* argv[]) {
    // printf("\033[1m\033[32m START_LEXER \033[0m \n");

    char *file = "../tests/basic.c";

    Preprocessor *preprocessor = preprocessor_init(file);
    preprocessor_run(preprocessor);

    for (int i = 0; i < preprocessor->n_tokens; i++) {
        print_token(&preprocessor->tokens[i]);
    }

    preprocessor_destruct(preprocessor);

    return 0;
}


