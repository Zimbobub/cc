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
    // for (int i = 0; i < argc; i++) {
    //     printf("%s\n", argv[i]);
    // }
    // return 0;

    char *input_file;
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

    printf("Compiling %s\n", input_file);

    char *preprocessed_file = strdup(input_file);
    char *assembly_file = strdup(input_file);
    char *object_file = strdup(input_file);
    char *exec_file = strndup(input_file, strlen(input_file-2)); // no file extension

    // change file extensions
    preprocessed_file[strlen(preprocessed_file)-1] = 'i';
    assembly_file[strlen(assembly_file)-1] = 's';
    object_file[strlen(object_file)-1] = 'o';



    // preprocess
    {
        char preprocess_cmd_format[] = "gcc -E -P  -o ";
        char* command_buf = malloc(strlen(preprocess_cmd_format) + strlen(input_file) + strlen(preprocessed_file) + 1);
        sprintf(command_buf, "gcc -E -P %s -o %s", input_file, preprocessed_file);

        printf("Running %s\n", command_buf);
        if (system(command_buf) != 0) {
            return EXIT_FAILURE;
        }
        free(command_buf);
        printf("Preprocessor succeeded\n");
    }

    // lexer
    Lexer *lexer = lexer_init(preprocessed_file);
    lexer_run(lexer);

    if (lexer->err) {
        printf("LEXER ERROR: %s\n", lexer->err_msg);
    } else {
        for (int i = 0; i < lexer->n_tokens; i++) {
            print_token(&lexer->tokens[i]);
        }
    }

    if (stage == STAGE_LEXER || lexer->err) {
        bool err = lexer->err; // prevent use after free
        lexer_destruct(lexer);
        free(preprocessed_file);
        free(assembly_file);
        free(object_file);
        free(exec_file);
        return (err ? EXIT_FAILURE : EXIT_SUCCESS);
    }

    // parser


    if (stage == STAGE_PARSER) {
        lexer_destruct(lexer);
        free(preprocessed_file);
        free(assembly_file);
        free(object_file);
        free(exec_file);
        return EXIT_SUCCESS;
    }

    // asm gen


    if (stage == STAGE_CODEGEN) {
        lexer_destruct(lexer);
        free(preprocessed_file);
        free(assembly_file);
        free(object_file);
        free(exec_file);
        return EXIT_SUCCESS;
    }

    // assemble and link
    {
        char assemble_cmd_format[] = "gcc -o ";
        char* command_buf = malloc(strlen(assemble_cmd_format) + strlen(assembly_file) + strlen(exec_file) + 1);
        sprintf(command_buf, "gcc %s -o %s", assembly_file, exec_file);

        printf("Running %s\n", command_buf);
        if (system(command_buf) != 0) {
            return EXIT_FAILURE;
        }
        free(command_buf);
        printf("Assembly succeeded\n");
    }

    // cleanup
    lexer_destruct(lexer);
    free(preprocessed_file);
    free(assembly_file);
    free(object_file);
    free(exec_file);
    printf("success\n");

    return EXIT_SUCCESS;
}


