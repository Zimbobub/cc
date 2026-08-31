#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "frontend/lexer/lexer.h"
#include "frontend/lexer/token.h"
#include "frontend/parser/parser.h"
#include "frontend/to_IR/to_IR.h"
#include "IR/IR.h"
#include "backend/codegen/codegen.h"
#include "backend/codegen/replace_pseudo_regs.h"
#include "backend/codegen/fixup_instructions.h"
#include "backend/code_emission/emission.h"

typedef enum {
    STAGE_LEXER,
    STAGE_PARSER,
    STAGE_IR,
    STAGE_CODEGEN,
    STAGE_CODE_EMISSION
} Stage;


void print_usage() {
    printf("Usage: cc [stage] <filename.c> \nStage: '--lex' | '--parse' | '--codegen'\nIf stage not given, defaults to code emission\n");
}

// pass in '\0' for no extension
char* with_extension(const char* filename, char extension) {
    size_t len = strlen(filename);
    char* new_str = malloc(len+1);
    if (new_str == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }

    strcpy(new_str, filename);
    if (extension == '\0') {
        new_str[len-1] = '\0';
        new_str[len-2] = '\0';
    } else {
        new_str[len-1] = extension;
    }

    return new_str;
}



void cleanup_files(const char* preprocessed_file, const char* assembly_file) {
    char *cmd_buf = malloc(6 + strlen(preprocessed_file) + strlen(assembly_file));
    sprintf(cmd_buf, "rm %s %s", preprocessed_file, assembly_file);
    system(cmd_buf);
    free(cmd_buf);
}


void run_preprocessor(const char* input_file, const char* preprocessed_file) {
    char preprocess_cmd_format[] = "gcc -E -P  -o ";
    char* command_buf = malloc(strlen(preprocess_cmd_format) + strlen(input_file) + strlen(preprocessed_file) + 1);
    sprintf(command_buf, "gcc -E -P %s -o %s", input_file, preprocessed_file);

    printf("Running %s\n", command_buf);
    if (system(command_buf) != 0) {
        exit(EXIT_FAILURE);
    }
    free(command_buf);
    printf("Preprocessor succeeded\n");
}


void run_assembler(const char* assembly_file, const char* exec_file) {
    char preprocess_cmd_format[] = "gcc -o ";
    char* command_buf = malloc(strlen(preprocess_cmd_format) + strlen(assembly_file) + strlen(exec_file) + 1);
    sprintf(command_buf, "gcc %s -o %s", assembly_file, exec_file);

    printf("Running %s\n", command_buf);
    if (system(command_buf) != 0) {
        exit(EXIT_FAILURE);
    }
    free(command_buf);
    printf("Assembly succeeded\n");
}


int main(int argc, char* argv[]) {
    char *input_file;
    Stage stage = STAGE_CODE_EMISSION;

    if (argc == 2) {
        input_file = argv[1];
    } else if (argc == 3) {
        input_file = argv[2];

        if (strcmp(argv[1], "--lex") == 0) stage = STAGE_LEXER;
        else if (strcmp(argv[1], "--parse") == 0) stage = STAGE_PARSER;
        else if (strcmp(argv[1], "--tacky") == 0) stage = STAGE_IR;
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



    char *preprocessed_file = with_extension(input_file, 'i');
    char *assembly_file = with_extension(input_file, 's');
    char *exec_file = with_extension(input_file, '\0'); // no file extension


    // preprocess
    run_preprocessor(input_file, preprocessed_file);

    // lexer
    TokenBuf tokens = lexer_run(preprocessed_file);
    print_tokens(&tokens);
    if (stage == STAGE_LEXER) return EXIT_SUCCESS;

    // parser
    CProgram ast = parse_program(tokens);
    print_c_program(&ast);
    if (stage == STAGE_PARSER) return EXIT_SUCCESS;

    // ir
    IRProgram ir = IR_transform_ast(ast);
    print_IR_program(&ir);
    if (stage == STAGE_IR) return EXIT_SUCCESS;



    // asm gen
    AsmProgram asm_ast = transform_program(ir);
    print_asm_program(&asm_ast);
    replace_pseudo_registers(&asm_ast);
    print_asm_program(&asm_ast);
    fixup_program(&asm_ast);
    print_asm_program(&asm_ast);
    if (stage == STAGE_CODEGEN) return EXIT_SUCCESS;

    // emit code
    char* assembly = emit_asm_program(&asm_ast);
    printf("%s\n", assembly);

    FILE* out = fopen(assembly_file, "w");
    if (out == NULL) {
        printf("Unable to open assembly file (%s)\n", assembly_file);
        return EXIT_FAILURE;
    }
    fwrite(assembly, sizeof(char), strlen(assembly), out);
    fclose(out);

    // assemble and link
    run_assembler(assembly_file, exec_file);

    printf("success\n");
    return EXIT_SUCCESS;
}


