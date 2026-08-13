#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "lexer/lexer.h"
#include "lexer/token.h"




typedef enum {
    STAGE_LEXER,
    STAGE_PARSER,
    STAGE_CODEGEN,
    STAGE_CODE_EMISSION
} Stage;


void print_usage() {
    printf("Usage: cc [stage] <filename.c> \nStage: '--lex' | '--parse' | '--codegen'\nIf stage not given, defaults to code emission\n");
}


int main(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
    // return 0;


    char *input_file;
    char *output_file;
    Stage stage = STAGE_CODE_EMISSION;

    if (argc == 2) {
        input_file = argv[1];
    } else if (argc == 3) {
        input_file = argv[2];

        if (strcmp(argv[1], "--lex") == 0) stage = STAGE_LEXER;
        else if (strcmp(argv[1], "--parse") == 0) stage = STAGE_PARSER;
        else if (strcmp(argv[1], "--codegen") == 0) stage = STAGE_CODEGEN;
        else {
            printf("Unknown stage '%s'\n", argv[1]);
            print_usage();
            return EXIT_FAILURE;
        }
    } else {
        printf("Must have 1 or 2 arguments\n");
        print_usage();
        return EXIT_FAILURE;
    }

    if (access(input_file, F_OK) != 0) {
        printf("File does not exist\n");
        return EXIT_FAILURE;
    }



    // // if user enters -h, or uses incorrect args
    // if ((argc >= 2 && strcmp(argv[1], "-h") == 0) || (argc != 3 && argc != 4)) {
    //     printf("Usage: cc <filename.c> <output.s> <stage>\nStage: 'lex' | 'parse' | 'codegen'\nIf stage not given, defaults to code emission\n");
    //     return EXIT_FAILURE;
    // }

    // char *input_file = argv[1];
    // char *output_file = argv[2];
    // Stage stage = STAGE_CODE_EMISSION;
    // if (argc == 4) {
    //     if (strcmp(argv[3], "lex") == 0) stage = STAGE_LEXER;
    //     else if (strcmp(argv[3], "parse") == 0) stage = STAGE_PARSER;
    //     else if (strcmp(argv[3], "codegen") == 0) stage = STAGE_CODEGEN;
    //     else {
    //         printf("Usage: cc <filename.c> <output.s> <stage>\nStage: 'lex' | 'parse' | 'codegen'\nIf stage not given, defaults to code emission\n");
    //         return EXIT_FAILURE;
    //     }
    // }

    Lexer *lexer = lexer_init(input_file);
    lexer_run(lexer);

    // for (int i = 0; i < lexer->n_tokens; i++) {
        // print_token(&lexer->tokens[i]);
    // }

    lexer_destruct(lexer);

    printf("success\n");

    return EXIT_SUCCESS;
}


