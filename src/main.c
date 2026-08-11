#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "args.h"
#include "lexer/lexer.h"
#include "lexer/token.h"




typedef enum {
    STAGE_LEXER,
    STAGE_PARSER,
    STAGE_CODEGEN,
    STAGE_CODE_EMISSION
} Stage;



int main(int argc, char* argv[]) {
    // if user enters -h, or uses incorrect args
    if ((argc >= 2 && strcmp(argv[1], "-h") == 0) || (argc != 3 && argc != 4)) {
        printf("Usage: cc <filename.c> <output.s> <stage>\nStage: 'lex' | 'parse' | 'codegen'\nIf stage not given, defaults to code emission\n");
        return EXIT_FAILURE;
    }

    char *input_file = argv[1];
    char *output_file = argv[2];
    Stage stage = STAGE_CODE_EMISSION;
    if (argc == 4) {
        if (strcmp(argv[3], "lex") == 0) stage = STAGE_LEXER;
        else if (strcmp(argv[3], "parse") == 0) stage = STAGE_PARSER;
        else if (strcmp(argv[3], "codegen") == 0) stage = STAGE_CODEGEN;
        else {
            printf("Usage: cc <filename.c> <output.s> <stage>\nStage: 'lex' | 'parse' | 'codegen'\nIf stage not given, defaults to code emission\n");
            return EXIT_FAILURE;
        }
    }

    Lexer *lexer = lexer_init(input_file);
    lexer_run(lexer);

    for (int i = 0; i < lexer->n_tokens; i++) {
        print_token(&lexer->tokens[i]);
    }

    lexer_destruct(lexer);

    return 0;
}


